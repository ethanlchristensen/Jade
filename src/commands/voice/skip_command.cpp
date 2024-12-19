#include "commands/voice/skip_command.h"

dpp::slashcommand skip_command()
{
    dpp::slashcommand skip = dpp::slashcommand();
    skip.set_name("skip");
    skip.set_description("Command to have Jade skip the currently playing song.");
    return skip;
}

void skip_process(dpp::cluster &bot, const dpp::slashcommand_t &event, JadeQueue& queue)
{
    dpp::voiceconn *v = event.from->get_voice(event.command.guild_id);

    if (v && v->voiceclient)
    {
        if (v->voiceclient->is_playing())
        {
            v->voiceclient->stop_audio();
            dpp::message success_msg(event.command.channel_id, "Skipped the song!", dpp::mt_default);
            event.reply(success_msg);
        }
        else
        {
            dpp::message error_msg(event.command.channel_id, "There is no song to skip!", dpp::mt_default);
            event.reply(error_msg);
        }
        if (!queue.isEmpty()) {
            stream_audio_to_discord(bot, queue.nextRequest());
        }
    }
    else
    {
        dpp::message not_in_voice_msg(event.command.channel_id, "I cannot skip a song if I am not in VC!");
        event.reply(not_in_voice_msg);
    }
}
