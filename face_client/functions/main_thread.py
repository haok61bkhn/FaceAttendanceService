from PyQt5.QtCore import QThread
from PyQt5.QtWidgets import QVBoxLayout
from .face_list_ui import FaceListUI
from .tools import base64_to_cv2
from PyQt5.QtCore import pyqtSignal as Signal
import numpy as np

import time


class MainThread(QThread):
    add_face_item_signal = Signal(np.ndarray, str, float)

    def __init__(self, parent, ui, face_ui, face_queue, status):
        QThread.__init__(self, parent)
        self.parent = parent
        self.ui = ui
        self.face_ui = face_ui
        self.status = status
        self.face_queue = face_queue
        self.first_init()

    def first_init(self):
        layout = self.parent.layout()
        layout.addWidget(self.face_ui)
        layout.setContentsMargins(0, 0, 0, 0)
        layout.setSpacing(0)

    def creat_event(self):
        # TODO
        pass

    def init(self):
        # TODO
        pass

    def clear(self):
        # TODO
        pass

    def add_face_item(self, face):
        # base64_face, name, score = face
        # image_face = base64_to_cv2(base64_face)
        image_face, name, score = face
        self.add_face_item_signal.emit(image_face, name, score)

    def run(self):

        while self.status:
            if not self.face_queue.empty():
                face = self.face_queue.get()
                self.add_face_item(face)
            time.sleep(0.1)
