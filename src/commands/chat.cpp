#include "commands.h"
#include "../apis/apis.h"

dpp::slashcommand chat_command() {
    // create the command
    dpp::slashcommand chat = dpp::slashcommand();
    chat.set_name("chat");
    chat.set_description("Chat with me :green_heart:");
    return chat;
}

void chat_process(dpp::cluster &bot, const dpp::slashcommand_t &event) {
    // run the command
    dpp::message msg(event.command.channel_id, ":-).", dpp::mt_default);
    event.reply(msg);
}
