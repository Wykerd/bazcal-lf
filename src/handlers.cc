#include "handlers.hpp"
#include "embeds.hpp"
#include "config.hpp"
#include "utils.hpp"
#include <nlohmann/json.hpp>
#include <curl/curl.h>

static size_t predictions_len = 0;
static bz_prediction_t **predictions = NULL;
static size_t pool_len = 0;;
static bz_auction_pool_t **pool = NULL;

bz_bazaar_t *bazaar_data = NULL;

void auction_load_cached (const char* filename) {
    FILE *fd;
    if ((fd = fopen(BAZCAL_PREDICTION_DUMP_FILE, "r"))) {
        fread(&predictions_len, sizeof(size_t), 1, fd);

        predictions = (bz_prediction_t **)malloc(sizeof(bz_prediction_t *) * predictions_len);

        for (size_t i = 0; i < predictions_len; i++) {
            predictions[i] = (bz_prediction_t *)malloc(sizeof(bz_prediction_t));

            size_t name_len;
            fread(&name_len, sizeof(size_t), 1, fd);

            predictions[i]->item_name = (char *)malloc(sizeof(char) * (name_len + 1));

            fread(predictions[i]->item_name, sizeof(char), name_len, fd);

            fread(&predictions[i]->value, sizeof(double), 1, fd);
            fread(&predictions[i]->n, sizeof(size_t), 1, fd);
        };
    }
};

void bazaar_loop_callback(bz_bazaar_t *data) {
    printf("[libbazcal] Got bazaar items.\n");
    // swop in new bazaar data
    bz_bazaar_t *_bazaar_data = bazaar_data;
    bazaar_data = data;
    if (_bazaar_data != NULL) bz_free_bazaar(_bazaar_data);
}

void loop_callback (sqlite3 *db) {
    // Get the new values
    size_t __predictions_len = 0;
    printf("[libbazcal] Generating auction predictions...\n");
    bz_prediction_t **__predictions = bz_generate_predictions(db, &__predictions_len);
    printf("[libbazcal] Generated %zu predictions for items\n", __predictions_len);

    // Save the old pointers
    bz_prediction_t **___predictions = predictions;
    size_t ___predictions_len = predictions_len;
    // Swop in new values
    predictions_len = __predictions_len;
    predictions = __predictions;
    // free the old pointers
    if (predictions != NULL) bz_free_predictions(___predictions, ___predictions_len);

    for (size_t i = 0; i < predictions_len; i++) {
        printf("[libbazcal] Prediction: %s,%d,%.2f\n", predictions[i]->item_name, predictions[i]->n, predictions[i]->value);
    }

    // Get the new values
    size_t __pool_len = 0;
    bz_auction_pool_t **__pool = bz_populate_auction_pool(db, predictions, predictions_len, &__pool_len);

    // Save the old pointers
    bz_auction_pool_t **___pool = pool;
    size_t ___pool_len = pool_len;
    // Swop in new
    pool = __pool;
    pool_len = __pool_len;
    // Free old
    if (pool != NULL) bz_free_auction_pool(___pool, ___pool_len);

    // Call webhook to notify about update
    json embed = {
        { "embeds", json::array({
            {
                { "title", "Auction Update" },
                { "color", 0xff6e5e },
                { "description", fmt::format("Auction predicted prices recalculated\nPrices for {} items were calculated", predictions_len) },
                { "footer",{ { "text", "Running Bazcal " BAZCAL_VER_SERIALIZED " (codename Light Falcon)" } } },
            }
        }) }
    };

    CURLcode ret;
    CURL *hnd;
    struct curl_slist *headers;

    headers = NULL;
    headers = curl_slist_append(headers, "Accept: application/json");
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, "charset: utf-8");

    hnd = curl_easy_init();
    std::string jsonstr = embed.dump();
    curl_easy_setopt(hnd, CURLOPT_URL, BAZCAL_AUCTION_HOOK);
    curl_easy_setopt(hnd, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(hnd, CURLOPT_POSTFIELDS, jsonstr.c_str());
    curl_easy_setopt(hnd, CURLOPT_POSTFIELDSIZE, jsonstr.length());
    curl_easy_setopt(hnd, CURLOPT_USERAGENT, "bazcal/3.0");
    curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(hnd, CURLOPT_MAXREDIRS, 50L);
    curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "POST");
    curl_easy_setopt(hnd, CURLOPT_TCP_KEEPALIVE, 1L);

    ret = curl_easy_perform(hnd);

    curl_easy_cleanup(hnd);
    hnd = NULL;
    curl_slist_free_all(headers);
    headers = NULL;

    FILE *fd;

    fd = fopen(BAZCAL_PREDICTION_DUMP_FILE, "w");
    
    fwrite(&predictions_len, 1, sizeof(size_t), fd);

    for (size_t i = 0; i < predictions_len; i++) {
        size_t name_len = strlen(predictions[i]->item_name);

        fwrite(&name_len, 1, sizeof(size_t), fd);
        fwrite(predictions[i]->item_name, name_len, sizeof(char), fd);
        fwrite(&predictions[i]->value, 1, sizeof(double), fd);
        fwrite(&predictions[i]->n, 1, sizeof(size_t), fd);
    };
    
    fclose(fd);
}

