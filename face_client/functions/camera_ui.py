from PyQt5 import QtWidgets
from .tools import (
    get_cameras,
    insert_camera,
    remove_camera,
)
from PyQt5.QtCore import pyqtSignal as Signal
from PyQt5.QtGui import QPixmap, QImage
from PyQt5.QtWidgets import QTableWidget, QTableWidgetItem
from PyQt5.QtGui import QStandardItem, QStandardItemModel


class CameraUI(QtWidgets.QTabWidget):
    message_signal = Signal(str, str)

    def __init__(
        self,
        parent,
        ui,
    ):
        super().__init__(parent)
        self.ui = ui
        self.hide()
        self.first_init()

    def create_event(self):
        self.ui.bn_remove_camera.clicked.connect(self.remove_camera_event)
        self.ui.bn_add_camera.clicked.connect(self.regist_event)
        self.ui.bn_get_camera.clicked.connect(self.get_camera_list)
        self.ui.tb_camera.clicked.connect(self.select_row_event)

    def first_init(self):
        self.titles = ["Camera ID", "Name", "Active", "FPS"]
        self.keys = ["camera_id", "camera_name", "active", "fps"]
        self.model = QStandardItemModel()
        self.model.setHorizontalHeaderLabels(self.titles)
        self.ui.tb_camera.setModel(self.model)
        self.ui.tb_camera.horizontalHeader().setStretchLastSection(True)
        self.ui.tb_camera.horizontalHeader().setSectionResizeMode(
            QtWidgets.QHeaderView.Stretch
        )
        self.ui.tb_camera.setSelectionBehavior(QTableWidget.SelectRows)
        self.create_event()

    def init(self):
        self.clear()
        self.get_camera_list()
        self.ui.bn_remove_camera.setEnabled(False)

    def clear(self):
        self.clear_register_text()

    def clear_register_text(self):
        self.ui.bt_camera_name.clear()
        self.ui.bt_camera_ip.clear()

    def regist_event(self):
        name = self.ui.bt_camera_name.text()
        ip = self.ui.bt_camera_ip.text()
        if name == "" or ip == "":
            self.message_signal.emit("Thất bại", "Vui lòng nhập đầy đủ thông tin")
            return
        else:
            status, message = insert_camera(ip, name, self.ui.ip, self.ui.token)
            if not status:
                self.message_signal.emit("Thất bại", message)
            else:
                self.message_signal.emit("Thành công", "Đăng ký thành công")
                self.get_camera_list()
                self.clear_register_text()

    def get_camera_list(self):
        self.cameras = get_cameras(self.ui.token, self.ui.ip)
        self.model.clear()
        self.model.setHorizontalHeaderLabels(self.titles)
        for camera in self.cameras:
            row = []
            for key in self.keys:
                if key == "fps":
                    item = QStandardItem(str(int(float(camera[key]))))
                else:
                    item = QStandardItem(str(camera[key]))
                row.append(item)
            self.model.appendRow(row)

    def remove_camera_event(self):
        selected = self.ui.tb_camera.selectedIndexes()
        if len(selected) == 0:
            return
        row = selected[0].row()
        camera_id = self.cameras[row]["camera_id"]
        status, message = remove_camera(
            camera_id,
            self.ui.ip,
            self.ui.token,
        )
        if not status:
            self.message_signal.emit("Thất bại", message)
        else:
            self.message_signal.emit("Thành công", message)
            self.get_camera_list()
            self.clear()
            self.ui.bn_remove_camera.setEnabled(False)

    def select_row_event(self):
        selected = self.ui.tb_camera.selectedIndexes()
        if len(selected) == 0:
            return
        self.ui.bn_remove_camera.setEnabled(True)
