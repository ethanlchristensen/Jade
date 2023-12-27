#include "commands.h"
#include "../apis/apis.h"

dpp::slashcommand chat_command() {
    // create the command
    dpp::slashcommand chat = dpp::slashcommand();
    chat.set_name("chat");
    chat.set_description("Chat with me <3");
    chat.add_option(dpp::command_option(dpp::co_string, "message", "The message to send to Jade.", true));
    return chat;
}

void chat_process(dpp::cluster &bot, const dpp::slashcommand_t &event, const std::string& apiToken) {
    // defer and let the user know we are processing
    dpp::message processing_msg(event.command.channel_id, "Processing your request!", dpp::mt_default);
    event.thinking(true);
    event.edit_response(processing_msg);
    // run the command
    std::string pre_user_prompt = std::get<std::string>(event.get_parameter("message"));
    // need a better way to do this
    std::replace(pre_user_prompt.begin(), pre_user_prompt.end(), '/', ' ');
    std::replace(pre_user_prompt.begin(), pre_user_prompt.end(), '{', ' ');
    std::replace(pre_user_prompt.begin(), pre_user_prompt.end(), '}', ' ');
    std::replace(pre_user_prompt.begin(), pre_user_prompt.end(), '\\', ' ');
    // call gpt
    std::string response = llm(apiToken, SYSTEMPROMPT, pre_user_prompt);
    // check to make sure we are 2000 characters, else chunk
    if (response.size() > 1500) {
        size_t pos = 0;
        while (pos < response.size()) {
            size_t end_pos = pos + 1500;
            if (end_pos < response.size()) { // If not the last chunk
                size_t last_space = response.rfind(' ', end_pos);
                if (last_space != std::string::npos && last_space > pos) {
                    end_pos = last_space;
                }
            }
            dpp::message chunk(event.command.channel_id, response.substr(pos, end_pos - pos), dpp::mt_default);
            bot.message_create(chunk);

            pos = end_pos;
            if (pos < response.size() && response[pos] == ' ') {
                ++pos; // Skip the space at the beginning of the next chunk
            }
        }
    } else {
        dpp::message msg(event.command.channel_id, response, dpp::mt_default);
        event.reply(msg);
    }
}

dpp::slashcommand summarize_command() {
    // create the command
    dpp::slashcommand summarize = dpp::slashcommand();
    summarize.set_name("summarize");
    summarize.set_description("Let me summarize a piece of text for you!");
    summarize.add_option(dpp::command_option(dpp::co_string, "message", "The message to send to Jade for her to summarize.", true));
    return summarize;
}

void summarize_process(dpp::cluster &bot, const dpp::slashcommand_t &event, const std::string& apiToken) {
    // run the command
    std::string pre_user_prompt = std::get<std::string>(event.get_parameter("message"));
    // need a better way to do this
    std::replace(pre_user_prompt.begin(), pre_user_prompt.end(), '/', ' ');
    std::replace(pre_user_prompt.begin(), pre_user_prompt.end(), '{', ' ');
    std::replace(pre_user_prompt.begin(), pre_user_prompt.end(), '}', ' ');
    std::replace(pre_user_prompt.begin(), pre_user_prompt.end(), '\\', ' ');
    // call gpt
    std::string response = llm(apiToken, SYSTEMPROMPT, fmt::format("TASK: {} INPUT: {}", SUMMARIZEPROMPT, pre_user_prompt));
    dpp::message msg(event.command.channel_id, response, dpp::mt_default);
    event.reply(msg);
}

dpp::slashcommand extract_command() {
    // create the command
    dpp::slashcommand extract = dpp::slashcommand();
    extract.set_name("extract");
    extract.set_description("Let me help you extract some topics!");
    extract.add_option(dpp::command_option(dpp::co_string, "message", "The message to send to Jade to extract topics from.", true));
    return extract;
}

void extract_process(dpp::cluster &bot, const dpp::slashcommand_t &event, const std::string& apiToken) {
    // run the command
    std::string pre_user_prompt = std::get<std::string>(event.get_parameter("message"));
    // need a better way to do this
    std::replace(pre_user_prompt.begin(), pre_user_prompt.end(), '/', ' ');
    std::replace(pre_user_prompt.begin(), pre_user_prompt.end(), '{', ' ');
    std::replace(pre_user_prompt.begin(), pre_user_prompt.end(), '}', ' ');
    std::replace(pre_user_prompt.begin(), pre_user_prompt.end(), '\\', ' ');
    // call gpt
    std::string response = llm(apiToken, SYSTEMPROMPT, fmt::format("TASK: {} INPUT: {}", EXTRACTPROMPT, pre_user_prompt));
    dpp::message msg(event.command.channel_id, response, dpp::mt_default);
    event.reply(msg);
}
