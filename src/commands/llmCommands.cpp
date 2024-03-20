#include "commands.h"
#include "../apis/apis.h"
#include <sqltypes.h>
#include <sql.h>
#include <sqlext.h>
#include <iostream>
#include <string>

dpp::slashcommand chat_command() {
    // create the command
    dpp::slashcommand chat = dpp::slashcommand();
    chat.set_name("chat");
    chat.set_description("Chat with me <3");
    chat.add_option(dpp::command_option(dpp::co_string, "message", "The message to send to Jade.", true));
    return chat;
}

void chat_process(dpp::cluster &bot, const dpp::slashcommand_t &event, const std::string& apiToken) {

    event.thinking(false);

    // run the command
    std::string pre_user_prompt = std::get<std::string>(event.get_parameter("message"));
    // need a better way to do this
    std::replace(pre_user_prompt.begin(), pre_user_prompt.end(), '/', ' ');
    std::replace(pre_user_prompt.begin(), pre_user_prompt.end(), '{', ' ');
    std::replace(pre_user_prompt.begin(), pre_user_prompt.end(), '}', ' ');
    std::replace(pre_user_prompt.begin(), pre_user_prompt.end(), '\\', ' ');
    // call gpt
    std::string response = llm(apiToken, SYSTEMPROMPT, pre_user_prompt);
    // check to make sure we are 2000 characters, else chunk
    if (response.size() > 1500) {
        size_t pos = 0;
        while (pos < response.size()) {
            size_t end_pos = pos + 1500;
            if (end_pos < response.size()) { // If not the last chunk
                size_t last_space = response.rfind(' ', end_pos);
                if (last_space != std::string::npos && last_space > pos) {
                    end_pos = last_space;
                }
            }
            dpp::message chunk(event.command.channel_id, response.substr(pos, end_pos - pos), dpp::mt_default);

            bot.log(dpp::ll_debug, fmt::format("pos -> {}", pos));
            if (pos == 0)
                event.edit_response(chunk);
            else
                bot.message_create(chunk);

            pos = end_pos;
            if (pos < response.size() && response[pos] == ' ') {
                ++pos; // Skip the space at the beginning of the next chunk
            }
        }
    } else {
        dpp::message msg(event.command.channel_id, response, dpp::mt_default);
        event.edit_response(msg);
    }
}

dpp::slashcommand summarize_command() {
    // create the command
    dpp::slashcommand summarize = dpp::slashcommand();
    summarize.set_name("summarize");
    summarize.set_description("Let me summarize a piece of text for you!");
    summarize.add_option(dpp::command_option(dpp::co_string, "message", "The message to send to Jade for her to summarize.", true));
    return summarize;
}

