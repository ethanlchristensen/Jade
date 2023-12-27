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
    // run the command
    if (event.command.usr.global_name != "etchris") {
        dpp::message msg(event.command.channel_id, "While I would love to talk to you, I am only speaking with etchris at the moment.");
        event.reply(msg);
    } else {
        std::string pre_user_prompt = std::get<std::string>(event.get_parameter("message"));

        // need a better way to do this
        std::replace(pre_user_prompt.begin(), pre_user_prompt.end(), '/', ' ');
        std::replace(pre_user_prompt.begin(), pre_user_prompt.end(), '{', ' ');
        std::replace(pre_user_prompt.begin(), pre_user_prompt.end(), '}', ' ');
        std::replace(pre_user_prompt.begin(), pre_user_prompt.end(), '\\', ' ');

        std::string response = chat(apiToken, pre_user_prompt);
        dpp::message msg(event.command.channel_id, response, dpp::mt_default);
        event.reply(msg);
    }
}
