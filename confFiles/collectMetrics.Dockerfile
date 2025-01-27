FROM ubuntu:latest
LABEL authors="Lenovo"
RUN apt-get install -y python3 python3-pip python3-venv && \
    python3 -m venv /opt/venv && \
    pip install --no-cache-dir --upgrade pip && \
    pip install --no-cache-dir metrixpp




WORKDIR /usr/application/src

ENTRYPOINT ["top", "-b"]