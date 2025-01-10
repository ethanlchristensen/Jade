#ifndef JADE_STREAM_AUDIO_H
#define JADE_STREAM_AUDIO_H

#include <dpp/dpp.h>
#include <fmt/format.h>
#include "utils/voice/stream_audio.h"
#include "utils/jade_embed.h"
#include "utils/jade_queue.h"

void stream_audio_to_discord(dpp::cluster &bot, SongRequest song, SongInfo songInfo);
std::string executeCommand(const std::string& command);

#endif
