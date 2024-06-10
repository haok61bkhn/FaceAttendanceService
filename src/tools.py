import base64
import cv2
import os
import numpy as np
import base64


def base64_to_cv2(base64_string):
    buffer = base64.b64decode(base64_string)
    np_array = np.frombuffer(buffer, dtype=np.uint8)
    image = cv2.imdecode(np_array, cv2.IMREAD_COLOR)
    return image


def cv2_to_base64(image):
    _, buffer = cv2.imencode(".jpg", image)
    base64_string = base64.b64encode(buffer).decode("utf-8")

    return base64_string


def create_folder(folder_path):
    if not os.path.exists(folder_path):
        os.makedirs(folder_path)
