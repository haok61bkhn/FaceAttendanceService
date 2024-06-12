import sys
from PyQt5.QtWidgets import (
    QWidget,
    QVBoxLayout,
    QScrollArea,
    QApplication,
    QGridLayout,
)
import cv2
from .face_item_ui import FaceItemUI


class FaceListUI(QWidget):
    def __init__(self, parent=None):
        super(FaceListUI, self).__init__(parent)

        self.max_columns = 5
        self.max_rows = 20
        self.initUI()

    def initUI(self):
        main_layout = QVBoxLayout()
        self.scroll_area = QScrollArea(self)
        self.scroll_area.setWidgetResizable(True)
        self.container_widget = QWidget()
        self.container_layout = QGridLayout(self.container_widget)
        self.scroll_area.setWidget(self.container_widget)
        main_layout.addWidget(self.scroll_area)
        self.setLayout(main_layout)
        self.setStyleSheet("background-color: #f7f7f7; padding: 10px;")

    def add_face_item(self, image, name, score, time_str, camera_name):
        current_count = self.container_layout.count()
        max_count = self.max_columns * self.max_rows
        if current_count >= max_count:
            item_to_remove = self.container_layout.itemAt(max_count - 1)
            if item_to_remove:
                widget_to_remove = item_to_remove.widget()
                if widget_to_remove:
                    self.container_layout.removeWidget(widget_to_remove)
                    widget_to_remove.deleteLater()
        for i in reversed(range(min(current_count, max_count - 1))):
            item = self.container_layout.itemAt(i)
            if item:
                widget = item.widget()
                if widget:
                    row = (i + 1) // self.max_columns
                    col = (i + 1) % self.max_columns
                    self.container_layout.removeWidget(widget)
                    self.container_layout.addWidget(widget, row, col)
        face_item = FaceItemUI(image, name, score, time_str, camera_name)
        self.container_layout.addWidget(face_item, 0, 0)


if __name__ == "__main__":
    app = QApplication(sys.argv)

    face_list = FaceListUI()

    # Example usage: adding a face item programmatically
    example_image = cv2.imread(
        "ui/images/unknow.png"
    )  # Replace with actual image path or data
    if example_image is not None:
        for i in range(15):
            face_list.add_face_item(example_image, "Jane Doe", 98)

    face_list.add_face_item(example_image, "Jane Doe", 98)

    face_list.show()
    sys.exit(app.exec_())
