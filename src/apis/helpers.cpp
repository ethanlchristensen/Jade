#include "apis.h"

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string *)userp)->append((char *)contents, size * nmemb);
    return size * nmemb;
}

size_t write_data_2(void *ptr, size_t size, size_t nmemb, std::stringstream *stream)
{
    size_t len = size * nmemb;
    stream->write(static_cast<char *>(ptr), len);
    return len;
}

size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}

std::string llm(const std::string &apiToken, const std::string &system_prompt, const std::string &user_prompt)
{
    // command to response to the users prompt
    CURL *curl;
    CURLcode response;
    std::string readBuffer;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl)
    {
        struct curl_slist *headers = nullptr;
        std::string auth = fmt::format("Authorization: Bearer {}", apiToken);
        headers = curl_slist_append(headers, "Content-Type: application/json");
        headers = curl_slist_append(headers, auth.c_str());
        std::string data = fmt::format(
            R"({{"model": "{}","messages": [{{"role": "system","content": "{}" }},{{"role": "user", "content": "{}" }}],"temperature": 1,"max_tokens": 500, "top_p": 1,"frequency_penalty": 0,"presence_penalty": 0 }})",
            MODEL, system_prompt, user_prompt);

        curl_easy_setopt(curl, CURLOPT_URL, OPENAIURL);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        response = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        curl_global_cleanup();

        // did we get response
        if (response == 0)
        {
            json responseJson = json::parse(readBuffer);
            return responseJson["choices"][0]["message"]["content"];
        }
        else
        {
            return "Hey, I am not in the mood to talk right now.";
        }
    }
    return "Hey, I am not in the mood to talk right now.";
}

std::int8_t tts(dpp::cluster &bot, const dpp::slashcommand_t &event, const std::string &apiToken, const std::string &text, const std::string &voice)
{
    // command to turn text into speech using open ai and send to discord VC.
    FILE *fp;
    CURL *curl;
    CURLcode response;
#if defined(_WIN32) || defined(_WIN64)
    errno_t err;
#elif defined(__APPLE__) && defined(__MACH__)
    error_t err;
#elif defined(__linux__)
    error_t err;
#elif defined(__unix__) || defined(__unix)
    error_t err;
#endif
    std::vector<uint8_t> pcmdata;
    mpg123_init();

    int error = 0;
    unsigned char *buffer;
    size_t buffer_size, done;
    int channels, encoding;
    long rate;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl)
    {

        bot.log(dpp::ll_debug, "[tts] -> attempting to download tts message.");

#if defined(_WIN32) || defined(_WIN64)
        err = fopen_s(&fp, TTSFILE, "wb");
#elif defined(__APPLE__) && defined(__MACH__)
        fp = fopen(TTSFILE, "wb");
#elif defined(__linux__)
        fp = fopen(TTSFILE, "wb");
#elif defined(__unix__) || defined(__unix)
        fp = fopen(TTSFILE, "wb");
#endif

#if defined(_WIN32) || defined(_WIN64)
        if (err != 0)
        {
            return -1;
        }
#elif defined(__APPLE__) && defined(__MACH__)
        if (fp == NULL)
        {
            return -1;
        }
#elif defined(__linux__)
        if (fp == NULL)
        {
            return -1;
        }
#elif defined(__unix__) || defined(__unix)
        if (fp == NULL)
        {
            return -1;
        }
#endif

        struct curl_slist *headers = nullptr;
        std::string auth = fmt::format("Authorization: Bearer {}", apiToken);
        headers = curl_slist_append(headers, "Content-Type: application/json");
        headers = curl_slist_append(headers, auth.c_str());
        std::string data = fmt::format(
            R"({{"model": "{}","input": "{}", "voice":"{}", "speed": 1.0}})",
            TTSMODEL, text, voice);

        std::cout << data << std::endl;

        curl_easy_setopt(curl, CURLOPT_URL, OPENAITTS);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 10);

        response = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
        fclose(fp);

        if (response != 0)
        {
            std::cout << "API call to OpenAI to TTS failed!" << std::endl;
        }
    }

    bot.log(dpp::ll_debug, "[tts] -> attempting to read tts.mp3 to send to vc.");
    mpg123_handle *mh = mpg123_new(nullptr, &error);
    mpg123_param(mh, MPG123_FORCE_RATE, 96000, 96000.0);

    buffer_size = mpg123_outblock(mh);
    buffer = new unsigned char[buffer_size];

    mpg123_open(mh, TTSFILE);
    mpg123_getformat(mh, &rate, &channels, &encoding);

    unsigned int counter = 0;
    for (size_t totalBytes = 0; mpg123_read(mh, buffer, buffer_size, &done) == MPG123_OK;)
    {
        for (size_t i = 0; i < buffer_size; i++)
        {
            pcmdata.push_back(buffer[i]);
        }
        counter += buffer_size;
        totalBytes += done;
    }

    delete[] buffer;
    mpg123_close(mh);
    mpg123_delete(mh);

    dpp::voiceconn *channel = event.from->get_voice(event.command.guild_id);

    if (channel && channel->voiceclient && channel->voiceclient->is_ready())
    {
        channel->voiceclient->set_send_audio_type(dpp::discord_voice_client::satype_overlap_audio);
        bot.log(dpp::ll_debug, "[tts] -> attempting to send tts audio to the vc!");
        channel->voiceclient->send_audio_raw((uint16_t *)pcmdata.data(), pcmdata.size());
    }

    return 0;
}

std::string findExecutablePath(const std::string &exeName)
{
    std::string cmd = "where " + exeName;
    std::array<char, 128> buffer;
    std::string result;
#if defined(_WIN32) || defined(_WIN64)
    auto pipe = _popen(data.c_str(), "rb");
#elif defined(__APPLE__) && defined(__MACH__)
    FILE *pipe = popen(cmd.c_str(), "r");
#elif defined(__linux__)
    FILE *pipe = popen(cmd.c_str(), "r");
#elif defined(__unix__) || defined(__unix)
    FILE *pipe = popen(cmd.c_str(), "r");
#endif
    if (!pipe)
    {
        std::cerr << "popen failed: " << strerror(errno) << '\n';
        return "";
    }
    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr)
    {
        result += buffer.data();
    }

#if defined(_WIN32) || defined(_WIN64)
    _pclose(pipe);
#elif defined(__APPLE__) && defined(__MACH__)
    pclose(pipe);
#elif defined(__linux__)
    pclose(pipe);
#elif defined(__unix__) || defined(__unix)
    pclose(pipe);
#endif
    return result.substr(0, result.find('\n')); // get the first line of output
}
