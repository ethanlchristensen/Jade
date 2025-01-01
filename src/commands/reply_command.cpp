#include "commands/reply_command.h"

dpp::slashcommand reply_command()
{
    auto reply = dpp::slashcommand();
    reply.set_name("reply");
    reply.set_description("Command to reply to a message with text and/or image.");
    reply.add_option(dpp::command_option(dpp::co_string, "message_id", "The ID of the message to reply to.", true));
    reply.add_option(dpp::command_option(dpp::co_string, "reply", "The reply message to be sent.", false));
    reply.add_option(dpp::command_option(dpp::co_attachment, "image", "An image to include in the reply.", false));
    return reply;
}

void reply_process(dpp::cluster &bot, const dpp::slashcommand_t &event)
{
    event.thinking(true);

    if (event.command.usr.global_name == "nulzo")
    {
        event.edit_response("Failed to send reply!");
        return;
    }

    const bool has_text = event.get_parameter("reply").index() != 0;
    const bool has_image = event.command.resolved.attachments.size() > 0;

    if (!has_text && !has_image) {
        event.edit_response("You must provide either a text message, an image, or both!");
        return;
    }

    const auto message_id = std::get<std::string>(event.get_parameter("message_id"));

    dpp::message msg(event.command.channel_id, "");
    msg.set_reference(message_id);

    if (has_text) {
        msg.set_content(std::get<std::string>(event.get_parameter("reply")));
    }

    if (has_image) {
        const auto& attachment = event.command.resolved.attachments.begin()->second;
        const auto content = APIClient::download_image(attachment.url);
        msg.add_file(attachment.filename, content);
    }

    bot.message_create(msg, [&bot, event](const dpp::confirmation_callback_t& callback) {
        if (callback.is_error()) {
            bot.log(dpp::ll_error, "Failed to send reply: " + callback.get_error().message);
            event.edit_response("Failed to send reply: " + callback.get_error().message);
        } else {
            bot.log(dpp::ll_debug, "Reply sent successfully!");
            event.edit_response("Reply sent successfully!");
        }
    });
}