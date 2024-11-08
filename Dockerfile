# syntax=docker/dockerfile:1
FROM docker.io/ubuntu:24.04 AS qost-builder

RUN apt update

RUN apt install -y \
    ninja-build \
    pip \
    python-is-python3 \
    python3 \
    sudo

RUN pip install --break-system-packages ninja_syntax

RUN apt install -y \
    cmake

RUN apt install -y \
    libreadline-dev

# RUN rm -fr /var/lib/apt/lists/*

# Passwordless sudo for all
RUN sed -i 's,(ALL:ALL) ALL,(ALL:ALL) NOPASSWD: ALL,g' /etc/sudoers
