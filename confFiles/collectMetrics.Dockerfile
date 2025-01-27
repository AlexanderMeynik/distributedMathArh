FROM ubuntu:latest
LABEL authors="Lenovo"
RUN apt-get update && \
    DEBIAN_FRONTEND="noninteractive" TZ="Europe/Moscow" apt-get install -y tzdata && \
    apt-get install -y python3 python3-pip python3-venv && \
    python3 -m venv .venv && \
    source .venv/bin/activate && \
    pip install --upgrade pip && \
    pip install metrixpp


WORKDIR /usr/application/src

ENTRYPOINT ["top", "-b"]