# syntax=docker/dockerfile:1
FROM docker.io/ubuntu:22.04 AS qost-builder

RUN apt update

RUN apt install -y \
    pip \
    python3 \
    python-is-python3

# RUN rm -fr /var/lib/apt/lists/*
