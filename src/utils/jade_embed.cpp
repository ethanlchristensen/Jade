#include "utils/jade_embed.h"

dpp::embed NowPlayingEmbed(const SongRequest& song, const SongInfo& songInfo) {
    dpp::embed embed = dpp::embed();
    embed.set_thumbnail(songInfo.thumbnailUrl.empty() ? "https://i.imgur.com/VARKMtQ.png" : songInfo.thumbnailUrl);
    embed.set_color(0x0099ff);
    embed.set_title("Now Playing");
    embed.add_field("Title", song.query.empty() ? "Unknown" : fmt::format("[{}]({})", songInfo.title, song.query), false);
    embed.add_field("Artist", songInfo.artistUrl.empty()? "Unknown" : fmt::format("[{}]({})", songInfo.artist, songInfo.artistUrl), false);
    embed.add_field("Duration", secondsToHHMMSS(songInfo.duration), false);
    embed.add_field("Filter", song.filter.empty() ? "No Filter" : song.filter);
    std::string avatarUrl = song.event.command.member.get_avatar_url();
    if (avatarUrl.empty()) {
        avatarUrl = song.event.command.usr.get_avatar_url();
    }
    embed.set_footer(dpp::embed_footer().set_text("/play by " + song.event.command.member.get_nickname()).set_icon(avatarUrl));
    return embed;
}

dpp::embed AddedToQueueEmbed(const SongRequest& song, const SongInfo& songInfo) {
    dpp::embed embed = dpp::embed();
    embed.set_thumbnail("https://i.imgur.com/VARKMtQ.png");
    embed.set_color(0x0099ff);
    embed.set_title("Added to the Queue");
    embed.add_field("Title", fmt::format("[{}]({})", songInfo.title, song.query), false);
    embed.add_field("Artist", fmt::format("[{}]({})", songInfo.artist, songInfo.artistUrl), false);
    embed.add_field("Filter", song.filter.empty() ? "No Filter" : song.filter);
    std::string avatarUrl = song.event.command.member.get_avatar_url();
    if (avatarUrl.empty()) {
        avatarUrl = song.event.command.usr.get_avatar_url();
    }
    embed.set_footer(dpp::embed_footer().set_text("/play by " + song.event.command.member.get_nickname()).set_icon(avatarUrl));
    return embed;
}

dpp::embed ChatEmbed(const dpp::slashcommand_t& event, const std::string& model, const std::string& message, const std::string& response) {
    dpp::embed embed = dpp::embed();
    embed.set_title("Chat Response");
    embed.add_field(event.command.usr.username, message);
    embed.add_field(model, response);

    std::string avatarUrl = event.command.member.get_avatar_url();
    if (avatarUrl.empty()) {
        avatarUrl = event.command.usr.get_avatar_url();
    }

    embed.set_footer(dpp::embed_footer().set_text("/chat by " + event.command.member.get_nickname()).set_icon(avatarUrl));
    return embed;
}

dpp::embed DescriptionEmbed(const dpp::slashcommand_t& event, const std::string& description, const std::string& imageUrl) {
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
    std::string avatarUrl = event.command.member.get_avatar_url();
    if (avatarUrl.empty()) {
        avatarUrl = event.command.usr.get_avatar_url();
    }
    embed.set_footer(dpp::embed_footer().set_text("/describe by " + event.command.member.get_nickname()).set_icon(avatarUrl));
    return embed;
}


