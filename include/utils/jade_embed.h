#ifndef JADE_JADE_EMBED_H
#define JADE_JADE_EMBED_H

#include <dpp/dpp.h>

dpp::embed getNowPlayingEmbed(const std::string& query, const std::string& filter, const dpp::slashcommand_t& event);

#endif //JADE_JADE_EMBED_H
