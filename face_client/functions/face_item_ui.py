from PyQt5.QtWidgets import QWidget, QLabel, QVBoxLayout
from PyQt5.QtGui import QPixmap, QImage
from PyQt5.QtCore import Qt
import cv2
from PyQt5.QtWidgets import QFrame


class FaceItemUI(QWidget):
    def __init__(self, image, name, score, time_str, camera_name, parent=None):
        super(FaceItemUI, self).__init__(parent)
        self.image = image
        self.name = name
        self.score = score
        self.time_str = time_str
        self.camera_name = camera_name

        self.initUI()

    def initUI(self):
        layout = QVBoxLayout(self)

        # Main frame
        frame = QFrame(self)
        frame.setStyleSheet(
            """
            QFrame {
                background-color: #ffffff;
                border-radius: 10px;
                padding: 2px;
                border: 1px solid #ccc;
            }
        """
        )
        frame_layout = QVBoxLayout(frame)

        # Display Image
        image_label = QLabel(self)
        pixmap = self.get_pixmap(self.image)
        image_label.setPixmap(pixmap)
        image_label.setFixedSize(200, 200)
        image_label.setScaledContents(True)
        frame_layout.addWidget(image_label, alignment=Qt.AlignCenter)

        # Display Name
        name_label = QLabel(self)
        name_label.setText(f"Name: {self.name}")
        name_label.setAlignment(Qt.AlignCenter)
        name_label.setStyleSheet(
            """
            QLabel {
                font-size: 16px;
                font-weight: bold;
                color: #333;
                margin-top: 2px;
            }
        """
        )
        frame_layout.addWidget(name_label)

        # Display Score
        score_label = QLabel(self)
        score_label.setText(f"Score: {self.score}")
        score_label.setAlignment(Qt.AlignCenter)
        score_label.setStyleSheet(
            """
            QLabel {
                font-size: 14px;
                color: #555;
                margin-top: 2px;
            }
        """
        )
        frame_layout.addWidget(score_label)

        # Display Time
        time_label = QLabel(self)
        time_label.setText(f"Time: {self.time_str}")
        time_label.setAlignment(Qt.AlignCenter)
        time_label.setStyleSheet(
            """
            QLabel {
                font-size: 14px;
                color: #555;
                margin-top: 2px;
            }
        """
        )
        frame_layout.addWidget(time_label)

        # Display Camera Name
        camera_label = QLabel(self)
        camera_label.setText(f"Camera: {self.camera_name}")
        camera_label.setAlignment(Qt.AlignCenter)
        camera_label.setStyleSheet(
            """
            QLabel {
                font-size: 14px;
                color: #555;
                margin-top: 2px;
            }
        """
        )
        frame_layout.addWidget(camera_label)

        layout.addWidget(frame, alignment=Qt.AlignCenter)

        self.setLayout(layout)
        self.setStyleSheet(
            """
            QWidget {
                background-color: #f0f0f0;
                border-radius: 10px;
                padding: 2px;
            }
        """
        )
        self.setFixedSize(240, 410)

    def get_pixmap(self, image):
        image = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
        height, width, channel = image.shape
        bytes_per_line = 3 * width
        qimage = QImage(image.data, width, height, bytes_per_line, QImage.Format_RGB888)
        pixmap = QPixmap.fromImage(qimage)
        return pixmap
