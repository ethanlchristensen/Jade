A C++ Discord bot made using the D++ library.
- _bruhshell (bs) dev (Ethan Christensen, etchris)_

![bruhshelllogo](https://github.com/ethanlchristensen/Jade/assets/55725575/3ee45b42-7a86-4ad6-bcc2-d9b9b1e0b6fa)

## Setup Instructions

### Prerequisites
- Docker
- Docker Compose
- [Ollama](https://ollama.com/)
- [Discord Application](https://discord.com/developers/applications)

### Running the Bot

1. **Clone the Repository:**

    ```bash
    git clone https://github.com/ethanlchristensen/Jade.git
    cd Jade
    ```

2. **Create a `.env` File:**

    Within the root of your project, create a `.env` file with the following content:

    ```
    DISCORD_BOT_TOKEN=<TOKEN>
    USERS_TO_ID={"<common-name>": "<@ID>"} # This will overwrite names with the actaul @'s when generating llm responses
    OLLAMA_ENDPOINT=localhost:11434 # OR if hosted on another machine x.x.x.x:11434
    ```
   
   Make sure to replace `<token>` and other placeholders with your actual configuration.

3. **Use Docker Compose to Build and Run:**

   ```bash
   docker-compose up --build -d
   ```
   This command will set up and run the bot using the Dockerfile and docker-compose.yaml provided in the project.

   To check the logs:
   ```bash
   docker logs jade   
   ```