FROM debian:wheezy

WORKDIR /build

RUN apt-get update && apt-get install -y --no-install-recommends \
    ca-certificates \
    g++-multilib \
    gcc-multilib \
    git \
    make \
    curl \
    wget \
    zip \
    lib32stdc++6 \
    unzip \
    && apt-get clean && rm -rf /var/lib/apt/lists/*
