#ifndef BZ_HANDLERS_HPP
#define BZ_HANDLERS_HPP

#include <aegis.hpp>
#include "bazcal.h"

void loop_callback (sqlite3 *db);
void message_create_handler(aegis::gateway::events::message_create obj);

#endif