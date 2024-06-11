from PyQt5 import QtWidgets
from .tools import open_files_dialog, register_face
from PyQt5.QtCore import pyqtSignal as Signal
from PyQt5.QtGui import QPixmap, QImage


class FaceRegisterUI(QtWidgets.QTabWidget):
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
        self.ui.bn_open_images.clicked.connect(self.open_images)
        self.ui.bn_register.clicked.connect(self.regist_event)

    def first_init(self):
        self.register_images = [
            self.ui.lb_image_1,
            self.ui.lb_image_2,
            self.ui.lb_image_3,
            self.ui.lb_image_4,
        ]
        self.register_frames = [
            self.ui.frame_image_1,
            self.ui.frame_image_2,
            self.ui.frame_image_3,
            self.ui.frame_image_4,
        ]

        for img, f in zip(self.register_images, self.register_frames):
            img.setScaledContents(True)

        self.create_event()

    def init(self):
        self.clear()

    def clear(self):
        self.clear_register_text()
        self.clear_register_images()

    def clear_register_images(self):
        self.file_paths = []
        self.ui.lb_image_1.clear()
        self.ui.lb_image_2.clear()
        self.ui.lb_image_3.clear()
        self.ui.lb_image_4.clear()

    def clear_register_text(self):
        self.ui.bt_name.clear()
        self.ui.bt_face_id.clear()

    def setPixmap(self, path, size, label):
        q_img = QImage(path)
        label.setPixmap(QPixmap(q_img).scaled(size[0], size[1]))

    def open_images(self):
        self.clear_register_images()
        file_paths = open_files_dialog()
        if len(file_paths) == 0:
            return
        if len(file_paths) > 4:
            self.message_signal.emit("Thất bại", "Tối đa 4 ảnh")
            return
        size_x = self.ui.frame_register_image.width() // 4
        size_y = self.ui.frame_register_image.height() // 4
        size = (size_x - 6, size_y)
        unknow_path = "ui/images/unknow.png"
        self.file_paths = file_paths.copy()
        for i in range(4 - len(file_paths)):
            file_paths.append(unknow_path)
        for i, path in enumerate(file_paths):
            if i == 0:
                self.setPixmap(path, size, self.ui.lb_image_1)
            elif i == 1:
                self.setPixmap(path, size, self.ui.lb_image_2)
            elif i == 2:
                self.setPixmap(path, size, self.ui.lb_image_3)
            elif i == 3:
                self.setPixmap(path, size, self.ui.lb_image_4)

    def regist_event(self):
        name = self.ui.bt_name.text()
        face_id = self.ui.bt_face_id.text()
        image_paths = self.file_paths
        if name == "" or face_id == "" or len(image_paths) == 0:
            self.message_signal.emit("Thất bại", "Vui lòng nhập đầy đủ thông tin")
            return
        else:
            status, message = register_face(
                face_id, name, image_paths, self.ui.token, self.ui.ip
            )
            if not status:
                self.message_signal.emit("Thất bại", message)
            else:
                self.message_signal.emit("Thành công", "Đăng ký thành công")
                self.clear()
