#include "commands.h"

dpp::slashcommand nolan_command() {
    // create the command
    dpp::slashcommand nolan = dpp::slashcommand();
    nolan.set_name("nolan");
    nolan.set_description("nolan");
    return nolan;
}

void nolan_process(dpp::cluster &bot, const dpp::slashcommand_t &event) {
    // run the command
    dpp::message msg(event.command.channel_id, "<@933796468731568191> is a small-time wannabe trying to talk big and act tough, but everyone sees through his act. He's all talk, no action, and nobody takes him seriously.", dpp::mt_default);
    event.reply(msg);
}
