#ifndef JADE_STREAM_AUDIO_H
#define JADE_STREAM_AUDIO_H

#include <dpp/dpp.h>
#include <fmt/format.h>
#include "utils/jade_embed.h"
#include "utils/jade_queue.h"

// Forward declarations
class PipeGuard;

/**
 * Streams audio from a YouTube link or other supported source to a Discord voice channel.
 * 
 * @param bot The Discord bot cluster instance
 * @param song The song request containing query and voice connection information
 * @param songInfo Information about the song being played
 */
void stream_audio_to_discord(dpp::cluster &bot, SongRequest song, const SongInfo& songInfo);

/**
 * Executes a shell command and returns its output as a string.
 * This function should be used with caution to prevent command injection.
 * 
 * @param command The shell command to execute
 * @return The output of the command as a string
 */
std::string executeCommand(const std::string& command);

/**
 * Sanitizes a query string to prevent command injection when used in shell commands.
 * 
 * @param query The query string to sanitize
 * @return A sanitized version of the query string
 */
std::string sanitizeQuery(const std::string& query);

#endif // JADE_STREAM_AUDIO_H