#include "utils/jade_queue.h"
#include <stdexcept>

void JadeQueue::addRequest(const std::string& query, const std::string& filter) {
    SongRequest request{query, filter};
    songQueue.push(request);
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