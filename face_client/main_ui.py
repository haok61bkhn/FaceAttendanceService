import site

site.USER_BASE = ""
from PyQt5 import QtWidgets, uic
import sys
from functions.tools import (
    change_frame_style,
    reset_menu,
    hide_text_button,
    show_text_button,
    message,
    create_folder,
    set_hook_url,
)
from functions import LoginUI, FaceRegisterUI, MainThread, FaceListUI, CameraUI
from PyQt5 import QtCore
from PyQt5.QtCore import QPropertyAnimation
from PyQt5.QtWidgets import QAction
from database.login_db import LoginDB
import cv2
import threading


DATA_DIR = "DATA"


class MainUi(QtWidgets.QMainWindow):
    def __init__(self, face_queue):
        super(MainUi, self).__init__()
        uic.loadUi("ui/main.ui", self)
        self.menu_frames = [
            self.frame_login,
            self.frame_main,
            self.frame_face_register,
            self.frame_camera,
        ]
        self.face_queue = face_queue
        self.status = True
        self.data_dir = DATA_DIR
        self.menu_btns = [
            self.bn_login,
            self.bn_main,
            self.bn_face_register,
            self.bn_camera,
        ]
        self.menu_names = ["Đăng nhập", "Điểm danh", "Đăng ký khuôn mặt", "Camera"]
        self.show()
        self.first_init()

    def init_folder(self):
        create_folder(self.data_dir)

    def init_db(self):
        self.login_db = LoginDB(f"{self.data_dir}/login.db")

    def create_event(self):
        self.bn_login.clicked.connect(self.show_login)
        self.bn_main.clicked.connect(self.show_page_main)
        self.bn_face_register.clicked.connect(self.show_face_register)
        self.bn_camera.clicked.connect(self.show_camera)
        self.toodle.clicked.connect(self.toodle_menu)
        self.bn_set_hook.clicked.connect(self.set_hook)
        self.page_login_ui.message_signal.connect(message)
        self.page_login_ui.login_success_signal.connect(self.login_success)
        self.page_login_ui.logout_success_signal.connect(self.logout_success)
        self.face_register_ui.message_signal.connect(message)
        self.main_th.add_face_item_signal.connect(self.add_face_item)
        self.camera_ui.message_signal.connect(message)

    def first_init(self):
        self.status = True
        self.ip = ""
        self.token = ""
        self.init_folder()
        self.init_db()
        self.init_ui()
        login_infor = self.login_db.get_login_infor()
        if login_infor:
            login_infor = login_infor[0]
            self.bt_username.setText(login_infor[2])
            self.bt_password.setText(login_infor[3])
            self.bt_ip.setText(login_infor[1])
            self.bt_hook_url.setText(login_infor[4])
        self.stackedWidget.setCurrentWidget(self.page_login)
        self.current_index = 0
        self.show_login()
        for fr in self.menu_frames:
            fr.hide()
        self.frame_login.show()
        hide_text_button(self.menu_btns)
        self.create_event()

        self.showMaximized()
        quit = QAction("Quit", self)
        quit.triggered.connect(self.closeEvent)

    def push_sample_thread(self):
        image = cv2.imread("ui/images/unknow.png")
        name = "test"
        score = 0.85
        for i in range(25):
            self.face_queue.put((image, name, score))
            threading.Event().wait(2)

    def init_ui(self):
        self.cb_remember_login.setChecked(True)
        self.face_ui = FaceListUI()
        self.page_login_ui = LoginUI(self.page_login, self)
        self.main_th = MainThread(
            self.page_main, self, self.face_ui, self.face_queue, self.status
        )
        self.main_th.start()
        self.face_register_ui = FaceRegisterUI(self.page_face_register, self)
        self.camera_ui = CameraUI(self.page_camera, self)

    def login_success(self):
        for fr in self.menu_frames:
            fr.show()
        if self.cb_remember_login.isChecked():
            self.login_db.save_login_infor(
                self.bt_ip.text(), self.bt_username.text(), self.bt_password.text()
            )
            hook_url = self.bt_hook_url.text()
            if hook_url:
                self.login_db.insert_hook_url(hook_url)
        else:
            self.login_db.delete_login_infor()

    def logout_success(self):
        for fr in self.menu_frames:
            fr.hide()
        self.frame_login.show()
        self.show_login()

    def set_hook(self):
        hook_url = self.bt_hook_url.text()
        status, message_str = set_hook_url(hook_url, self.ip, self.token)
        if status:
            self.login_db.insert_hook_url(hook_url)
            message("Thành công", "Thiết lập hook thành công")
        else:
            message("Thất bại", message_str)

    def show_login(self):
        if self.current_index == 0:
            return
        self.clear_page()
        self.current_index = 0
        reset_menu(self.menu_frames)
        change_frame_style(self.frame_login)
        self.frame_login.setEnabled(False)
        self.stackedWidget.setCurrentWidget(self.page_login)

    def show_page_main(self):
        if self.current_index == 1:
            return
        self.clear_page()
        self.current_index = 1
        reset_menu(self.menu_frames)
        change_frame_style(self.frame_main)
        self.main_th.init()
        self.frame_main.setEnabled(False)
        self.stackedWidget.setCurrentWidget(self.page_main)

    def show_face_register(self):
        if self.current_index == 2:
            return
        self.clear_page()
        self.current_index = 2
        reset_menu(self.menu_frames)
        change_frame_style(self.frame_face_register)
        self.face_register_ui.init()
        self.frame_face_register.setEnabled(False)
        self.stackedWidget.setCurrentWidget(self.page_face_register)

    def show_camera(self):
        if self.current_index == 3:
            return
        self.clear_page()
        self.current_index = 3
        reset_menu(self.menu_frames)
        change_frame_style(self.frame_camera)
        self.camera_ui.init()
        self.frame_camera.setEnabled(False)
        self.stackedWidget.setCurrentWidget(self.page_camera)

    def add_face_item(self, face_image, name, score):
        self.face_ui.add_face_item(face_image, name, score)

    def toodle_menu(self):
        max_width = 160
        min_width = 80
        if self.frame_bottom_west.width() == max_width:
            new_width = min_width
            hide_text_button(self.menu_btns)
            for bn in self.menu_btns:
                bn.setMinimumWidth(min_width)
        else:
            show_text_button(self.menu_btns, self.menu_names)
            new_width = max_width
            for bn in self.menu_btns:
                bn.setMinimumWidth(max_width)
        self.animation = QPropertyAnimation(self.frame_bottom_west, b"minimumWidth")
        self.animation.setDuration(250)
        self.animation.setStartValue(self.frame_bottom_west.width())
        self.animation.setEndValue(new_width)
        self.animation.setEasingCurve(QtCore.QEasingCurve.InOutQuart)
        self.animation.start()

    def closeEvent(self, event):
        self.status = False
        event.accept()

    def clear_page(self):
        if self.current_index == 1:
            self.main_th.clear()
        elif self.current_index == 2:
            self.face_register_ui.clear()
        elif self.current_index == 3:
            self.camera_ui.clear()
