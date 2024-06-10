import glob
import time

import cv2

# Load model
import numpy as np
import py_ai
import yaml


def resize_and_pad(image, target_size):
    height, width = image.shape[:2]
    aspect_ratio = float(width) / float(height)
    if aspect_ratio > 1:
        new_width = target_size
        new_height = int(target_size / aspect_ratio)
    else:
        new_width = int(target_size * aspect_ratio)
        new_height = target_size
    resized_image = cv2.resize(image, (new_width, new_height))
    canvas = np.zeros((target_size, target_size, 3), dtype=np.uint8)
    x_offset = int((target_size - new_width) / 2)
    y_offset = int((target_size - new_height) / 2)
    canvas[
        y_offset : y_offset + new_height, x_offset : x_offset + new_width
    ] = resized_image
    return canvas


with open("config/lpn_det_py.yaml", "r") as f:
    config = yaml.load(f, Loader=yaml.FullLoader)["LPN_DET"]
    lpn_detector = py_ai.LPN_DETECTION_YOLOV8(
        config["onnx_file"],
        config["engine_file"],
        config["BATCH_SIZE"],
        config["IMAGE_WIDTH"],
        config["IMAGE_HEIGHT"],
        config["INPUT_CHANNEL"],
        config["topk"],
        config["score_thres"],
        config["iou_thres"],
    )
with open("config/char_det_py.yaml", "r") as f:
    config = yaml.load(f, Loader=yaml.FullLoader)["CHAR_DET"]
    char_detector = py_ai.CHAR_DETECTION_YOLOV8(
        config["onnx_file"],
        config["engine_file"],
        config["lable_file"],
        config["BATCH_SIZE"],
        config["IMAGE_WIDTH"],
        config["IMAGE_HEIGHT"],
        config["INPUT_CHANNEL"],
    )


def predict(img):
    t1 = time.time()
    dets = lpn_detector.Detect(img)
    print(len(dets))
    print("time detect = ", time.time() - t1)
    t1 = time.time()
    for det in dets:
        lp_img = lpn_detector.Align_LPN(image, det.kps)
        lp_img = resize_and_pad(lp_img, 320)
        lp_type = det.label
        height = lp_img.shape[0]
        char_det = char_detector.Detect(lp_img)
        lp_text = char_detector.GetTextLPN(char_det, height)
        print("{} : LPN = {}".format(lp_type, lp_text))
        cv2.imshow("lp_img", lp_img)
        cv2.waitKey(0)


for path in glob.glob("images/data_LP_test/easy/*"):
    image = cv2.imread(path)
    t1 = time.time()
    predict(image)
    print("time = ", time.time() - t1)

    # cv2.imshow("image", image)
    # cv2.waitKey(0)
