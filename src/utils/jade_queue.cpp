#include <stdexcept>
#include "utils/jade_queue.h"


void JadeQueue::addSong(const std::string& query, const std::string& filter, dpp::slashcommand_t event) {
    SongRequest song{query, filter, event};
    songQueue.push(song);
}

void JadeQueue::addSong(const SongRequest newSong) {
    songQueue.push(newSong);
}

SongRequest JadeQueue::nextRequest() {
    if (songQueue.empty()) {
        throw std::out_of_range("No song requests in the queue.");
    }
    SongRequest next = songQueue.front();
    songQueue.pop();
    return next;
}

bool JadeQueue::isEmpty() const {
    return songQueue.empty();
}