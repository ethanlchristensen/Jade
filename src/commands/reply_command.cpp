#include "commands/reply_command.h"

dpp::slashcommand reply_command()
{
    auto reply = dpp::slashcommand();
    reply.set_name("reply");
    reply.set_description("Command to reply to a message with a specified ID.");
    reply.add_option(dpp::command_option(dpp::co_string, "message_id", "The ID of the message to reply to.", true));
    reply.add_option(dpp::command_option(dpp::co_string, "reply", "The reply message to be sent.", true));
    return reply;
}

void reply_process(dpp::cluster &bot, const dpp::slashcommand_t &event)
{
    const auto message_id = std::get<std::string>(event.get_parameter("message_id"));
    const auto reply_text = std::get<std::string>(event.get_parameter("reply"));

    dpp::message msg(event.command.channel_id, reply_text);
    msg.set_reference(message_id);
    event.thinking(true);
    event.edit_response("reply sent");
    bot.message_create(msg);
}