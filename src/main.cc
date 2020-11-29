#include "handlers.hpp"
#include "config.hpp"
#include "bazcal.h"
#include "embeds.hpp"
#include <aegis.hpp>
#include <nlohmann/json.hpp>
#include <thread>

int main(int argc, char * argv[])
{
    using namespace std::chrono_literals;
    try
    {
        // Create bot object
        aegis::core bot(aegis::create_bot_t().log_level(spdlog::level::trace).token(BAZCAL_BOT_TOKEN));

        // These callbacks are what the lib calls when messages come in
        bot.set_on_message_create(*message_create_handler);
        
        auction_load_cached(BAZCAL_PREDICTION_DUMP_FILE);

        // start the auction update loop
        std::thread auction_loop_thread(bz_auction_loop, BAZCAL_DB_FILENAME, 1, loop_callback);
        std::thread bazaar_loop_thread (bz_bazaar_loop, bazaar_loop_callback);

        // start the bot
        bot.run();
        // yield thread execution to the library
        bot.yield();
    }
    catch (std::exception & e)
    {
        std::cout << "Error during initialization: " << e.what() << '\n';
        return 1;
    }
    catch (...)
    {
        std::cout << "Error during initialization: uncaught\n";
        return 1;
    }
    return 0;
}
