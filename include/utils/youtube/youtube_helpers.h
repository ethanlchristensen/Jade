#ifndef JADE_YOUTUBE_HELPERS_H
#define JADE_YOUTUBE_HELPERS_H

#include <regex>
#include <iostream>
#include "utils/jade_queue.h"
#include "utils/jade_util.h"

bool isValidURL(const std::string& input);
std::string getYoutubeUrl(const std::string& query);
SongInfo getSongInfo(const std::string& url);

#endif //JADE_YOUTUBE_HELPERS_H
