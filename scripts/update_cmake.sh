wget https://github.com/Kitware/CMake/releases/download/v3.19.5/cmake-3.19.5-Linux-aarch64.tar.gz -q --show-progress 
tar -zxvf cmake-3.19.5-Linux-aarch64.tar.gz 
cd cmake-3.19.5-Linux-aarch64/
sudo cp -rf bin/ doc/ share/ /usr/local/
sudo cp -rf man/* /usr/local/man
sync
cmake --version 