import time

import cv2
import py_ai
import yaml

# Load model
image = cv2.imread("images/55958.jpg")
with open("config/scrfd_py.yaml", "r") as f:
    config = yaml.load(f, Loader=yaml.FullLoader)["SCRFD"]
    face_detector = py_ai.SCRFD(
        config["onnx_file"],
        config["engine_file"],
        config["BATCH_SIZE"],
        config["IMAGE_WIDTH"],
        config["IMAGE_HEIGHT"],
        config["INPUT_CHANNEL"],
        config["obj_threshold"],
        config["nms_threshold"],
    )

# image_draw = face_detector.Demo(image)

# cv2.imshow("image", image_draw)
# cv2.waitKey(0)

video = cv2.VideoCapture("video/test.mp4")
while True:
    ret, frame = video.read()
    if not ret:
        break
    start = time.time()
    image_draw = face_detector.Demo(frame)
    end = time.time()
    print("time = ", end - start)
    # cv2.imshow("image", image_draw)
    # cv2.waitKey(1)