/*
bz_auction_pool_t *random_flips = bz_random_auction_flips(pool, pool_len, 0, 0, RAND_MAX, 50, 6, NULL);
printf("[libbazcal] %zu random predictions\n", random_flips->size);
bz_free_random_auction_flips(random_flips);
*/

void message_create_handler(aegis::gateway::events::message_create obj) {
    try {
        if (obj.msg.get_content() == "") return; // ignore empty messages

        aegis::core & bot = obj.msg.get_guild().get_bot();
        const std::string mention = obj.msg.get_user().get_mention();
#if !defined(AEGIS_DISABLE_ALL_CACHE)
        const aegis::snowflake member_id = obj.msg.get_user().get_id();
#else
        const aegis::snowflake member_id = obj.msg.author.id;
#endif

        // Is message author myself?
        if (member_id == bot.get_id()) return;

        // Ignore bot messages and DMs
        if (obj.msg.is_bot() || !obj.msg.has_guild()) return;

        auto & _channel = obj.msg.get_channel();

        std::string content{obj.msg.get_content()};
        // Transform command to lowercase
        std::transform(content.begin(), content.end(), content.begin(), [](unsigned char c){ return std::tolower(c); });

        std::vector<std::string> command_args = split(content, ' ');

        if (command_args[0] != BAZCAL_PREFIX) return;

        if (command_args.size() < 2) {
            // send help
            _channel.create_message_embed(mention, make_help_obj());
            return;
        }

        if (command_args[1] == "help") {
            _channel.create_message_embed(mention, make_help_obj());
        } else if (command_args[1] == "stats" || command_args[1] == "info" || command_args[1] == "about") {
            _channel.create_message_embed(mention, make_info_obj(bot, &obj.shard));
        } else if (command_args[1] == "auction" || command_args[1] == "ahflip" || command_args[1] == "ahf" || command_args[1] == "af") {
            double max_bid = 0;
            double min_profit = 2000;

            if (command_args.size() > 2) {
                max_bid = parse_float(command_args[2]);
            }

            if (command_args.size() > 3) {
                min_profit = parse_float(command_args[3]);
            }

            bz_auction_pool_t *random_flips = bz_random_auction_flips(pool, pool_len, max_bid, min_profit, 600000, 60000, 100, 3, NULL);

            if (random_flips->size) _channel.create_message_embed(mention, make_auction_flips_obj(random_flips));
            else _channel.create_message_embed(mention, make_error_obj("No results returned, try lowering the minimum profit."));

            bz_free_random_auction_flips(random_flips);
            //advise, advice, a
        } else if (command_args[1] == "bazaar" || command_args[1] == "advise" || command_args[1] == "advice" || command_args[1] == "a") {
            if (command_args.size() < 2) {
                _channel.create_message_embed(mention, make_error_obj("Invalid amount of arguments: Expected 1 got 0."));
                return;
            }

            int balance = (int)parse_float(command_args[2]);

            bz_bazaar_advice_buf_t *items = bz_advise(bazaar_data, balance, 5);

            _channel.create_message_embed(mention, make_bazaar_obj(items));

            bz_free_advise(items);
        } /*else if (command_args[1] == "test") {
            using namespace aegis::gateway::objects;

            permission_overwrite everyone_permissions;
            everyone_permissions.id = guild_id;
            everyone_permissions.type = overwrite_type::Role;
            everyone_permissions.deny = 0x00000400;

            _guild.create_text_channel("blf_test", 0, false, {
                everyone_permissions
            });
        }*/ else {
            _channel.create_message_embed(mention, make_error_obj("Command not found."));
        }
    } catch (std::exception &e) {
        std::cout << "Error: " << e.what() << '\n';
    }
    return;
}