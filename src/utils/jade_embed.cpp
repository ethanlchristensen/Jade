#include "utils/jade_embed.h"

dpp::embed getNowPlayingEmbed(const std::string& query, const std::string& filter, const dpp::slashcommand_t& event) {
    dpp::embed embed = dpp::embed();
    embed.set_thumbnail("https://i.imgur.com/VARKMtQ.png");
    embed.set_color(0x0099ff);
    embed.set_title("Now Playing");
    embed.add_field("Query", query);
    embed.add_field("Filter", filter.empty() ? "No Filter" : filter);
    embed.set_footer(dpp::embed_footer().set_text("/play by " + event.command.usr.global_name).set_icon(event.command.usr.get_avatar_url()));
    return embed;
}
