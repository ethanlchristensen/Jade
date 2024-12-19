#ifndef JADE_QUEUE_H
#define JADE_QUEUE_H

#include <string>
#include <queue>
#include <dpp/dpp.h>

struct SongRequest {
    std::string query;
    std::string filter;
    const dpp::slashcommand_t event;
};

class JadeQueue {
public:
    void addSong(const std::string& query, const std::string& filter, dpp::slashcommand_t event);
    void addSong(const SongRequest newSong);
    SongRequest nextRequest();
    [[nodiscard]] bool isEmpty() const;

private:
    std::queue<SongRequest> songQueue;
};

#endif // JADE_QUEUE_H