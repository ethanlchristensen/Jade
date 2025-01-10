#include "commands/voice/play_test_command.h"

dpp::slashcommand play_test_command()
{
    dpp::slashcommand play_test = dpp::slashcommand();
    play_test.set_name("playtest");
    play_test.set_description("Command to play audio in VC using a link or query,");
    play_test.add_option(dpp::command_option(dpp::co_string, "query_or_link", "Link to play or query to search.", true));
    dpp::command_option filters = dpp::command_option(dpp::co_string, "filter", "The filter to apply to the audio.",
                                                      false);
    for (auto &it : TEST_FILTERS)
    {
        filters.add_choice(dpp::command_option_choice(it.first, it.first));
    }
    play_test.add_option(filters);
    return play_test;
}

void play_test_process(dpp::cluster &bot, const dpp::slashcommand_t& event, std::string& query,  std::string& filter, JadeQueue &queue)
{
    event.thinking();
    if (!isValidURL(query)) {
        query = getYoutubeUrl(query);
    }
    SongInfo songInfo = getSongInfo(query);
    SongRequest song{query, filter, event};
    dpp::embed embed = getNowPlayingEmbed(song, songInfo);
    dpp::message now_playing_message(event.command.channel_id, embed);
    event.edit_response(now_playing_message);
}