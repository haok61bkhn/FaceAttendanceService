from PyQt5 import QtWidgets
from .tools import open_files_dialog
from PyQt5.QtCore import pyqtSignal as Signal
from PyQt5.QtGui import QPixmap as Pixmap


class FaceRegisterUI(QtWidgets.QTabWidget):
    show_message_signal = Signal(str)

    def __init__(
        self,
        parent,
        ui,
    ):
        super().__init__(parent)
        self.ui = ui
        parent.hide()
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
            size = f.size()
            img.setFixedSize(size.width(), size.height())

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

    def open_images(self):
        self.clear_register_images()
        file_paths = open_files_dialog()
        if len(file_paths) == 0:
            return
        if len(file_paths) > 4:
            self.show_message_signal.emit("Tối đa 4 ảnh")
            return
        for i, path in enumerate(file_paths):
            if i == 0:
                self.ui.lb_image_1.setPixmap(Pixmap(path))
            elif i == 1:
                self.ui.lb_image_2.setPixmap(Pixmap(path))
            elif i == 2:
                self.ui.lb_image_3.setPixmap(Pixmap(path))
            elif i == 3:
                self.ui.lb_image_4.setPixmap(Pixmap(path))
        self.file_paths = file_paths

    def regist_event(self):
        name = self.ui.bt_name.text()
        face_id = self.ui.bt_face_id.text()
        image_paths = self.file_paths
        if name == "" or face_id == "" or len(image_paths) == 0:
            self.show_message_signal.emit("Vui lòng nhập đầy đủ thông tin")
            return
        else:
            # register face
            pass
