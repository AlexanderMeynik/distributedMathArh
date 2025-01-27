FROM ubuntu:latest
LABEL authors="Meynik A.V."


#todo replace cpp rest with other
# add designated images for different parts
RUN apt-get update && \
    apt-get install -y build-essential ninja-build git \
    libssl-dev libpq-dev cmake doxygen \
    libjpeg-dev libtiff6 libpng-dev zlib1g-dev \
    libcpprest-dev libomp-dev\
    qt6-base-dev libqt6charts6-dev zip && \
    cd /home && \
    mkdir deps && \
    chmod -R 777 deps && \
    cd deps

RUN apt-get install -y libeigen3-dev linux-tools-common \
    linux-tools-generic linux-tools-$(uname -r)


RUN git clone https://github.com/google/glog.git && \
    cd glog && \
    mkdir build && \
    cd build && \
    cmake -G Ninja ..  && \
    cmake --build . && \
    ninja install


RUN git clone https://github.com/alandefreitas/matplotplusplus.git && \
    cd matplotplusplus && \
    mkdir build && \
    cd build && \
    cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-O2" -DMATPLOTPP_BUILD_EXAMPLES=OFF -DMATPLOTPP_BUILD_TESTS=OFF -G  Ninja .. && \
    cmake --build . --config Release && \
    ninja install

RUN cd /home/deps && \
    git clone https://github.com/jtv/libpqxx.git && \
    cd libpqxx && \
    mkdir build&&cd build && \
    cmake -DSKIP_BUILD_TEST=ON -G Ninja .. && \
    cmake --build . && \
    ninja install



RUN cd /home/deps && \
    git clone https://github.com/google/googletest && \
    cd googletest && \
    mkdir build&&cd build && \
    cmake -G Ninja  .. && \
    cmake --build . && \
    ninja install

RUN apt-get install -y python3 python3-pip python3-venv && \
    python3 -m venv /opt/venv && \
    /opt/venv/bin/pip install --no-cache-dir --upgrade pip && \
    /opt/venv/bin/pip install --no-cache-dir metrixpp






WORKDIR /usr/application/src
