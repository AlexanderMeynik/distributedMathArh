FROM ubuntu:latest
LABEL authors="Meynik A.V."

RUN apt-get update && \
    DEBIAN_FRONTEND="noninteractive" TZ="Europe/Moscow" apt-get install -y tzdata && \
    apt-get install -y git python3 python3-pip python3-venv && \
    python3 -m venv .venv && \
    . .venv/bin/activate && \
    pip install --upgrade pip && \
    pip install metrixpp



WORKDIR /usr/application/src
