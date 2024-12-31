#include "commands/ollama/describe_command.h"

dpp::slashcommand describe_command() {
    auto describe = dpp::slashcommand();
    describe.set_name("describe");
    describe.set_description("Use Jade to describe an image for you.");
    describe.add_option(dpp::command_option(dpp::co_attachment, "file", "Select an image", true));
    return describe;
}

void describe_process(const dpp::cluster &bot, const dpp::slashcommand_t& event, OllamaAPI &ollamaApi) {
    event.thinking();
    const auto file_id = std::get<dpp::snowflake>(event.get_parameter("file"));
    dpp::attachment att = event.command.get_resolved_attachment(file_id);
    std::string image_base64 = encode_to_base64(APIClient::download_image(att.url));
    const OllamaAPI::ChatMessage message{"user", "Describe this image", {image_base64}};
    std::string response = ollamaApi.sendMessage("Describe", message, false);

    auto jsonResponse = nlohmann::json::parse(response, nullptr, false);

    if (jsonResponse.is_discarded()) {
        bot.log(dpp::ll_error, "Failed to parse JSON response");
        event.edit_response(dpp::message(event.command.channel_id, "Error: Invalid response from server."));
        return;
    }

    bot.log(dpp::ll_debug, "Received a response from Ollama!");

    if (!jsonResponse.contains("message") || !jsonResponse["message"].contains("content")) {
        bot.log(dpp::ll_error, "Missing expected keys in JSON response");
        event.edit_response(dpp::message(event.command.channel_id, "Error: Malformed response content."));
        return;
    }

    const std::string description = jsonResponse["message"]["content"];
    const dpp::embed embed = getDescriptionEmbed(event, description, att.url);
    const dpp::message describe_message(event.command.channel_id, embed);
    event.edit_response(describe_message);
}