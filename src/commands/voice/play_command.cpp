#include "commands/voice/play_command.h"

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

void play_process(dpp::cluster &bot, const dpp::slashcommand_t& event, std::string& query,  std::string& filter, JadeQueue &queue)
{
    event.thinking();

    if (!isValidURL(query)) {
        bot.log(dpp::ll_debug, fmt::format("Query was not a URL, searching youtube for a url: {}", query));
        query = getYoutubeUrl(query);
    }

    SongInfo songInfo = getSongInfo(query);
    SongRequest song{query, filter, event};

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
            bot.log(dpp::ll_debug, "Bot is playing music, adding song to queue.");
            dpp::embed embed = getAddedToQueueEmbed(song, songInfo);
            dpp::message message(event.command.channel_id, embed);
            song.event.edit_response(message);
            queue.addSong(song, songInfo);
        }
        else
        {
            bot.log(dpp::ll_debug, "Jade in VC, streaming audio.");
            song.event.edit_response("Processing your request!");
            stream_audio_to_discord(bot, song, songInfo);
        }
    }
    else
    {
        bot.log(dpp::ll_debug, "Jade not in VC, attempting to connect then stream.");
        song.event.edit_response("Processing your request!");
        queue.addSong(song, songInfo);
        song.event.from->connect_voice(guild->id, song.event.command.channel_id, false, true);
    }
}
