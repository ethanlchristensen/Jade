#include "commands/voice/play_command.h"
#include "utils/voice/stream_audio.h"

dpp::slashcommand play_command()
{
    dpp::slashcommand play = dpp::slashcommand();
    play.set_name("play");
    play.set_description("Command to play audio in VC using a link or query,");
    play.add_option(dpp::command_option(dpp::co_string, "query_or_link", "Link to play or query to search.", true));
    dpp::command_option filters = dpp::command_option(dpp::co_string, "filter", "The filter to apply to the audio.",
                                                      false);
    for (auto &it : FILTERS)
    {
        filters.add_choice(dpp::command_option_choice(it.first, it.first));
    }
    play.add_option(filters);
    return play;
}

void play_process(dpp::cluster &bot, SongRequest song, JadeQueue &queue)
{
    song.event.thinking(true);

    dpp::guild *guild = dpp::find_guild(song.event.command.guild_id);

    if (!guild->connect_member_voice(song.event.command.get_issuing_user().id))
    {
        dpp::message error_msg(song.event.command.channel_id,
                               "I would love to play some music, but don't you want to listen too?", dpp::mt_default);
        song.event.edit_response(error_msg);
        return;
    }

    dpp::voiceconn *channel = song.event.from->get_voice(song.event.command.guild_id);

    if (channel && channel->voiceclient && channel->voiceclient->is_ready())
    {
        if (channel->voiceclient->is_playing())
        {
            std::cout << "Adding Song to the Queue\n";
            song.event.edit_response("Adding your request to the queue!");
            queue.addSong(song);
        }
        else
        {
            bot.log(dpp::ll_debug, "Jade in VC, streaming audio.");
            song.event.edit_response("Processing your request!");
            stream_audio_to_discord(bot, song);
        }
    }
    else
    {
        bot.log(dpp::ll_debug, "Jade not in VC, attempting to connect then stream.");
        song.event.edit_response("Processing your request!");
        queue.addSong(song);
        song.event.from->connect_voice(guild->id, song.event.command.channel_id, false, true);
    }
}
