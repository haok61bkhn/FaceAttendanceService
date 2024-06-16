SRC=src/deepstream_src/parser/yolov5_parser
CUDA_VERSION=12.2

cd $SRC
CUDA_VER=$CUDA_VERSION make
CUDA_VER=$CUDA_VERSION make install