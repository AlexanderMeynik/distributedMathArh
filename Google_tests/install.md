
Install google tests+gmock
git clone https://github.com/google/googletest
cd googletest
mkdir build && cd build
cmake ..
make -j8
sudo checkinstall

удалить
dpkg -r googletest