from PyQt5 import QtWidgets
from .tools import login
from PyQt5.QtCore import pyqtSignal as Signal


class LoginUI(QtWidgets.QTabWidget):
    login_success_signal = Signal()
    logout_success_signal = Signal()
    message_signal = Signal(str, str)

    def __init__(self, parent, ui):
        super().__init__(parent)
        self.is_login = False
        self.parent = parent
        self.ui = ui
        self.ui.bt_password.setEchoMode(QtWidgets.QLineEdit.Password)
        self.hide()
        self.ui.bt_login.clicked.connect(self.login)
        self.ui.bt_logout.clicked.connect(self.logout)
        self.ui.wg_logout.hide()

    def clear(self):
        if self.is_login:
            self.ui.bt_username.setText("")
            self.ui.bt_password.setText("")

    def login_success(self):
        self.ui.wg_login.hide()
        self.ui.wg_logout.show()
        self.login_success_signal.emit()

    def login(self):
        username = self.ui.bt_username.text()
        password = self.ui.bt_password.text()
        ip = self.ui.bt_ip.text()
        success, is_admin, self.ui.token = login(username, password, ip)
        if not success:
            self.message_signal.emit("Thất bại", "Sai tên đăng nhập hoặc mật khẩu")
            return
        self.ui.username = username
        self.ui.password = password
        self.ui.ip = ip
        if is_admin:

            self.message_signal.emit("Thành công", "Đăng nhập thành công admin")
            self.ui.lab_user.setText("Admin")
            self.ui.is_admin = True
            self.login_success()
        else:
            self.message_signal.emit("Thành công", "Đăng nhập thành công")
            self.ui.lab_user.setText("User")
            self.ui.is_admin = False
            self.login_success()

    def refresh_token(self):
        _, _, self.ui.token = login(self.username, self.password, self.ip)

    def logout(self):
        self.ui.wg_logout.hide()
        self.ui.wg_login.show()
        self.logout_success_signal.emit()
        self.message_signal.emit("Thành công", "Đăng xuất thành công")
        self.ui.bt_username.setEnabled(True)
        self.ui.bt_password.setEnabled(True)
