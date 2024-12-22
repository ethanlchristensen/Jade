#ifndef JADE_JADE_SLASH_H
#define JADE_JADE_SLASH_H

#include <dpp/dpp.h>
#include "commands/commands.h"
#include "commands/voice/join_command.h"
#include "commands/voice/leave_command.h"
#include "commands/voice/play_command.h"
#include "commands/voice/play_test_command.h"
#include "commands/voice/pause_command.h"
#include "commands/voice/resume_command.h"
#include "commands/voice/skip_command.h"
#include "commands/ollama/chat_command.h"
#include "commands/ollama/describe_command.h"

#include "utils/voice/stream_audio.h"
#include "utils/jade_queue.h"
#include "utils/ollama/ollama.h"

void processSlashCommand(dpp::cluster &bot, const dpp::slashcommand_t &event, JadeQueue &songQueue, OllamaAPI &ollamaApi);

#endif //JADE_JADE_SLASH_H
