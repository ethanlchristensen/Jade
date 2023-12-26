#pragma once
#include <iostream>
#include <dpp/dpp.h>
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

    bot.on_guild_members_chunk([](const dpp::guild_members_chunk_t& event) {
        std::cout << "guild members chunk\n";
    });

    bot.on_user_update([](const dpp::user_update_t& event) {
        std::cout << "user update\n";
    });

    bot.on_guild_member_update([](const dpp::guild_member_update_t& event){
        std::cout << "guild member update\n";
    });

//    bot.on_guild_member_update([&bot](const dpp::guild_member_update_t& event) {
//        bot.log(dpp::ll_debug, "A user was updated in the guild!");
//        if (event.updated.get_user()->global_name == "etchris") {
//            if (event.updated.get_nickname() != "Ethan") {
//                auto target_user = bot.guild_get_member_sync(event.updating_guild->id, event.updated.get_user()->id);
//                target_user.set_nickname("Ethan");
//                bot.guild_edit_member(target_user);
//
//                dpp::message msg(1084339043938095104, "Let's not change Ethan's username :-)");
//                bot.message_create(msg);
//            }
//        }
//    });

    bot.start(dpp::st_wait);
    return 0;
}
