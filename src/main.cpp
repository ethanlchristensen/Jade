#include <iostream>
#include <dpp/dpp.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cout << "Bot token must be provided as command line arg!\n";
        exit(1);
    }

    dpp::cluster bot(argv[1]);

    bot.on_log(dpp::utility::cout_logger());
    bot.on_slashcommand([&bot](const dpp::slashcommand_t& event) {
        std::string called_command = event.command.get_command_name();
        if (called_command == "blep") {
            std::string animal = std::get<std::string>(event.get_parameter("animal"));
            event.reply(std::string("Blep! You chose ") + animal);
        }
        else if (called_command == "nolan") {
            std::cout << "Nolan command was called!\n";
            std::string balls = std::get<std::string>(event.get_parameter("nolan"));
            event.reply(balls);
        }
        else if (called_command == "clear") {
            if (dpp::run_once<struct clear_bot_commands>()) {
                bot.global_bulk_command_delete();
            }
        }
    });
    bot.on_ready([&bot](const dpp::ready_t& event) {
        if (dpp::run_once<struct register_bot_commands>()) {
            dpp::slashcommand new_command("blep", "Send a random adorable animal photo", bot.me.id);
            new_command.add_option(
                    dpp::command_option(dpp::co_string, "animal", "The type of animal", true)
                            .add_choice(dpp::command_option_choice("Dog", std::string("animal_dog")))
                            .add_choice(dpp::command_option_choice("Cat", std::string("animal_cat")))
                            .add_choice(dpp::command_option_choice("Penguin", std::string("animal_penguin")))
            );
            bot.global_command_create(new_command);
            dpp::slashcommand nolan_command("nolan", "nolan", bot.me.id);
            nolan_command.add_option(
                    dpp::command_option(dpp::co_string, "nolan", "What nolan does", true)
                            .add_choice(dpp::command_option_choice("a", std::string("nolan a")))
                            .add_choice(dpp::command_option_choice("b", std::string("nolan a")))
                            .add_choice(dpp::command_option_choice("c", std::string("nolan a")))
            );
            bot.global_command_create(nolan_command);
            dpp::slashcommand clear_command("clear", "Command to clear the registered commands", bot.me.id);
            bot.global_command_create(clear_command);
        }
    });
    bot.start(dpp::st_wait);
    return 0;
}
