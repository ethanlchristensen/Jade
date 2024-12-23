#include "commands/commands.h"

dpp::slashcommand echo_command()
{
    // create the slash command
    dpp::slashcommand echo = dpp::slashcommand();
    echo.set_name("echo");
    echo.set_description("Command to have Jade echo back what you type.");
    echo.add_option(dpp::command_option(dpp::co_string, "message", "The message to be echoed.", true));
    return echo;
}

void echo_process(dpp::cluster &bot, const dpp::slashcommand_t &event)
{
    bot.log(dpp::ll_info, std::get<std::string>(event.get_parameter("message")));
    dpp::message msg(event.command.channel_id, std::get<std::string>(event.get_parameter("message")));
    event.thinking(true);
    event.edit_response("echo sent");
    bot.message_create(msg);
}