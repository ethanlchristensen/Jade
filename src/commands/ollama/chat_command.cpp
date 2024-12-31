#include "commands/ollama/chat_command.h"

dpp::slashcommand chat_command() {
    auto chat = dpp::slashcommand();
    chat.set_name("chat");
    chat.set_description("Chat with Jade");
    chat.add_option(dpp::command_option(dpp::co_string, "message", "The message to send to Jade.", true));
    auto models = dpp::command_option(dpp::co_string, "model", "Model you want to chat with.", false);
    for (auto & [fst, snd] : MODELS) {
        models.add_choice(dpp::command_option_choice(fst, snd));
    }
    chat.add_option(models);
    return chat;
}

void chat_process(dpp::cluster &bot, const dpp::slashcommand_t& event, std::string& message, std::string& model, OllamaAPI &ollamaApi) {
    std::thread([&bot, event, message, model, &ollamaApi]() {
        try {
            event.thinking(false);
            bot.log(dpp::ll_debug, fmt::format("Chat command called with {}, {}", message, model));

            std::string response = ollamaApi.sendMessage(model, "user", message, false);

            auto jsonResponse = nlohmann::json::parse(response, nullptr, false);

            if (jsonResponse.is_discarded()) {
                bot.log(dpp::ll_error, "Failed to parse JSON response");
                event.edit_response(dpp::message(event.command.channel_id, "Error: Invalid response from server."));
                return;
            }

            bot.log(dpp::ll_debug, "JSON response from Ollama: " + jsonResponse.dump() + "\n");

            if (!jsonResponse.contains("message") || !jsonResponse["message"].contains("content")) {
                bot.log(dpp::ll_error, "Missing expected keys in JSON response");
                event.edit_response(dpp::message(event.command.channel_id, "Error: Malformed response content."));
                return;
            }

            std::string extracted_message = jsonResponse["message"]["content"];
            std::string users = EnvLoader::getEnvValue("USERS_TO_ID");
            auto jsonObject = nlohmann::json::parse(users);
            for (auto& [key, value] : jsonObject.items()) {
                if (value.is_string()) {
                    extracted_message = std::regex_replace(extracted_message, std::regex(key), value.get<std::string>());
                }
            }
            const dpp::embed embed = getChatEmbed(event, model, message, extracted_message);
            const dpp::message chat_message(event.command.channel_id, embed);
            event.edit_response(chat_message);
        } catch (const std::exception &e) {
            bot.log(dpp::ll_error, fmt::format("Exception in chat_process: {}", e.what()));
            event.edit_response(dpp::message(event.command.channel_id, "An error occurred during processing."));
        }
    }).detach();
}