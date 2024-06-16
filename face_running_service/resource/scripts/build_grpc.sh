sudo -s
cd /opt/
apt-get install libre2-dev -y && \
apt-get install -y build-essential autoconf libtool pkg-config && \
apt-get -y install libyaml-cpp-dev && \
git clone --recurse-submodules -b v1.34.0 https://github.com/grpc/grpc && \
cd grpc && \
mkdir -p cmake/build && \
cd cmake/build && \
cmake -DgRPC_INSTALL=ON -DBUILD_DEPS=ON \
-DgRPC_BUILD_TESTS=OFF \
-DCMAKE_INSTALL_PREFIX=$MY_INSTALL_DIR \
../.. && \
make -j8 && \
make install