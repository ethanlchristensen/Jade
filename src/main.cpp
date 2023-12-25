#pragma once
#include <iostream>
#include <dpp/dpp.h>
#include "utils/utils.h"
#include "commands/commands.h"


int main(int argc, char *argv[]) {

    if (argc != 2) {
        std::cout << "Bot token must be provided as command line arg!\n";
        exit(1);
    }

    dpp::cluster bot(argv[1]);

    bot.on_log(dpp::utility::cout_logger());

    bot.on_slashcommand([&bot](const dpp::slashcommand_t& event) {
        std::string command = event.command.get_command_name();

        if (command == "nolan") {
            bot.log(dpp::ll_debug, "Nolan command called by " + event.command.usr.global_name + ".");
            nolan_process(bot, event);
        }
        else if (command == "clear") {
            bot.log(dpp::ll_debug, "Clear command called by " + event.command.usr.global_name + ".");
            clear_process(bot, event);
        }
        else if (command == "echo") {
            bot.log(dpp::ll_debug, "Echo command called by " + event.command.usr.global_name + ".");
            echo_process(bot, event);
        }
    });

    bot.on_ready([&bot](const dpp::ready_t& event) {
        if (dpp::run_once<struct register_bot_commands>()) {
            bot.log(dpp::ll_debug, "Creating commands.");
            bot.global_command_create(nolan_command());
            bot.global_command_create(clear_command());
            bot.global_command_create(echo_command());
        }
    });
    bot.log(dpp::ll_debug, "Starting up bot.");
    bot.start(dpp::st_wait);
    return 0;
}
