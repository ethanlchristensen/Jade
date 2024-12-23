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

dpp::embed getAddedToQueueEmbed(const SongRequest& song, const SongInfo& songInfo) {
    dpp::embed embed = dpp::embed();
    embed.set_thumbnail("https://i.imgur.com/VARKMtQ.png");
    embed.set_color(0x0099ff);
    embed.set_title("Added to the Queue");
    embed.add_field("Title", fmt::format("[{}]({})", songInfo.title, song.query), false);
    embed.add_field("Artist", fmt::format("[{}]({})", songInfo.artist, songInfo.artistUrl), false);
    embed.add_field("Filter", song.filter.empty() ? "No Filter" : song.filter);
    embed.set_footer(dpp::embed_footer().set_text("/play by " + song.event.command.usr.global_name).set_icon(song.event.command.usr.get_avatar_url()));
    return embed;
}

dpp::embed getChatEmbed(const dpp::slashcommand_t& event, const std::string& model, const std::string& message, const std::string& response) {
    dpp::embed embed = dpp::embed();
    embed.set_title("Chat Response");
    embed.add_field(event.command.usr.global_name, message);
    embed.add_field(model, response);
    embed.set_footer(dpp::embed_footer().set_text("/chat by " + event.command.usr.global_name).set_icon(event.command.usr.get_avatar_url()));
    return embed;
}

dpp::embed getDescriptionEmbed(const dpp::slashcommand_t& event, const std::string& description, const std::string& imageUrl) {
    dpp::embed embed = dpp::embed();
    embed.set_title("Describe");
    embed.set_image(imageUrl);

    // field can only be 1024 characters
    size_t maxFieldLength = 1024;
    size_t pos = 0;
    size_t prev_pos = 0;

    // try to split on full sentences
    while (pos < description.length()) {
        if (pos - prev_pos >= maxFieldLength || pos == description.length() - 1) {
            size_t end = description.find_last_of(".!?", pos);
            if (end != std::string::npos && end > prev_pos) {
                pos = end + 1;
            }

            std::string chunk = description.substr(prev_pos, pos - prev_pos);
            if (prev_pos == 0) {
                embed.add_field("Description", chunk);
            } else {
                embed.add_field("", chunk);
            }
            prev_pos = pos;
        }
        pos++;
    }

    embed.set_footer(dpp::embed_footer().set_text("/describe by " + event.command.usr.global_name).set_icon(event.command.usr.get_avatar_url()));
    return embed;
}


