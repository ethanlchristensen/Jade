#ifndef JADE_STREAM_AUDIO_H
#define JADE_STREAM_AUDIO_H

void stream_audio_to_discord(dpp::cluster &bot, const dpp::slashcommand_t &event, std::string query_or_link, const std::string& filter);
void stream_audio_secondary(dpp::cluster &bot, const dpp::voice_ready_t &event, std::string query_or_link, std::uint64_t channel_id, const std::string& filter);

#endif
