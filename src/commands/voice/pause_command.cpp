#include "commands/voice/pause_command.h"

dpp::slashcommand pause_command()
{
    dpp::slashcommand pause = dpp::slashcommand();
    pause.set_name("pause");
    pause.set_description("Command to have Jade pause the currently playing audio.");
    return pause;
}

void pause_process(dpp::cluster &bot, const dpp::slashcommand_t &event)
{
    dpp::voiceconn *v = event.from->get_voice(event.command.guild_id);

    if (!v || !v->voiceclient)
    {
        event.reply("I am not in a VC, nothing to pause!");
        return;
    }

    if (v->voiceclient->is_playing())
    {
        v->voiceclient->pause_audio(true);
        event.reply("Paused the audio!");
    }
    else
    {
        event.reply("Nothing to pause!");
    }
}
