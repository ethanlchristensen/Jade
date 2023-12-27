#pragma once
#include <iostream>
#include <dpp/dpp.h>
#include "commands/commands.h"
//#include <curlpp/cURLpp.hpp>
//#include <curlpp/Options.hpp>


int main(int argc, char *argv[]) {

//    curlpp::Cleanup myCleanup;

    if (argc != 3) {
        std::cout << "ERROR: Usage: Bot Token and OpenAI API Token must be provided as command line arguments in "
                     "the order '. . . <bot-token> <openai-api-token>'\n";
        exit(1);
    }

    uint64_t intents = dpp::i_all_intents;

    dpp::cluster bot(argv[1], intents);

    bot.on_log(dpp::utility::cout_logger());

    bot.on_slashcommand([&bot](const dpp::slashcommand_t& event) {
        std::string command = event.command.get_command_name();

        if (command == "nolan") {
            bot.log(dpp::ll_debug, "/nolan called by " + event.command.usr.global_name + ".");
            nolan_process(bot, event);
        }
        else if (command == "clear") {
            bot.log(dpp::ll_debug, "/clear called by " + event.command.usr.global_name + ".");
            clear_process(bot, event);
        }
        else if (command == "echo") {
            bot.log(dpp::ll_debug, "/echo called by " + event.command.usr.global_name + ".");
            echo_process(bot, event);
        }
    });

    bot.on_ready([&bot](const dpp::ready_t& event) {
        if (dpp::run_once<struct register_bot_commands>()) {
            bot.log(dpp::ll_debug, "creating slash commands.");
            bot.global_command_create(nolan_command());
            bot.global_command_create(clear_command());
            bot.global_command_create(echo_command());
            bot.global_command_create(chat_command());
        }
    });

    bot.on_guild_member_update([&bot](const dpp::guild_member_update_t& event) {
        bot.log(dpp::ll_debug, "a user in the guild was updated.");
        if (event.updated.get_user()->global_name == "etchris") {
            if (event.updated.get_nickname() != "Ethan") {
                auto target_user = bot.guild_get_member_sync(event.updating_guild->id, event.updated.get_user()->id);
                target_user.set_nickname("Ethan");
                bot.guild_edit_member(target_user);
                bot.log(dpp::ll_debug, "Reverted " + event.updated.get_user()->global_name + " name.");
            }
        }
    });

    bot.start(dpp::st_wait);

    return 0;
}
