sudo -s
sudo apt install build-essential libboost-system-dev libboost-thread-dev libboost-program-options-dev libboost-test-dev
cd /opt/
git clone https://github.com/mfontanini/cppkafka.git
cd cppkafka && mkdir build && cd build
cmake ..
make
make install