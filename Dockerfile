# C:\Users\ethan\Desktop\Github\Jade\Dockerfile
FROM debian:stable-slim

# Install required packages
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    wget \
    libssl-dev \
    gcc \
    g++ \
    zlib1g-dev \
    libsodium-dev \
    libopus-dev \
    libb64-dev \
    ffmpeg \
    libspdlog-dev \
    opus-tools \
    libfmt-dev \
    libavformat-dev \
    libavcodec-dev \
    libavutil-dev \
    libavfilter-dev \
    libcurl4-openssl-dev \
    libmpg123-dev \
    mpg123 \
    libcodec2-dev \
    nlohmann-json3-dev \
    python3 \
    python3-pip && \
    apt-get clean && rm -rf /var/lib/apt/lists/*

# Install yt-dlp via pip
RUN python3 -m pip install --break-system-packages --no-cache-dir yt-dlp

# Download and install DPP
RUN wget -O dpp.deb https://github.com/brainboxdotcc/DPP/releases/download/v10.1.2/libdpp-10.1.2-linux-rpi-arm64.deb && \
    dpkg -i dpp.deb && \
    rm dpp.deb

# Set working directory
WORKDIR /usr/src/app

# Copy source and configuration files
COPY CMakeLists.txt .
COPY ./src /usr/src/app/src
COPY ./include /usr/src/app/include

# Build the application
RUN mkdir -p build && \
    cd build && \
    cmake .. && \
    make

# Create a directory for the final executable
RUN mkdir -p /app && \
    cp /usr/src/app/build/Jade /app/

# Set the working directory for the entrypoint
WORKDIR /app

# Set the entry point for the container
ENTRYPOINT ["./Jade"]
