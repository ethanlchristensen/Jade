#ifndef JADE_RESUME_COMMAND_H
#define JADE_RESUME_COMMAND_H

#include <dpp/dpp.h>

dpp::slashcommand resume_command();
void resume_process(dpp::cluster &bot, const dpp::slashcommand_t &event);

#endif
