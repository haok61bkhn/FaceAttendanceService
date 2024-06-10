import glob
import time

import cv2

# Load model
import numpy as np
import py_ai
import yaml

with open("config/yolov8_det_py.yaml", "r") as f:
    config = yaml.load(f, Loader=yaml.FullLoader)["OBJECT_DET"]
    detector = py_ai.DETECTION_YOLOV8(
        config["onnx_file"],
        config["engine_file"],
        config["lable_file"],
        config["BATCH_SIZE"],
        config["IMAGE_WIDTH"],
        config["IMAGE_HEIGHT"],
        config["INPUT_CHANNEL"],
    )


video_path = "/home/haobk/video.mp4"
cap = cv2.VideoCapture(video_path)
while True:
    _, image = cap.read()
    if image is None:
        break
    t1 = time.time()
    dets = detector.Detect(image)
    t2 = time.time()
    print("time detect = ", t2 - t1)
    image_draw = detector.DrawDet(image, dets)
    cv2.imshow("image", image_draw)
    cv2.waitKey(1)
