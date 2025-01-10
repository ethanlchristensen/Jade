#include "commands/voice/join_command.h"

dpp::slashcommand join_command()
{
    dpp::slashcommand join = dpp::slashcommand();
    join.set_name("join");
    join.set_description("Command to have Jade join the VC you are in.");
    return join;
}

void join_process(dpp::cluster &bot, const dpp::slashcommand_t &event)
{
    event.thinking(true);
    if (event.command.usr.global_name == "nulzo") {
        event.edit_response("Processing your request!");
        return;
    }
    event.edit_response("Processing your request!");

    dpp::guild *g = dpp::find_guild(event.command.guild_id);

    if (!g->connect_member_voice(event.command.get_issuing_user().id))
    {
        dpp::message error_msg(event.command.channel_id,
                               "I would love to play some music, but don't you want to listen too?", dpp::mt_default);
        bot.message_create(error_msg);
        return;
    }
}