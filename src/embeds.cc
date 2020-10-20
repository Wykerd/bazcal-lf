#include "embeds.hpp"
#include "config.hpp"

const json make_info_obj(aegis::core & bot, aegis::shards::shard * _shard)
{
    int64_t guild_count = bot.get_guild_count();
    int64_t user_count_unique = bot.get_user_count();
    int64_t channel_count = bot.get_channel_count();

    int64_t eventsseen = 0;

    for (auto & e : bot.message_count)
        eventsseen += e.second;

    std::string members = fmt::format("{}", user_count_unique);
    std::string channels = fmt::format("{}", channel_count);
    std::string guilds = fmt::format("{}", guild_count);
    std::string events = fmt::format("{}", eventsseen);

size_t thread_count = bot.threads.size();

#if defined(DEBUG) || defined(_DEBUG)
    std::string misc = fmt::format("Shard # {} of {} running on `{}` in `DEBUG` mode with {} threads", _shard->get_id() + 1, bot.shard_max_count, aegis::utility::platform::get_platform(), thread_count);

    std::string memory = fmt::format("{:.2f}MiB\nMax: {:.2f}MiB", double(aegis::utility::getCurrentRSS()) / (1024 * 1024), double(aegis::utility::getPeakRSS()) / (1024 * 1024));
#else
    std::string shard_info = fmt::format("Shard # {} of {} running on `{}` with {} threads", _shard->get_id(), bot.shard_max_count, aegis::utility::platform::get_platform(), thread_count);

    std::string memory = fmt::format("{:.2f}MiB", double(aegis::utility::getCurrentRSS()) / (1024 * 1024));
#endif

    json t = {
        { "title", "Bazcal Info" },
        { "color", 0xff6e5e },
        { "fields", json::array(
            {
                { { "name", "Invite" },{ "value", "You can invite Bazcal to your own server [by clicking here](https://discord.com/oauth2/authorize?client_id=715462011256832090&permissions=8&scope=bot)" } },
                { { "name", "Support" },{ "value", "Need support or have a suggestion for Bazcal? [Join our support server](https://discord.gg/QVQffMa)" } },
                { { "name", "Members" },{ "value", members },{ "inline", true } },
                { { "name", "Channels" },{ "value", channels },{ "inline", true } },
                { { "name", "Uptime" },{ "value", bot.uptime_str() },{ "inline", true } },
                { { "name", "Guilds" },{ "value", guilds },{ "inline", true } },
                { { "name", "Events Seen" },{ "value", events },{ "inline", true } },
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
        { "title", "Bazcal Help" },
        { "description", error },
        { "color", 0xff6e5e },
    };
    return t;
}