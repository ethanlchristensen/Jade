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

bool checkAndModerateMediaContent(dpp::cluster& bot, const dpp::message_create_t& event, GeminiAPI& geminiApi) {
    // Function to check content and take action if inappropriate
    auto check_content = [&bot, &event, &geminiApi](const std::string& image_data) -> bool {
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
            return true;
        }

        bot.log(dpp::ll_info, "Content in the message was appropriate.");
        return false;
    };

    // Helper function to check if a file extension is image or video
    auto is_media_extension = [](const std::string& extension) -> bool {
        return extension == "jpg" || extension == "jpeg" || extension == "png" ||
               extension == "gif" || extension == "mp4" || extension == "mov";
    };

    // Helper function to extract extension from URL
    auto get_extension_from_url = [](const std::string& url) -> std::string {
        size_t query_pos = url.find('?');
        std::string path = query_pos != std::string::npos ? url.substr(0, query_pos) : url;
        size_t dot_pos = path.find_last_of('.');
        if (dot_pos != std::string::npos) {
            std::string ext = path.substr(dot_pos + 1);
            std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
            return ext;
        }
        return "";
    };

    // Check message content for Discord CDN links
    std::string content = event.msg.content;
    std::regex discord_cdn_regex(R"((https?://(?:cdn\.discordapp\.com|media\.discordapp\.net)/[^\s]+))");

    std::smatch matches;
    std::string::const_iterator search_start(content.cbegin());

    bool inappropriate_found = false;

    while (std::regex_search(search_start, content.cend(), matches, discord_cdn_regex) && !inappropriate_found) {
        std::string url = matches[1].str();
        bot.log(dpp::ll_debug, "Found Discord CDN URL: " + url);

        std::string extension = get_extension_from_url(url);

        if (is_media_extension(extension)) {
            std::string image_data;
            if (extension == "mp4" || extension == "mov") {
                bot.log(dpp::ll_info, "Video detected in URL, extracting first frame for moderation");
                image_data = extractFirstFrameFromVideo(url);
            } else {
                image_data = APIClient::download_image(url);
            }

            inappropriate_found = check_content(image_data);
            if (inappropriate_found) break;
        }

        search_start = matches[0].second;
    }

    // If we already found inappropriate content in URLs, skip checking attachments
    if (inappropriate_found) return true;

    // Check if the message contains attachments
    if (!event.msg.attachments.empty()) {
        bot.log(dpp::ll_debug, "Target channel found and attachments in message found!");
        for (const auto& attachment : event.msg.attachments) {
            // Check if it's an image or video
            std::string filename = attachment.filename;
            std::string extension = filename.substr(filename.find_last_of('.') + 1);
            std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

            if (is_media_extension(extension)) {
                std::string image_data;
                if (extension == "mp4" || extension == "mov") {
                    bot.log(dpp::ll_info, "Video detected, extracting first frame for moderation");
                    image_data = extractFirstFrameFromVideo(attachment.url);
                } else {
                    image_data = APIClient::download_image(attachment.url);
                }

                if (check_content(image_data)) return true;
            }
        }
    }

    return false;
}

std::map<dpp::snowflake, dpp::discord_client *> discord_clients;
std::mutex discord_clients_m;

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
    nlohmann::json channel_ids_json = nlohmann::json::parse(EnvLoader::getEnvValue("IMAGE_FILTER_CHANNELS"));
    nlohmann::json users_to_check = nlohmann::json::parse(EnvLoader::getEnvValue("IMAGE_FILTER_USERS"));
    std::string clownUserId = EnvLoader::getEnvValue("CLOWN_USER");

    bot.on_log(dpp::utility::cout_logger());

    bot.log(dpp::ll_info, fmt::format("Loading Jade Discord Bot in environment {}", environment));

    bot.on_ready([&bot](const dpp::ready_t& event) {
        loadSlashCommands(bot);
        const dpp::presence presence(dpp::ps_online, dpp::at_custom, "Exploring infinite possibilities.");
        bot.set_presence(presence);
    });

    bot.on_slashcommand([&bot, &songQueue, &ollamaApi](const dpp::slashcommand_t &event) {
        processSlashCommand(bot, event, songQueue, ollamaApi);
    });

    bot.on_message_create([&bot, &geminiApi, channel_ids_json, users_to_check, &clownUserId](const dpp::message_create_t &event) {
        if (event.msg.author.is_bot()) return;

        if (event.msg.author.id.str() == clownUserId) {
            bot.message_add_reaction(event.msg.id, event.msg.channel_id, "🤡");
        }

        bool channelCheck = false;
        for (const auto& channel_id : channel_ids_json) {
            if (channel_id == event.msg.channel_id.str()) {
                channelCheck = true;
                break;
            }
        }

        bool userCheck = false;
        for (const auto& user_id : users_to_check) {
            if (user_id == event.msg.author.id.str()) {
                userCheck = true;
                break;
            }
        }

        if (channelCheck || userCheck) {
            checkAndModerateMediaContent(bot, event, geminiApi);
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
        bot.log(dpp::ll_info, "Voice On Ready Event");
        if (!songQueue.isEmpty()) {
            auto [request, info] = songQueue.nextRequest();
            stream_audio_to_discord(bot, request, info);
        }
    });

    bot.on_voice_track_marker([&bot, &songQueue](const dpp::voice_track_marker_t &event) {
        bot.log(dpp::ll_info, "Voice Track Marker Event");
        if (!songQueue.isEmpty()) {
            auto [request, info] = songQueue.nextRequest();
            stream_audio_to_discord(bot, request, info);
        }
    });

    bot.start(dpp::st_wait);

    return 0;
}
