#include "embeds.hpp"
#include "config.hpp"
#include "item_names.hpp"

const json make_info_obj(aegis::core & bot, aegis::shards::shard * _shard)
{
    int64_t guild_count = bot.get_guild_count();
    int64_t user_count_unique = bot.get_user_count();
    int64_t channel_count = bot.get_channel_count();

    int64_t eventsseen = 0;

    for (auto & e : bot.message_count)
        eventsseen += e.second;

    // std::string members = fmt::format("{}", user_count_unique);
    // std::string channels = fmt::format("{}", channel_count);
    std::string guilds = fmt::format("{}", guild_count);
    // std::string events = fmt::format("{}", eventsseen);

    size_t thread_count = bot.threads.size();

    std::string shard_info = fmt::format("Shard # {} of {} running on `{}` with {} threads", _shard->get_id() + 1, bot.shard_max_count, aegis::utility::platform::get_platform(), thread_count);

    std::string memory = fmt::format("Current: {:.2f}MiB\nPeak: {:.2f}MiB", 
        double(aegis::utility::getCurrentRSS()) / (1024 * 1024), 
        double(aegis::utility::getPeakRSS()) / (1024 * 1024)
    );

    json t = {
        { "title", "Bazcal Info" },
        { "color", 0xff6e5e },
        { "fields", json::array(
            {
                { { "name", "Invite" },{ "value", "You can invite Bazcal to your own server [by clicking here](https://discord.com/oauth2/authorize?client_id=715462011256832090&permissions=8&scope=bot)" } },
                { { "name", "Support" },{ "value", "Need support or have a suggestion for Bazcal? [Join our support server](https://discord.gg/QVQffMa)" } },
                // { { "name", "Members" },{ "value", members },{ "inline", true } },
                // { { "name", "Channels" },{ "value", channels },{ "inline", true } },
                { { "name", "Uptime" },{ "value", bot.uptime_str() },{ "inline", true } },
                { { "name", "Guilds" },{ "value", guilds },{ "inline", true } },
                // { { "name", "Events Seen" },{ "value", events },{ "inline", true } },
                { { "name", "Memory" },{ "value", memory },{ "inline", true } },
                { { "name", "Runtime Info" },{ "value", shard_info },{ "inline", false } },
                { { "name", "Library Info" },{ "value", fmt::format("C++{}, [{}](https://github.com/zeroxs/aegis.cpp)", CXX_VERSION, AEGIS_VERSION_TEXT) } },
                { { "name", "License" },{ "value", "Bazcal is released under the [AGPL-3.0](https://github.com/Wykerd/bazcal-lf/blob/master/LICENSE) license. Source is available on [Github](https://github.com/Wykerd/bazcal-lf)" } }
            })
        },
        { "footer",{ { "text", "Running Bazcal " BAZCAL_VER_SERIALIZED " (codename Light Falcon)" } } }
    };
    return t;
}

const json make_help_obj () {
    json t = {
        { "title", "Bazcal Help" },
        { "description", "All of Bazcal's commands are prefixed with `" BAZCAL_PREFIX "`" },
        { "color", 0xff6e5e },
        { "fields",
        json::array(
            {
                { { "name", "**`bazaar` (aliases: advise, advice, a)**" },{ "value", "Simple bazaar flipping command.\nParams: <amount>" } },
                { { "name", "**`auction` (aliases: ahflip, ahf, af)**" },{ "value", "Randomized auction house flipping command\nParams: [max_bid] [min_profit]" } },
                { { "name", "**`info` (aliases: about, stats)**" },{ "value", "Sends bot info" } }
            }
            )
        },
        { "footer",{ { "text", "Running Bazcal " BAZCAL_VER_SERIALIZED " (codename Light Falcon)" } } }
    };
    return t;
}

const json make_error_obj (std::string error) {
    json t = {
        { "title", "Error" },
        { "description", error },
        { "color", 0xff6e5e },
    };
    return t;
}

const json make_bazaar_obj (bz_bazaar_advice_buf_t *baz) {
    std::vector<json> fields;

    size_t s = baz->count >= 6 ? 6 : baz->count;

    for (size_t i = 0; i < s; i++) {
        auto item = item_names.find(baz->items[i]->name); 
        std::string pretty_name;
        if (item != item_names.end()) {
            pretty_name = item->second;
        } else {
            pretty_name = baz->items[i]->name;
            std::replace(pretty_name.begin(), pretty_name.end(), '_', ' ');
        };

        fields.push_back({
            { "name", fmt::format("`{}` {}", i + 1, pretty_name) },
            { 
                "value", 
                fmt::format(
                    "```yaml\nQuantity:\n{:.0f}\nEstimated Profit:\n{:.0f} [{:.2f}%]\nInvested:\n{:.0f} [{:.2f}%]\n```",
                    baz->items[i]->evolume,
                    baz->items[i]->eprofit,
                    baz->items[i]->pprofit * 100,
                    baz->items[i]->invested,
                    baz->items[i]->pinvested * 100
                ) 
            },
            { "inline", true }
        });
    }

    json t = {
        { "title", "Bazaar Flips" },
        { "description", "Create buy orders for the items below, wait for them to fill. When orders are filled, immediately create sell orders." },
        { "color", 0x54B4E3 },
        { "footer",{ { "text", "This data is updated about every 15 seconds" } } },
        { "fields", json(fields) }
    };

    return t;
}

const json make_auction_flips_obj (bz_auction_pool_t *flips) {
    std::vector<json> fields;
    
    for (size_t i = 0; i < flips->size; i++) {
        std::string uuid(flips->candidates[i]->item->uuid);
        fields.push_back({
            { "name", flips->candidates[i]->item->name.full },
            { 
                "value", 
                fmt::format(
                    "```yaml\nBid Price: {}\nEstimated Profit: {:.0f}\nPredicted Sell Price: {:.0f}\nDataset Size: {}\n``` ```/viewauction {}```", 
                    flips->candidates[i]->item->max_bid, 
                    flips->candidates[i]->profit, 
                    flips->candidates[i]->predicted->value, 
                    flips->candidates[i]->predicted->n, 
                    uuid.substr(0,8)+"-"+uuid.substr(8,4)+"-"+uuid.substr(12,4)+"-"+uuid.substr(16,4)+"-"+uuid.substr(20)
                ) 
            }
        });
    };

    json t = {
        { "title", "Auction Flips" },
        { "description", "Bid on one of the below items and put back up for auction when you won the bidding. Use your brain to determine when to stop raising your bid. Predicted profits might be lower" },
        { "color", 0xAAE5E6 },
        { "footer",{ { "text", "Bazcal doesn't recommend the best trades to prevent bid wars." } } },
        { "fields", json(fields) }
    };
    return t;
};