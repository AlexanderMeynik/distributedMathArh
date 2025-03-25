FROM ubuntu:22.04 as build
LABEL authors="Meynik A.V."

#todo delete mathplot++
#todo github actions run stages 
#todo create runner image
RUN apt-get update && \
    apt-get install -y build-essential ninja-build git \
    wget unzip \
    cmake libomp-dev libssl-dev libpq-dev \
    libjpeg-dev libtiff5 libpng-dev zlib1g-dev \
    libjsoncpp-dev libfmt-dev uuid-dev zlib1g-dev && \
    cd /home && \
    mkdir deps && \
    chmod -R 777 deps && \
    cd deps

RUN apt-get install -y libeigen3-dev

RUN  git clone https://github.com/drogonframework/drogon && \
     cd drogon && \
     git submodule update --init && \
     mkdir build && cd build && \
     cmake -DBUILD_CTL=OFF -DBUILD_EXAMPLES=OFF -DCOZ_PROFILING=OFF .. -G Ninja && \
     ninja && \
     ninja install && \
     ln -s /usr/include/jsoncpp/json/ /usr/include/json


RUN wget -q https://github.com/AlexanderMeynik/distributedMathArh/releases/download/dependencies/gausQuadratureMin.zip && \
    unzip -q gausQuadratureMin.zip && \
    rm gausQuadratureMin.zip && \
    cp -r gausQuadratureMin/boost/ /usr/include/ && \
    rm -rf gausQuadratureMin

RUN git clone https://github.com/google/glog.git && \
    cd glog && \
    mkdir build && \
    cd build && \
    cmake -G Ninja ..  && \
    cmake --build . && \
    ninja install

RUN cd /home/deps/&& git clone https://github.com/alandefreitas/matplotplusplus.git && \
    cd matplotplusplus && \
	git reset --hard fe308e13f6c3ac7c5bd2b2f4717961d1d56d07ea && \
    cmake -B build/system         \
    -DMATPLOTPP_BUILD_EXAMPLES=OFF      \
    -DMATPLOTPP_BUILD_SHARED_LIBS=ON    \
    -DMATPLOTPP_BUILD_TESTS=OFF         \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=ON -G  Ninja . && \
    cmake --build build/system && \
    cd build/system/ && ninja install

RUN cd /home/deps && \
    git clone https://github.com/jtv/libpqxx.git && \
	cd libpqxx && \
    git reset --hard 58379ef56a8d4e95e9fe76674e38cdcfc0ca725f && \
    mkdir build&&cd build && \
    cmake -DSKIP_BUILD_TEST=OFF -DBUILD_SHARED_LIBS=off -G Ninja .. && \
    cmake --build . && \
    ninja install


RUN cd /home/deps && \
    git clone https://github.com/google/googletest && \
    cd googletest && \
    mkdir build&&cd build && \
    cmake -G Ninja  .. && \
    cmake --build . && \
    ninja install

FROM ubuntu:22.04 as base_env
COPY --from=build /usr /usr
COPY --from=build /etc /etc
WORKDIR /usr/application/src


FROM base_env as env
RUN apt-get update && \
    apt-get install  -y qt6-base-dev libqt6charts6-dev \
    libqt6datavisualization6-dev   \
    qt6-declarative-dev libgl-dev libopengl-dev
WORKDIR /usr/application/src
