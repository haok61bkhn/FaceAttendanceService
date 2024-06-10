sudo -s
cd /opt/
git clone https://github.com/mfontanini/cppkafka.git
cd cppkafka && mkdir build && cd build
cmake ..
make
make install