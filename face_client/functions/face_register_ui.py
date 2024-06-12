from PyQt5 import QtWidgets
from .tools import (
    open_files_dialog,
    register_face,
    get_faces,
    base64_to_cv2,
    remove_face,
    open_folder_dialog,
    get_folder_paths,
)
from PyQt5.QtCore import pyqtSignal as Signal
from PyQt5.QtGui import QPixmap, QImage
from PyQt5.QtWidgets import QTableWidget, QTableWidgetItem
from PyQt5.QtGui import QStandardItem, QStandardItemModel
import cv2
import glob


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
        self.ui.bn_clear.clicked.connect(self.clear)
        self.ui.bn_get_faces.clicked.connect(self.get_face_list)
        self.ui.tb_faces.clicked.connect(self.select_row_event)
        self.ui.bn_register_folder.clicked.connect(self.register_folder)
        self.ui.bn_remove_user.clicked.connect(self.remove_face_event)

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
        self.model = None
        for img, f in zip(self.register_images, self.register_frames):
            img.setScaledContents(True)
        self.titles = ["Face ID", "Name"]
        self.keys = ["faceId", "name"]
        self.model = QStandardItemModel()
        self.model.setHorizontalHeaderLabels(self.titles)
        self.ui.tb_faces.setModel(self.model)
        self.ui.tb_faces.horizontalHeader().setStretchLastSection(True)
        self.ui.tb_faces.horizontalHeader().setSectionResizeMode(
            QtWidgets.QHeaderView.Stretch
        )
        self.ui.tb_faces.setSelectionBehavior(QTableWidget.SelectRows)
        self.create_event()

    def init(self):
        self.clear()
        self.get_face_list()
        self.ui.bn_remove_user.setEnabled(False)

    def clear(self):
        self.clear_register_text()
        self.clear_register_images()
        self.clear_select_image()

    def clear_select_image(self):
        self.ui.lb_image_face.clear()
        self.ui.tb_faces.clearSelection()

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
                self.get_face_list()
                self.clear()

    def get_face_list(self):
        self.faces = get_faces(self.ui.token, self.ui.ip)
        self.model.clear()
        self.model.setHorizontalHeaderLabels(self.titles)
        for face in self.faces:
            row = []
            for key in self.keys:
                item = QStandardItem(str(face[key]))
                row.append(item)
            self.model.appendRow(row)

    def get_pixmap(self, image):
        image = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
        height, width, channel = image.shape
        bytes_per_line = 3 * width
        qimage = QImage(image.data, width, height, bytes_per_line, QImage.Format_RGB888)
        pixmap = QPixmap.fromImage(qimage)
        return pixmap

    def select_row_event(self):

        selected = self.ui.tb_faces.selectedIndexes()
        if len(selected) == 0:
            return
        self.ui.bn_remove_user.setEnabled(True)
        row = selected[0].row()
        image1 = self.faces[row]["image1"]
        if image1 != "":
            image1_cv = base64_to_cv2(image1)
            self.ui.lb_image_face.setScaledContents(True)
            self.ui.lb_image_face.setPixmap(self.get_pixmap(image1_cv))

    def remove_face_event(self):
        selected = self.ui.tb_faces.selectedIndexes()
        if len(selected) == 0:
            return
        row = selected[0].row()
        face_id = self.faces[row]["faceId"]
        status, message = remove_face(face_id, self.ui.token, self.ui.ip)
        if not status:
            self.message_signal.emit("Thất bại", message)
        else:
            self.message_signal.emit("Thành công", message)
            self.get_face_list()
            self.clear_select_image()
            self.ui.bn_remove_user.setEnabled(False)

    def register_face_folder(self, folder_path):
        face_id = folder_path.split("/")[-1]
        image_paths = (
            glob.glob(folder_path + "/*.jpg")
            + glob.glob(folder_path + "/*.png")
            + glob.glob(folder_path + "/*.jpeg")
        )
        if len(image_paths) > 4:
            image_paths = image_paths[:4]
        if len(image_paths) > 0:
            status, message = register_face(
                face_id, face_id, image_paths, self.ui.token, self.ui.ip
            )

    def register_folder(self):
        folder_path = open_folder_dialog()
        if folder_path == "":
            return
        else:
            folder_paths = get_folder_paths(folder_path)
            for folder_path in folder_paths:
                self.register_face_folder(folder_path)
            self.get_face_list()
            self.message_signal.emit("Thành công", "Đăng ký thành công")
