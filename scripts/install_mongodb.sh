sudo apt-get install -y libmongoc-1.0-0
sudo apt-get install libbson-1.0-0 -y
sudo apt-get install cmake libssl-dev libsasl2-dev -y
sudo apt-get install python3-sphinx -y

cd /opt

wget https://github.com/mongodb/mongo-c-driver/releases/download/1.19.0/mongo-c-driver-1.19.0.tar.gz
tar xzf mongo-c-driver-1.19.0.tar.gz
cd mongo-c-driver-1.19.0
mkdir cmake-build
cd cmake-build
cmake -DENABLE_AUTOMATIC_INIT_AND_CLEANUP=OFF ..
sudo cmake --build . --target install
cmake -DENABLE_MAN_PAGES=ON -DENABLE_HTML_DOCS=ON ..
cmake --build . --target bson-doc



cd /opt

wget https://github.com/mongodb/mongo-cxx-driver/releases/download/r3.6.5/mongo-cxx-driver-r3.6.5.tar.gz

tar -xzf mongo-cxx-driver-r3.6.5.tar.gz

cd mongo-cxx-driver-r3.6.5/build

cmake ..                                           \
-DCMAKE_BUILD_TYPE=Release                      \
-DLIBMONGOC_DIR=/opt/mongo-c-driver             \
-DLIBBSON_DIR=/opt/mongo-c-driver               \
-DCMAKE_INSTALL_PREFIX=/opt/mongo-cxx-driver

cmake --build .

sudo cmake --build . --target install
