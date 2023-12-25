#include "commands.h"

dpp::slashcommand clear_command() {
    // create the slash command
    dpp::slashcommand clear = dpp::slashcommand();
    clear.set_name("clear");
    clear.set_description("Command to clear commands");
    return clear;
}

void clear_process(dpp::cluster &bot, const dpp::slashcommand_t &event){
    // run the command
    if (dpp::run_once<struct clear_bot_commands>()) {
        if (event.command.usr.global_name == "etchris") // epic guy right there
            bot.global_bulk_command_delete();
        else {
            dpp::message msg(event.command.channel_id, "Nice try pal, not gonna let you run that command!", dpp::mt_default);
            event.reply(msg);
        }
    }
}
