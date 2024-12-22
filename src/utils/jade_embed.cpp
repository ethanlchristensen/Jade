#include "utils/jade_embed.h"

dpp::embed getNowPlayingEmbed(const SongRequest& song, const SongInfo& songInfo) {
    dpp::embed embed = dpp::embed();
    embed.set_thumbnail("https://i.imgur.com/VARKMtQ.png");
    embed.set_color(0x0099ff);
    embed.set_title("Now Playing");
    embed.add_field("Title", fmt::format("[{}]({})", songInfo.title, song.query), false);
    embed.add_field("Artist", fmt::format("[{}]({})", songInfo.artist, songInfo.artistUrl), false);
    embed.add_field("Filter", song.filter.empty() ? "No Filter" : song.filter);
    embed.set_footer(dpp::embed_footer().set_text("/play by " + song.event.command.usr.global_name).set_icon(song.event.command.usr.get_avatar_url()));
    return embed;
}

dpp::embed addedToQueueEmbed(const std::string& query, const std::string& filter, const dpp::slashcommand_t& event) {
    return {};
}

dpp::embed chatEmbed(const dpp::slashcommand_t& event, const std::string& model, const std::string& message, const std::string& response) {
    dpp::embed embed = dpp::embed();
    embed.set_title("Chat Response");
    embed.add_field(event.command.usr.global_name, message);
    embed.add_field(model, response);
    embed.set_footer(dpp::embed_footer().set_text("/chat by " + event.command.usr.global_name).set_icon(event.command.usr.get_avatar_url()));
    return embed;
}


