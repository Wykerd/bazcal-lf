#ifndef BZ_EMBEDS_HPP
#define BZ_EMBEDS_HPP

#include <bazcal.h>
#include <aegis.hpp>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

const json make_info_obj(aegis::core & bot, aegis::shards::shard * _shard);
const json make_help_obj ();
const json make_error_obj (std::string error);
const json make_auction_flips_obj (bz_auction_pool_t *flips);
const json make_bazaar_obj (bz_bazaar_advice_buf_t *baz);

#endif