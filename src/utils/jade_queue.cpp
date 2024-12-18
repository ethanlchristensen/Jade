#include "utils/jade_queue.h"
#include <stdexcept>

void JadeQueue::addRequest(const std::string& query, const std::string& filter, dpp::slashcommand_t &event) {
    SongRequest request{query, filter, event};
    songQueue.push(request);
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
    std::cout << "[isEmpty] Size of the Queue is: " << songQueue.size() << "\n";
    return songQueue.empty();
}