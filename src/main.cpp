#pragma once
#include <iostream>
#include <dpp/dpp.h>

#include "utils/env.h"
#include "utils/jade_slash.h"
#include "utils/ollama/ollama.h"

int main(const int argc, char *argv[]) {

    std::string botToken;

    if (EnvLoader::loadEnvFile(".env")) {
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
        std::cerr << "Bot token was not read in correctly. Is there a value in the .env file for DISCORD_BOT_TOKEN?\n";
        exit(1);
    }

    constexpr uint64_t intents = dpp::i_all_intents | dpp::i_message_content;
    dpp::cluster bot(botToken, intents);
    JadeQueue songQueue;
    OllamaAPI ollamaApi(EnvLoader::getEnvValue("OLLAMA_ENDPOINT"));

    bot.on_log(dpp::utility::cout_logger());

    bot.on_slashcommand([&bot, &songQueue, &ollamaApi](const dpp::slashcommand_t &event) {
        processSlashCommand(bot, event, songQueue, ollamaApi);
    });

    bot.on_message_create([](const dpp::message_create_t& event) {
        if (event.msg.author.is_bot()) return;
        return;
    });

    bot.on_ready([&bot](const dpp::ready_t& event) {
        loadSlashCommands(bot);
        const dpp::presence presence(dpp::ps_online, dpp::at_custom, "Exploring infinite possibilities.");
        bot.set_presence(presence);
    });

    bot.on_guild_member_update([&bot](const dpp::guild_member_update_t& event) {
        bot.log(dpp::ll_info, "a user in the guild was updated.");
        if (event.updated.get_user()->global_name == "etchris") {
            if (event.updated.get_nickname() != "etchris") {
                auto target_user = bot.guild_get_member_sync(event.updating_guild->id, event.updated.get_user()->id);
                target_user.set_nickname("etchris");
                bot.guild_edit_member(target_user);
                bot.log(dpp::ll_info, "Reverted " + event.updated.get_user()->global_name + " name.");
            }
        }
    });

    bot.on_voice_ready([&bot, &songQueue](const dpp::voice_ready_t &event){
        if (!songQueue.isEmpty()) {
            auto [request, info] = songQueue.nextRequest();
            stream_audio_to_discord(bot, request, info);
        }
    });

    bot.on_voice_track_marker([&bot, &songQueue](const dpp::voice_track_marker_t &event){
        std::cout << "Voice Track Marker Event\n";
        if (!songQueue.isEmpty()) {
            auto [request, info] = songQueue.nextRequest();
            stream_audio_to_discord(bot, request, info);
        }
    });

    bot.start(dpp::st_wait);

    return 0;
}
