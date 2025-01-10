FROM debian:stable-slim

# Set working directory
WORKDIR /usr/src/app

# Copy source files
COPY ./src /usr/src/app/src
COPY ./include /usr/src/app/include
COPY CMakeLists.txt .
COPY .env .

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
    ffmpeg \
    libspdlog-dev \
    opus-tools \
    libfmt-dev \
    libavformat-dev \
    libavcodec-dev \
    libavutil-dev \
    libavfilter-dev \
    libcurl4-openssl-dev \
    yt-dlp \
    libmpg123-dev \
    mpg123 \
    libcodec2-dev \
    nlohmann-json3-dev && \
    apt-get clean && rm -rf /var/lib/apt/lists/*

rUN yt-dlp -U

# Clone, build, and install cppcodec
RUN git clone https://github.com/tplgy/cppcodec.git && \
    cd cppcodec && \
    mkdir build && cd build && \
    cmake .. && \
    make && make install && \
    cd /usr/src/app && rm -rf cppcodec

# Download and install DPP
RUN wget -O dpp.deb https://dl.dpp.dev/latest/linux-rpi-arm64 && \
    dpkg -i dpp.deb && \
    rm dpp.deb

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