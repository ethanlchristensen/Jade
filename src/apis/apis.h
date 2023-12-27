#include <iostream>
#include <curl/curl.h>

#define OPENAIURL "https://api.openai.com/v1/chat/completions"
#define MODEL "gpt-3.5-turbo"

std::string generate();