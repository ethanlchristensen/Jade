#include <dpp/dpp.h>
#include <iostream>

dpp::slashcommand nolan_command();
dpp::slashcommand clear_command();
dpp::slashcommand echo_command();
dpp::slashcommand chat_command();

void nolan_process(dpp::cluster &bot, const dpp::slashcommand_t &event);
void clear_process(dpp::cluster &bot, const dpp::slashcommand_t &event);
void echo_process(dpp::cluster &bot, const dpp::slashcommand_t &event);
void chat_process(dpp::cluster &bot, const dpp::slashcommand_t &event, const std::string& apiToken);