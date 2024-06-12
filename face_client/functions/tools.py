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
    try:
        url = ip + "/login"
        response = requests.post(
            url,
            data={"username": username, "password": password},
            headers={"Content-Type": "application/x-www-form-urlencoded"},
        )
        if response.status_code == 200:
            return True, True, response.json()["access_token"]
        else:
            return False, False, None
    except Exception as e:
        print(e)
        return False, False, None


def set_hook_url(hook_url, ip, token):
    try:
        url = ip + "/hook"
        headers = {"Authorization": "Bearer " + token}
        response = requests.post(url, json={"url": hook_url}, headers=headers)
        if response.status_code == 200:
            return True, "Thành công"
        else:
            return False, "Lỗi server"
    except Exception as e:
        print(e)
        return False, str(e)


def remove_face(face_id, token, ip):
    try:
        url = ip + "/faces/" + face_id
        headers = {"Authorization": "Bearer " + token}
        response = requests.delete(url, headers=headers)
        if response.status_code == 200:
            return True, "Xóa thành công"
        else:
            return False, "Lỗi server"
    except Exception as e:
        print(e)
        return False, str(e)


def register_face(face_id, name, image_paths, token, ip):
    try:
        url = ip + "/faces"
        headers = {"Authorization": "Bearer " + token}
        base64_images = []
        for path in image_paths:
            base64_images.append(cv2_to_base64(cv2.imread(path)))
        if len(base64_images) < 4:
            for i in range(4 - len(base64_images)):
                base64_images.append("")

        data = {
            "faceId": face_id,
            "name": name,
            "image1": base64_images[0],
            "image2": base64_images[1],
            "image3": base64_images[2],
            "image4": base64_images[3],
        }
        response = requests.post(url, json=data, headers=headers)
        if response.status_code == 200:
            json_data = response.json()
            return json_data["status"], json_data["message"]
        else:
            return False, "Lỗi server"
    except Exception as e:
        print(e)
        return False, str(e)


def get_faces(token, ip):
    try:
        url = ip + "/faces"
        headers = {"Authorization": "Bearer " + token}
        response = requests.get(url, headers=headers)
        if response.status_code == 200:
            return response.json()["data"]["faces"]
        else:
            return []
    except Exception as e:
        print(e)
        return []


def get_cameras(token, ip):
    try:
        url = ip + "/camera"
        headers = {"Authorization": "Bearer " + token}
        response = requests.get(url, headers=headers)
        if response.status_code == 200:
            return response.json()["data"]["cameras"]
        else:
            return []
    except Exception as e:
        print(e)
        return []


def insert_camera(camera_url, camera_name, ip, token):
    try:
        url = ip + "/camera"
        headers = {"Authorization": "Bearer " + token}
        data = {"url": camera_url, "name": camera_name}
        response = requests.post(url, json=data, headers=headers)
        if response.status_code == 200:
            return True, "Thêm camera thành công"
        else:
            return False, "Lỗi server"
    except Exception as e:
        print(e)
        return False, str(e)


def remove_camera(camera_id, ip, token):
    try:
        url = ip + "/camera/" + camera_id
        headers = {"Authorization": "Bearer " + token}
        response = requests.delete(url, headers=headers)
        if response.status_code == 200:
            return True, "Xóa camera thành công"
        else:
            return False, "Lỗi server"
    except Exception as e:
        print(e)
        return False, str(e)


def create_folder(folder):
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
