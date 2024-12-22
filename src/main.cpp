#pragma once
#include <iostream>
#include <dpp/dpp.h>

#include "utils/env.h"
#include "utils/jade_slash.h"
#include "utils/ollama/ollama.h"

int main(int argc, char *argv[]) {

    std::string botToken;

    bool envLoaded = EnvLoader::loadEnvFile(".env");

    if (envLoaded) {
        botToken = EnvLoader::getEnvValue("DISCORD_BOT_TOKEN");
    } else {
        std::cerr << "WARNING: Failed to load .env file, attempting to grab bot token from command line args...\n";
        if (argc != 2) {
            std::cerr << "ERROR: Usage: Bot Token must be provided from a .env file or as command line argument: "
                         "'Jade.exe <bot-token>'\n";
            exit(1);
        }
        botToken = argv[1];
    }

    if (botToken.empty()) {
        std::cerr << "Bot token was not read in correctly. Is there a value in the .env file?\n";
        exit(1);
    }

    uint64_t intents = dpp::i_all_intents | dpp::i_message_content;
    dpp::cluster bot(botToken, intents);
    JadeQueue songQueue;
    OllamaAPI ollamaApi("http://localhost:11434");

    bot.on_log(dpp::utility::cout_logger());

    bot.on_slashcommand([&bot, &songQueue, &ollamaApi](const dpp::slashcommand_t &event) {
        processSlashCommand(bot, event, songQueue, ollamaApi);
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
                describe_command(),
                play_command(),
                play_test_command(),
                join_command(),
                leave_command(),
                pause_command(),
                resume_command(),
                skip_command(),
            });
        }
        bot.set_presence(dpp::presence(dpp::ps_online, dpp::at_custom, "Jade: The bot that turns 'Cringe' into history."));
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

    bot.on_voice_ready([&bot, &songQueue](const dpp::voice_ready_t &event){
        if (!songQueue.isEmpty()) {
            QueueEntry nextRequest = songQueue.nextRequest();
            stream_audio_to_discord(bot, nextRequest.request, nextRequest.info);
        }
    });

    bot.on_voice_track_marker([&bot, &songQueue](const dpp::voice_track_marker_t &event){
        std::cout << "Voice Track Marker Event\n";
        if (!songQueue.isEmpty()) {
            QueueEntry nextRequest = songQueue.nextRequest();
            stream_audio_to_discord(bot, nextRequest.request, nextRequest.info);
        }
    });

    bot.start(dpp::st_wait);

    return 0;
}
