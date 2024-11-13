# syntax=docker/dockerfile:1
FROM docker.io/ubuntu:24.04 AS qost-builder

RUN apt update && apt install -y \
    cmake \
    git \
    libreadline-dev \
    ninja-build \
    pip \
    python-is-python3 \
    python3 \
    sudo

COPY requirements.txt requirements.txt

RUN pip install --break-system-packages -r requirements.txt

# RUN rm -fr /var/lib/apt/lists/*

# Passwordless sudo for all
RUN sed -i 's,(ALL:ALL) ALL,(ALL:ALL) NOPASSWD: ALL,g' /etc/sudoers
