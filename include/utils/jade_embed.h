#ifndef JADE_JADE_EMBED_H
#define JADE_JADE_EMBED_H

#include <dpp/dpp.h>
#include <fmt/format.h>
#include "utils/jade_queue.h"

dpp::embed getNowPlayingEmbed(const SongRequest& song, const SongInfo& songInfo);
dpp::embed addedToQueueEmbed(const std::string& query, const std::string& filter, const dpp::slashcommand_t& event);
dpp::embed chatEmbed(const dpp::slashcommand_t& event, const std::string& model, const std::string& message, const std::string& response);

#endif //JADE_JADE_EMBED_H
