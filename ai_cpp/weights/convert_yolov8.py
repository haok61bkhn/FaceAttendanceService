from ultralytics import YOLO

model = YOLO(
    "char_lp_yolov8n.pt"
)  # load a pretrained model (recommended for training)
success = model.export(format="onnx")  # export the model to ONNX format