void summarize_process(dpp::cluster &bot, const dpp::slashcommand_t &event, const std::string& apiToken) {
    // run the command
    event.thinking(false);
    std::string pre_user_prompt = std::get<std::string>(event.get_parameter("message"));
    // need a better way to do this
    std::replace(pre_user_prompt.begin(), pre_user_prompt.end(), '/', ' ');
    std::replace(pre_user_prompt.begin(), pre_user_prompt.end(), '{', ' ');
    std::replace(pre_user_prompt.begin(), pre_user_prompt.end(), '}', ' ');
    std::replace(pre_user_prompt.begin(), pre_user_prompt.end(), '\\', ' ');
    // call gpt
    std::string response = llm(apiToken, SYSTEMPROMPT, fmt::format("TASK: {} INPUT: {}", SUMMARIZEPROMPT, pre_user_prompt));
    dpp::message msg(event.command.channel_id, response, dpp::mt_default);
    event.edit_response(msg);


    // writing to SQL!!!
    // https://tenor.com/view/social-credit-gif-23976170
    SQLHANDLE sqlEnvHandle;
    SQLHANDLE sqlConnHandle;
    SQLHANDLE sqlStmtHandle;
    SQLRETURN retCode;

    // Allocate environment handle
    if (SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &sqlEnvHandle) != SQL_SUCCESS) {
        std::cout << "Error allocating environment handle." << std::endl;
        return;
    }

    // Set the ODBC version environment attribute
    if (SQLSetEnvAttr(sqlEnvHandle, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0) != SQL_SUCCESS) {
        std::cout << "Error setting ODBC version." << std::endl;
        return;
    }

    // Allocate connection handle
    if (SQLAllocHandle(SQL_HANDLE_DBC, sqlEnvHandle, &sqlConnHandle) != SQL_SUCCESS) {
        std::cout << "Error allocating connection handle." << std::endl;
        return;
    }

    // Connect to the database
    SQLCHAR* connectionString = (SQLCHAR*)"DRIVER={SQL Server};SERVER=ETCHRIS\\SQLEXPRESS;DATABASE=tododb;Trusted_Connection=yes;";
    retCode = SQLDriverConnect(sqlConnHandle, NULL, connectionString, SQL_NTS, NULL, 0, NULL, SQL_DRIVER_COMPLETE);
    if (retCode != SQL_SUCCESS && retCode != SQL_SUCCESS_WITH_INFO) {
        std::cout << "Error connecting to database." << std::endl;
        return;
    }

    // Allocate statement handle
    if (SQLAllocHandle(SQL_HANDLE_STMT, sqlConnHandle, &sqlStmtHandle) != SQL_SUCCESS) {
        std::cout << "Error allocating statement handle." << std::endl;
        return;
    }

    // Prepare SQL statement
    std::replace(response.begin(), response.end(), '\'', '\\');
    std::cout << "SQL QUERY: " << fmt::format("INSERT INTO OpenAiLogging (RecordWrittenUtc, JSONPayload, FunctionCalled) VALUES (GETUTCDATE(), '{}', 'Summarize')", response).c_str() << std::endl;
    SQLCHAR* query = (SQLCHAR*) fmt::format("INSERT INTO OpenAiLogging (RecordWrittenUtc, JSONPayload, FunctionCalled) VALUES (GETUTCDATE(), '{}', 'Summarize')", response).c_str();
    retCode = SQLExecDirect(sqlStmtHandle, query, SQL_NTS);
    if (retCode != SQL_SUCCESS && retCode != SQL_SUCCESS_WITH_INFO) {
        std::cout << "Error executing SQL statement." << std::endl;
        return;
    }

    // Free handles
    SQLFreeHandle(SQL_HANDLE_STMT, sqlStmtHandle);
    SQLDisconnect(sqlConnHandle);
    SQLFreeHandle(SQL_HANDLE_DBC, sqlConnHandle);
    SQLFreeHandle(SQL_HANDLE_ENV, sqlEnvHandle);
}

dpp::slashcommand extract_command() {
    // create the command
    dpp::slashcommand extract = dpp::slashcommand();
    extract.set_name("extract");
    extract.set_description("Let me help you extract some topics!");
    extract.add_option(dpp::command_option(dpp::co_string, "message", "The message to send to Jade to extract topics from.", true));
    return extract;
}

void extract_process(dpp::cluster &bot, const dpp::slashcommand_t &event, const std::string& apiToken) {
    // run the command
    event.thinking(false);
    std::string pre_user_prompt = std::get<std::string>(event.get_parameter("message"));
    // need a better way to do this
    std::replace(pre_user_prompt.begin(), pre_user_prompt.end(), '/', ' ');
    std::replace(pre_user_prompt.begin(), pre_user_prompt.end(), '{', ' ');
    std::replace(pre_user_prompt.begin(), pre_user_prompt.end(), '}', ' ');
    std::replace(pre_user_prompt.begin(), pre_user_prompt.end(), '\\', ' ');
    // call gpt
    std::string response = llm(apiToken, SYSTEMPROMPT, fmt::format("TASK: {} INPUT: {}", EXTRACTPROMPT, pre_user_prompt));
    dpp::message msg(event.command.channel_id, response, dpp::mt_default);
    event.edit_response(msg);
}
