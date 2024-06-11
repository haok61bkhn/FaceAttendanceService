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
)
from functions import (
    LoginUI,
    FaceRegisterUI,
    Page1,
)
from PyQt5 import QtCore
from PyQt5.QtCore import QPropertyAnimation
from PyQt5.QtWidgets import QAction
from database.login_db import LoginDB

DATA_DIR = "DATA"


class Ui(QtWidgets.QMainWindow):
    def __init__(self):
        super(Ui, self).__init__()
        uic.loadUi("ui/main.ui", self)
        self.menu_frames = [
            self.frame_login,
            self.frame_page_1,
            self.frame_face_register,
        ]
        self.data_dir = DATA_DIR
        self.menu_btns = [self.bn_login, self.bn_page_1, self.bn_face_register]
        self.menu_names = ["Login", "Page 1", "Page 2"]
        self.show()
        self.first_init()

    def init_folder(self):
        create_folder(self.data_dir)

    def init_db(self):
        self.login_db = LoginDB(f"{self.data_dir}/login.db")

    def create_event(self):
        self.bn_login.clicked.connect(self.show_login)
        self.bn_page_1.clicked.connect(self.show_page_1)
        self.bn_face_register.clicked.connect(self.show_face_register)
        self.toodle.clicked.connect(self.toodle_menu)
        self.page_login_ui.message_signal.connect(message)
        self.page_login_ui.login_success_signal.connect(self.login_success)
        self.page_login_ui.logout_success_signal.connect(self.logout_success)
        self.face_register_ui.message_signal.connect(message)

    def first_init(self):
        self.status = True
        self.init_folder()
        self.init_db()
        self.init_ui()
        login_infor = self.login_db.get_login_infor()
        if login_infor:
            login_infor = login_infor[0]
            self.bt_username.setText(login_infor[2])
            self.bt_password.setText(login_infor[3])
            self.bt_ip.setText(login_infor[1])
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

    def init_ui(self):
        self.cb_remember_login.setChecked(True)
        self.page_login_ui = LoginUI(self.page_login, self)
        self.page_1_ui = Page1(self.page_1, self)
        self.face_register_ui = FaceRegisterUI(self.page_face_register, self)

    def login_success(self):
        for fr in self.menu_frames:
            fr.show()
        if self.cb_remember_login.isChecked():
            self.login_db.save_login_infor(
                self.bt_ip.text(), self.bt_username.text(), self.bt_password.text()
            )
        else:
            self.login_db.delete_login_infor()

    def logout_success(self):
        for fr in self.menu_frames:
            fr.hide()
        self.frame_login.show()
        self.show_login()

    def show_login(self):
        if self.current_index == 0:
            return
        self.clear_page()
        self.current_index = 0
        reset_menu(self.menu_frames)
        change_frame_style(self.frame_login)
        self.frame_login.setEnabled(False)
        self.stackedWidget.setCurrentWidget(self.page_login)

    def show_page_1(self):
        if self.current_index == 1:
            return
        self.clear_page()
        self.current_index = 1
        reset_menu(self.menu_frames)
        change_frame_style(self.frame_page_1)
        self.page_1_ui.init()
        self.frame_page_1.setEnabled(False)
        self.stackedWidget.setCurrentWidget(self.page_1)

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
        event.accept()

    def clear_page(self):
        if self.current_index == 1:
            self.page_1_ui.clear()
        elif self.current_index == 2:
            self.face_register_ui.clear()


if __name__ == "__main__":
    app = QtWidgets.QApplication(sys.argv)
    window = Ui()
    app.exec_()
