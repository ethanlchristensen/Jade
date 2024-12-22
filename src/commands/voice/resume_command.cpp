#include "commands/voice/resume_command.h"

dpp::slashcommand resume_command()
{
    dpp::slashcommand resume = dpp::slashcommand();
    resume.set_name("resume");
    resume.set_description("Command to have Jade resume the paused audio.");
    return resume;
}

void resume_process(dpp::cluster &bot, const dpp::slashcommand_t &event)
{
    dpp::voiceconn *v = event.from->get_voice(event.command.guild_id);

    if (!v || !v->voiceclient)
    {
        event.reply("I am not in a VC, nothing to resume!");
        return;
    }

    if (v->voiceclient->is_paused())
    {
        v->voiceclient->pause_audio(false);
        event.reply("Resumed the audio!");
    }
    else
    {
        event.reply("Nothing to resume!");
    }
}
