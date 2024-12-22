#include <stdexcept>
#include "utils/jade_queue.h"

void JadeQueue::addSong(const SongRequest& request, const SongInfo& info) {
    QueueEntry entry{request, info};
    songQueue.push(entry);
}

QueueEntry JadeQueue::nextRequest() {
    if (songQueue.empty()) {
        throw std::out_of_range("No song requests in the queue.");
    }
    QueueEntry next = songQueue.front();
    songQueue.pop();
    return next;
}

bool JadeQueue::isEmpty() const {
    return songQueue.empty();
}