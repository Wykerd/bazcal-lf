#include "handlers.hpp"
#include "config.hpp"
#include <aegis.hpp>
#include <nlohmann/json.hpp>

int main(int argc, char * argv[])
{
    using namespace std::chrono_literals;
    try
    {
        // Create bot object
        aegis::core bot(aegis::create_bot_t().log_level(spdlog::level::trace).token(BAZCAL_BOT_TOKEN));

        // These callbacks are what the lib calls when messages come in
        bot.set_on_message_create(*message_create_handler);

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
