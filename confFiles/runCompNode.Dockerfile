FROM ubuntu:22.04 as build
LABEL authors="Meynik A.V."
LABEL task = "Runn compNode"

RUN apt-get update && \
    apt-get install -y build-essential ninja-build git wget unzip cmake \
    libomp-dev libssl-dev libpq-dev libjsoncpp-dev libfmt-dev uuid-dev \
    zlib1g-dev libcurl4-openssl-dev libeigen3-dev && \
    cd /home && \
    mkdir deps && \
    chmod -R 777 deps && \
    cd deps


RUN  git clone https://github.com/drogonframework/drogon && \
     cd drogon && \
     git submodule update --init && \
     mkdir build && cd build && \
     cmake -DBUILD_CTL=OFF -DBUILD_EXAMPLES=OFF -DCOZ_PROFILING=OFF .. -G Ninja && \
     ninja && \
     ninja install && \
     ln -s /usr/include/jsoncpp/json/ /usr/include/json

RUN wget -q https://github.com/AlexanderMeynik/distributedMathArh/releases/download/dependencies/boostSubset.zip && \
    unzip -q boostSubset.zip && \
    rm boostSubset.zip && \
    cp -r boostSubset/boost/ /usr/include/ && \
    rm -rf boostSubset


RUN cd /home/deps && \
    git clone https://github.com/jtv/libpqxx.git && \
	cd libpqxx && \
    git reset --hard 58379ef56a8d4e95e9fe76674e38cdcfc0ca725f && \
    mkdir build&&cd build && \
    cmake -DSKIP_BUILD_TEST=OFF -DBUILD_SHARED_LIBS=off -G Ninja .. && \
    cmake --build . && \
    ninja install

RUN cd /home/deps && \
    git clone https://github.com/CopernicaMarketingSoftware/AMQP-CPP.git && \
	cd AMQP-CPP/ && \
    mkdir build && cd build && \
    cmake .. -DAMQP-CPP_LINUX_TCP=ON -G Ninja && \
    cmake --build . && \
    ninja install

RUN git clone https://github.com/eliaskosunen/scnlib && \
    cd scnlib/ && \
    git reset --hard e937be1a52588621b406d58ce8614f96bb5de747 && \
    cmake -DBUILD_TESTING=OFF -G Ninja .. && \
    cmake --build . && \
    ninja install

FROM ubuntu:22.04
COPY --from=build /usr /usr
COPY --from=build /etc /etc
COPY ./ /usr/application/src

WORKDIR /usr/application/build
ENV PORT = 8080

RUN cmake -G Ninja  ../src/ && \
    ninja compNode

WORKDIR /usr/application/build/computationalNode/bin
RUN chmod +x compNode


ENTRYPOINT ./compNode $PORT