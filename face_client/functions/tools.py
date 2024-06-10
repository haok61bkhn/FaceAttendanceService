import numpy as np
import os
from PyQt5.QtWidgets import QMessageBox
from PyQt5.QtWidgets import QFileDialog
import requests
import cv2
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


def login(username, password, ip):
    ip = "http://127.0.0.1:8000"
    url = ip + "/login"
    username = "device_id_01"
    password = "device_01"
    response = requests.post(
        url,
        data={"username": username, "password": password},
        headers={"Content-Type": "application/x-www-form-urlencoded"},
    )
    if response.status_code == 200:
        return True, True, response.json()["access_token"]
    else:
        return False, False, None


def register_face(face_id, name, image_paths, token, ip):
    ip = "http://127.0.0.1:8000"
    url = ip + "/faces"
    headers = {"Authorization": "Bearer " + token}
    base64_images = []
    for path in image_paths:
        base64_images.append(cv2_to_base64(cv2.imread(path)))

    data = {
        "face_id": face_id,
        "name": name,
        "image1": base64_images[0],
        "image2": base64_images[1],
        "image3": base64_images[2],
    }
    response = requests.post(url, data=data, headers=headers)
    print(response.json())
    if response.status_code == 200:
        return True
    else:
        return False


def creat_folder(folder):
    if not os.path.exists(folder):
        os.makedirs(folder)


def message(title, message):
    msg = QMessageBox()
    msg.setWindowTitle(title)
    msg.setText(message)
    if title == "Thất bại":
        msg.setStyleSheet(
            """
                QMessageBox {
                    background-color: #FFCCCC;
                    color: #FF0000;
                    font-size: 12px;
                    font-family: Arial, sans-serif;
                }
                
                QMessageBox QLabel {
                    color: #FF0000;
                }
                
                QMessageBox QPushButton {
                    background-color: #FF6666;
                    color: #FFFFFF;
                    padding: 5px 10px;
                    border: none;
                }
                
                QMessageBox QPushButton:hover {
                    background-color: #FF5555;
                }
            """
        )
    elif title == "Thành công":
        msg.setStyleSheet(
            """
                QMessageBox {
                    background-color: #CCFFCC;
                    color: #006600;
                    font-size: 12px;
                    font-family: Arial, sans-serif;
                }
                
                QMessageBox QLabel {
                    color: #006600;
                }
                
                QMessageBox QPushButton {
                    background-color: #66FF66;
                    color: #FFFFFF;
                    padding: 5px 10px;
                    border: none;
                }
                
                QMessageBox QPushButton:hover {
                    background-color: #55FF55;
                }
            """
        )

    msg.exec_()


def hide_text_button(menu_btns):
    for btn in menu_btns:
        btn.setText("")


def show_text_button(menu_btns, menu_names):
    for i in range(len(menu_btns)):
        menu_btns[i].setText(menu_names[i])


def reset_menu(menu_frames):
    default_style = """QPushButton {\n	border: none;\n	background-color: rgba(0,0,0,0);\n}\nQPushButton:hover {\n	background-color: rgb(91,90,90);\n}\nQPushButton:pressed {	\n	background-color: rgba(0,0,0,0);\n}"""
    for frame in menu_frames:
        frame.setEnabled(True)
        frame.setStyleSheet(default_style)


def change_frame_style(qframe):
    qframe.setStyleSheet(
        """
                QFrame {
                    background-color: #66FF66;
                    color: #66FF66;
                  
                    border: none;
                }
                """
    )


def open_files_dialog():
    options = QFileDialog.Options()
    options |= QFileDialog.DontUseNativeDialog
    files, _ = QFileDialog.getOpenFileNames(
        None,
        "Open Files",
        "",
        "Images (*.png *.xpm *.jpg, *.jpeg);;All Files (*)",
        options=options,
    )
    return files
