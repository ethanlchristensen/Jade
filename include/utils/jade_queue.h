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

struct SongInfo {
    std::string url;
    std::string title;
    std::string artist;
    std::string artistUrl;
};

struct QueueEntry {
    SongRequest request;
    SongInfo info;
};

class JadeQueue {
public:
    void addSong(const SongRequest& request, const SongInfo& info);
    QueueEntry nextRequest();
    [[nodiscard]] bool isEmpty() const;

private:
    std::queue<QueueEntry> songQueue;
};

#endif // JADE_QUEUE_H