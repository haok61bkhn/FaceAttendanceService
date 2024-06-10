
BINARY=./build/main
clear
if [[ $# == 0 ]]; then
    echo Keep build directory
    cd build
    cmake ..
    make -j16
    cd ..
    $BINARY
else
    echo Remove build directory
    rm -rf build
    mkdir build
    cd build
    cmake ..
    make -j16
    cd ..
    $BINARY
fi




