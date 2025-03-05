#include <iostream>
#include <random>
#include <dpp/dpp.h>

#include "utils/env.h"
#include "utils/jade_util.h"
#include "utils/jade_slash.h"
#include "utils/ollama/ollama.h"
#include "utils/gemini/gemini.h"


void validateEnvironmentVariables() {
    std::vector<std::string> required_env_vars = {
            "DISCORD_BOT_TOKEN",
            "USERS_TO_ID",
            "OLLAMA_ENDPOINT",
            "ENV",
            "GUILD_ID",
            "REMOVE_REACTION_MAPPINGS",
            "GEMINI_API_KEY",
            "GEMINI_API_URL"
    };

    for (const auto &var: required_env_vars) {
        if (EnvLoader::getEnvValue(var).empty()) {
            std::cerr << "ERROR: Required environment variable " << var << " is missing or empty.\n";
            std::terminate();
        }
    }
}


int main(const int argc, char *argv[]) {
    std::random_device rd;
    std::mt19937 gen(rd());

    bool envLoaded = EnvLoader::loadEnvFile(".env");

    if (envLoaded) {
        validateEnvironmentVariables();
    }

    auto botToken = EnvLoader::getEnvValue("DISCORD_BOT_TOKEN");

    if (botToken.empty()) {
        std::cerr << "Bot token was not read in correctly. Is there a value in the .env file for DISCORD_BOT_TOKEN?\n";
        std::terminate();
    }

    dpp::cluster bot(botToken, dpp::i_all_intents | dpp::i_message_content);

    JadeQueue songQueue;
    OllamaAPI ollamaApi(EnvLoader::getEnvValue("OLLAMA_ENDPOINT"));
    GeminiAPI geminiApi(
            EnvLoader::getEnvValue("GEMINI_API_URL"),
            EnvLoader::getEnvValue("GEMINI_API_KEY")
    );
    auto environment = EnvLoader::getEnvValue("ENV");
    auto removeReactionMappings = nlohmann::json::parse(EnvLoader::getEnvValue("REMOVE_REACTION_MAPPINGS"));

    bot.on_log(dpp::utility::cout_logger());

    bot.log(dpp::ll_info, fmt::format("Loading Jade Discord Bot in environment {}", environment));

    bot.on_slashcommand([&bot, &songQueue, &ollamaApi](const dpp::slashcommand_t &event) {
        processSlashCommand(bot, event, songQueue, ollamaApi);
    });

    bot.on_message_create([&bot, &geminiApi](const dpp::message_create_t &event) {
        if (event.msg.author.is_bot()) return;

        nlohmann::json channel_ids_json = nlohmann::json::parse(EnvLoader::getEnvValue("NORMAL_CHAT_CHANNEL_ID"));
        bool check = false;
        for (const auto& channel_id : channel_ids_json) {
            if (channel_id == event.msg.channel_id.str()) {
                check = true;
                break;
            }
        }

        // Check if the message contains attachments
        if (check && !event.msg.attachments.empty()) {
            bot.log(dpp::ll_debug, "Target channel found and attachments in message found!");

            for (const auto& attachment : event.msg.attachments) {
                // Check if it's an image or video
                std::string filename = attachment.filename;
                std::string extension = filename.substr(filename.find_last_of('.') + 1);
                std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

                if (extension == "jpg" || extension == "jpeg" || extension == "png" || extension == "gif" ||
                    extension == "mp4" || extension == "mov") {

                    std::string image_data;

                    if (extension == "mp4" || extension == "mov") {
                        bot.log(dpp::ll_info, "Video detected, extracting first frame for moderation");
                        image_data = extractFirstFrameFromVideo(attachment.url);
                    } else {
                        image_data = APIClient::download_image(attachment.url);
                    }

                    std::string result = geminiApi.checkImage("gemini-2.0-flash-lite", image_data);
                    std::string lower_result = result;
                    std::transform(lower_result.begin(), lower_result.end(), lower_result.begin(), ::tolower);

                    bot.log(dpp::ll_info, "Image/video check result: " + result);

                    if (lower_result.find("inappropriate") != std::string::npos) {
                        bot.log(dpp::ll_info, "Inappropriate content detected in normal chat, deleting the message.");
                        bot.message_delete(event.msg.id, event.msg.channel_id, [&bot, event](const dpp::confirmation_callback_t& callback) {
                            if (callback.is_error()) {
                                bot.log(dpp::ll_error, "Failed to delete inappropriate content: " + callback.get_error().message);
                            } else {
                                bot.log(dpp::ll_info, "Deleted inappropriate content from user " + std::to_string(event.msg.author.id) + " in channel " + std::to_string(event.msg.channel_id));
                                dpp::guild* guild = dpp::find_guild(event.msg.guild_id);
                                std::string guild_name = guild ? guild->name : "unknown server";
                                dpp::message warning(event.msg.author.id, fmt::format("Your message in the server {} was removed because it contained inappropriate content. Please use a channel other than normal chat.", guild_name));
                                bot.direct_message_create(event.msg.author.id, warning);
                            }
                        });
                        break;
                    } else {
                        bot.log(dpp::ll_info, "Content in the message was appropriate.");
                    }
                }
            }
        }
    });

    bot.on_ready([&bot, &environment, &gen](const dpp::ready_t &event) {
        std::string bot_username;
        std::string presence_message;

        if (environment == "dev") {
            std::uniform_int_distribution<int> devDis(0, static_cast<int>(devMessages.size() - 1));
            presence_message = devMessages[devDis(gen)];
            bot_username = "Jade (dev)";
        } else if (environment == "prod") {
            std::uniform_int_distribution<int> prodDis(0, static_cast<int>(prodMessages.size() - 1));
            presence_message = prodMessages[prodDis(gen)];
            bot_username = "Jade";
        }

        loadSlashCommands(bot);
        const dpp::presence presence(dpp::ps_online, dpp::at_custom, presence_message);
        bot.set_presence(presence);

        if (dpp::run_once<struct set_bot_nickname>()) {
            bot.guild_get_member(EnvLoader::getEnvValue("GUILD_ID"), bot.me.id,
                                 [&bot, bot_username](const dpp::confirmation_callback_t &callback) {
                                     const auto *bot_member = std::get_if<dpp::guild_member>(&callback.value);
                                     if (bot_member->get_nickname() != bot_username) {
                                         bot.log(dpp::ll_info,
                                                 fmt::format(
                                                         "Bot guild nickname is not correctly set, setting it to {}.",
                                                         bot_username));
                                         bot.guild_current_member_edit(EnvLoader::getEnvValue("GUILD_ID"),
                                                                       bot_username,
                                                                       [&bot](const dpp::confirmation_callback_t &callback) {
                                                                           if (callback.is_error()) {
                                                                               bot.log(dpp::ll_error,
                                                                                       fmt::format(
                                                                                               "Failed to set the guild nickname: {}",
                                                                                               callback.get_error().message));
                                                                           } else {
                                                                               bot.log(dpp::ll_info,
                                                                                       "Successfully set the guild nickname!");
                                                                           }
                                                                       });
                                     } else {
                                         bot.log(dpp::ll_info, "Bot guild nickname is already correctly set.");
                                     }
                                 });
        }
    });

    bot.on_guild_member_update([&bot](const dpp::guild_member_update_t &event) {
        if (event.updated.get_user()->global_name == "etchris" && event.updated.get_nickname() != "etchris") {
            bot.guild_get_member(
                    event.updating_guild->id,
                    event.updated.get_user()->id,
                    [&bot](const dpp::confirmation_callback_t &callback) {
                        if (callback.is_error()) {
                            bot.log(dpp::ll_error, fmt::format("Failed to get the guild user: {}",
                                                               callback.get_error().message));
                        } else {
                            const auto *target_user = std::get_if<dpp::guild_member>(&callback.value);
                            if (target_user) {
                                dpp::guild_member edited_user = *target_user;
                                edited_user.set_nickname("etchris");
                                bot.guild_edit_member(edited_user);
                                bot.log(dpp::ll_info,
                                        "Successfully updated the guild user's nickname!");
                            } else {
                                bot.log(dpp::ll_error, "Received unexpected type in callback.value");
                            }
                        }
                    }
            );
        }
    });

    bot.on_message_reaction_add([&bot, &removeReactionMappings](const dpp::message_reaction_add_t &event) {
        bot.message_get(event.message_id, event.channel_id,
                        [&bot, event, removeReactionMappings](const dpp::confirmation_callback_t &callback) {
                            if (callback.is_error()) return;

                            const auto *msg = std::get_if<dpp::message>(&callback.value);
                            if (!msg) return;

                            for (auto &mapping: removeReactionMappings.items()) {
                                const std::string &messageAuthor = mapping.key();
                                const std::vector<std::string>& reactingUsers = mapping.value().get<std::vector<std::string>>();
                                const std::string reactorName = event.reacting_user.global_name;
                                const std::string msgUserName = msg->author.global_name;
                                if (msg->author.id.str()== messageAuthor) {
                                    for (const std::string &reactingUser: reactingUsers) {
                                        if (event.reacting_member.get_user()->id.str() == reactingUser) {
                                            auto reaction = event.reacting_emoji.id != 0 ? fmt::format("{}:{}",
                                                                                                       event.reacting_emoji.name,
                                                                                                       event.reacting_emoji.id.str())
                                                                                         : event.reacting_emoji.name;
                                            bot.message_delete_reaction(event.message_id,
                                                                        event.channel_id,
                                                                        event.reacting_user.id,
                                                                        reaction,
                                                                        [&bot, reactorName, msgUserName](
                                                                                const dpp::confirmation_callback_t &completionCallback) {
                                                                            if (completionCallback.is_error()) {
                                                                                bot.log(dpp::ll_error,
                                                                                        "Error removing reaction from message.");
                                                                            } else {
                                                                                bot.log(dpp::ll_info, fmt::format(
                                                                                        "Removed reaction by {} from {} message.",
                                                                                        reactorName, msgUserName));
                                                                            }
                                                                        });
                                        }
                                    }
                                }
                            }
                        });
    });

    bot.on_voice_ready([&bot, &songQueue](const dpp::voice_ready_t &event) {
        if (!songQueue.isEmpty()) {
            auto [request, info] = songQueue.nextRequest();
            stream_audio_to_discord(bot, request, info);
        }
    });

    bot.on_voice_track_marker([&bot, &songQueue](const dpp::voice_track_marker_t &event) {
        std::cout << "Voice Track Marker Event\n";
        if (!songQueue.isEmpty()) {
            auto [request, info] = songQueue.nextRequest();
            stream_audio_to_discord(bot, request, info);
        }
    });

    bot.start(dpp::st_wait);

    return 0;
}
