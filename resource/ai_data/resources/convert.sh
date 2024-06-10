# /usr/src/tensorrt/bin/trtexec --onnx=yolov8s.onnx --saveEngine=yolov8s.engine --fp16
/usr/src/tensorrt/bin/trtexec --onnx=LicensePlate/char_320_1.onnx --saveEngine=LicensePlate/char_320_1.engine --fp16
/usr/src/tensorrt/bin/trtexec --onnx=LicensePlate/lpdet.onnx --saveEngine=LicensePlate/lpdet.engine --fp16

/usr/src/tensorrt/bin/trtexec --onnx=Face/scrfd_500m_320_1.onnx --saveEngine=Face/scrfd_500m_320_1.engine --fp16
/usr/src/tensorrt/bin/trtexec --onnx=Face/iresnet124_1.onnx --saveEngine=Face/iresnet124_1.engine --fp16
