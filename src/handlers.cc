#include "handlers.hpp"
#include "embeds.hpp"
#include "config.hpp"
#include "utils.hpp"
#include <nlohmann/json.hpp>

void message_create_handler(aegis::gateway::events::message_create obj) {
    try {
        //get snowflakes related to this message
        // C++17 version
        //const auto [channel_id, guild_id, message_id, member_id] = obj.msg.get_related_ids();
        const aegis::snowflake channel_id = obj.msg.get_channel().get_id();
        const aegis::snowflake guild_id = obj.msg.get_guild().get_id();
        const aegis::snowflake message_id = obj.msg.get_id();
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
        auto & _guild = _channel.get_guild();
        auto & username = obj.msg.author.username;

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
        } else {
            _channel.create_message_embed(mention, make_error_obj("Command not found."));
        }
    } catch (std::exception &e) {
        std::cout << "Error: " << e.what() << '\n';
    }
    return;
}