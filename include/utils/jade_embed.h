#ifndef JADE_JADE_EMBED_H
#define JADE_JADE_EMBED_H

#include <dpp/dpp.h>
#include <fmt/format.h>
#include "utils/jade_queue.h"

dpp::embed getNowPlayingEmbed(const SongRequest& song, const SongInfo& songInfo);
dpp::embed getAddedToQueueEmbed(const SongRequest& song, const SongInfo& songInfo);
dpp::embed getChatEmbed(const dpp::slashcommand_t& event, const std::string& model, const std::string& message, const std::string& response);
dpp::embed getDescriptionEmbed(const dpp::slashcommand_t& event, const std::string& description, const std::string& imageUrl);

#endif //JADE_JADE_EMBED_H
