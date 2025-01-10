#include "commands/commands.h"

dpp::slashcommand echo_command()
{
    auto echo = dpp::slashcommand()
        .set_name("echo")
        .set_description("Command to echo a message with text and/or image.")
        .add_option(dpp::command_option(dpp::co_string, "message", "The message to be sent.", false))
        .add_option(dpp::command_option(dpp::co_attachment, "image", "An image to include in the message.", false));
    return echo;
}

void echo_process(dpp::cluster &bot, const dpp::slashcommand_t &event)
{
    event.thinking(true);

    if (event.command.usr.global_name == "nulzo")
    {
        event.edit_response("Failed to send echo!");
        return;
    }

    const bool has_text = event.get_parameter("message").index() != 0;
    const bool has_image = !event.command.resolved.attachments.empty();

    if (!has_text && !has_image) {
        event.edit_response("You must provide either a text message, an image, or both!");
        return;
    }


    dpp::message msg(event.command.channel_id, "");

    if (has_text) {
        msg.set_content(std::get<std::string>(event.get_parameter("message")));
    }

    if (has_image) {
        const auto& attachment = event.command.resolved.attachments.begin()->second;
        const auto content = APIClient::download_image(attachment.url);
        msg.add_file(attachment.filename, content);
    }

    bot.message_create(msg, [&bot, event](const dpp::confirmation_callback_t& callback) {
        if (callback.is_error()) {
            bot.log(dpp::ll_error, "Failed to send echo: " + callback.get_error().message);
            event.edit_response("Failed to send echo: " + callback.get_error().message);
        } else {
            bot.log(dpp::ll_info, "Echo sent successfully!");
            event.edit_response("Echo sent successfully!");
        }
    });
}