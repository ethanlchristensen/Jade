#pragma once
#include <iostream>
#include <dpp/dpp.h>
#include "commands/commands.h"
//#include <curlpp/cURLpp.hpp>
//#include <curlpp/Options.hpp>


int main(int argc, char *argv[]) {

    if (argc != 3) {
        std::cout << "ERROR: Usage: Bot Token and OpenAI API Token must be provided as command line arguments in "
                     "the order '. . . <bot-token> <openai-api-token>'\n";
        exit(1);
    }

    std::string music_query;
    std::uint64_t channel_id;
    std::string filter;

    const std::string& apiToken = argv[2];

    uint64_t intents = dpp::i_all_intents;

    dpp::cluster bot(argv[1], intents);

    bot.on_log(dpp::utility::cout_logger());

    bot.on_slashcommand([&bot, &apiToken, &music_query, &channel_id, &filter](const dpp::slashcommand_t& event) {

        std::string user = event.command.usr.global_name;
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
        else if (command == "chat") {
            bot.log(dpp::ll_debug, "/chat called by " + event.command.usr.global_name + ".");
            chat_process(bot, event, apiToken);
        }
        else if (command == "summarize") {
            bot.log(dpp::ll_debug, "/summarize called by " + event.command.usr.global_name + ".");
            summarize_process(bot, event, apiToken);
        }
        else if (command == "extract") {
            bot.log(dpp::ll_debug, "/extract called by " + event.command.usr.global_name + ".");
            extract_process(bot, event, apiToken);
        }
        else if (command == "play") {
            bot.log(dpp::ll_debug, "/play called by " + event.command.usr.global_name + ".");
            // save the music query and channel_id to pass around as needed
            music_query = std::get<std::string>(event.get_parameter("query_or_link"));
            channel_id = event.command.channel_id;
            std::variant filter_provided = event.get_parameter("filter");
            if (filter_provided.index() > 0) {
                std::string filter_key = std::get<std::string>(filter_provided);
                filter = FILTERS.at(filter_key);
            } else {
                filter = "None";
            }
            play_process(bot, event, music_query, filter);
        }
        else if (command == "join") {
            bot.log(dpp::ll_debug, "/join called by " + event.command.usr.global_name + ".");
            join_process(bot, event);
        }
        else if (command == "leave") {
            bot.log(dpp::ll_debug, "/leave called by " + event.command.usr.global_name + ".");
            leave_process(bot, event);
        }
        else if (command == "pause") {
            bot.log(dpp::ll_debug, "/pause called by " + event.command.usr.global_name + ".");
            pause_process(bot, event);
        }
        else if (command == "resume") {
            bot.log(dpp::ll_debug, "/resume called by " + event.command.usr.global_name + ".");
            resume_process(bot, event);
        }
        else if (command == "skip") {
            bot.log(dpp::ll_debug, "/skip called by " + event.command.usr.global_name + ".");
            skip_process(bot, event);
        }
        else if (command == "say") {
            bot.log(dpp::ll_debug, "/say called by " + event.command.usr.global_name + ".");
            if (user == "etchris" || user == "nulzo") {
                say_process(bot, event, apiToken);
            }
        }
    });

    bot.on_message_create([](const dpp::message_create_t& event) {
        return;
    });

    bot.on_ready([&bot](const dpp::ready_t& event) {
        if (dpp::run_once<struct register_bot_commands>()) {
            bot.log(dpp::ll_debug, "creating slash commands.");
            bot.global_bulk_command_create({
                clear_command(),
                echo_command(),
                nolan_command(),
                chat_command(),
                summarize_command(),
                extract_command(),
                play_command(),
                join_command(),
                leave_command(),
                pause_command(),
                resume_command(),
                skip_command(),
                say_command(),
            });
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

    bot.on_voice_ready([&bot, &music_query, &channel_id, &filter](const dpp::voice_ready_t &event){
        stream_audio_secondary(bot, event, music_query, channel_id, filter);
    });

    bot.start(dpp::st_wait);

    return 0;
}
