#ifndef BZ_EMBEDS_HPP
#define BZ_EMBEDS_HPP

#include <aegis.hpp>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

const json make_info_obj(aegis::core & bot, aegis::shards::shard * _shard);
const json make_help_obj ();
const json make_error_obj (std::string error);

#endif