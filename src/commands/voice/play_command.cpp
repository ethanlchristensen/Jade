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

void play_process(dpp::cluster &bot, const dpp::slashcommand_t &event, std::string query_or_link,
                  const std::string &filter)
{
    event.thinking(true);

    dpp::guild *guild = dpp::find_guild(event.command.guild_id);

    if (!guild->connect_member_voice(event.command.get_issuing_user().id))
    {
        dpp::message error_msg(event.command.channel_id,
                               "I would love to play some music, but don't you want to listen too?", dpp::mt_default);
        event.edit_response(error_msg);
        return;
    }

    dpp::voiceconn *channel = event.from->get_voice(event.command.guild_id);

    if (channel && channel->voiceclient && channel->voiceclient->is_ready())
    {
        if (channel->voiceclient->is_playing())
        {
            event.edit_response("I am playing something right now, leave me alone!");
        }
        else
        {
            bot.log(dpp::ll_debug, "Jade in VC, streaming audio.");
            event.edit_response("Processing your request!");
            stream_audio_to_discord(bot, event, std::move(query_or_link), filter);
        }
    }
    else
    {
        bot.log(dpp::ll_debug, "Jade not in VC, attempting to connect then stream.");
        event.edit_response("Processing your request!");
        event.from->connect_voice(guild->id, event.command.channel_id, false, true);
    }
}
