#ifndef JADE_PLAY_COMMAND_H
#define JADE_PLAY_COMMAND_H

#include <regex>
#include <iostream>
#include <dpp/dpp.h>
#include "utils/jade_queue.h"
#include "utils/voice/stream_audio.h"
#include "utils/youtube/youtube_helpers.h"

static std::map<std::string, std::string> FILTERS = {
        {"VAPORWAVE", "\"highpass=f=50, lowpass=f=2750, aresample=48000, asetrate=48000*0.85,bass=g=5:f=110:w=0.6, compand=attacks=0:points=-80/-169|-54/-80|-49.5/-64.6|-41.1/-41.1|-25.8/-15|-10.8/-4.5|0/0|20/8.3\""},
        {"INTHEBATHROOM", "\"highpass=f=10, lowpass=f=400, aresample=44100, asetrate=44100*0.85,bass=g=4:f=110:w=0.6, alimiter=1, compand=attacks=0:points=-80/-169|-54/-80|-49.5/-64.6|-41.1/-41.1|-25.8/-15|-10.8/-4.5|0/0|20/8.3\""},
        {"NIGHTCORE", "\"aresample=48000, asetrate=48000*1.5\""},
        {"BASSBOOST", "\"bass=g=20:f=110:w=0.3\""},
        {"DIM", "apulsator=hz=0.09"},
        {"LOFI", "aresample=48000,asetrate=48000*0.9,extrastereo=m=2.5:c=disabled"},
        {"NOSILENCE", "\"silenceremove=1:0:-50dB\""},
        {"EXPANDER", "\"compand=attacks=0:points=-80/-169|-54/-80|-49.5/-64.6|-41.1/-41.1|-25.8/-15|-10.8/-4.5|0/0|20/8.3\""},
        {"None", ""}
};

dpp::slashcommand play_command();
void play_process(dpp::cluster &bot, const dpp::slashcommand_t& event, std::string& query,  std::string& filter, JadeQueue &queue);

#endif