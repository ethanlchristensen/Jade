#include "commands/voice/leave_command.h"

dpp::slashcommand leave_command()
{
    // create the command
    dpp::slashcommand leave = dpp::slashcommand();
    leave.set_name("leave");
    leave.set_description("Command to have Jade leave the VC she is currently in.");
    return leave;
}

void leave_process(dpp::cluster &bot, const dpp::slashcommand_t &event)
{
    dpp::voiceconn *v = event.from->get_voice(event.command.guild_id);

    if (v)
    {
        if (v->voiceclient->is_playing())
        {
            bot.log(dpp::ll_info, "bot is playing audio, stopping audio.");
            v->voiceclient->stop_audio();
        }
        bot.log(dpp::ll_info, "leaving voice channel.");
        event.from->disconnect_voice(event.command.guild_id);
        event.reply("Peace out ✌");
    }
    else
    {
        event.reply("I'm not in a VC right now silly!");
    }
}