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

        # Create scroll area
        self.scroll_area = QScrollArea(self)
        self.scroll_area.setWidgetResizable(True)

        # Create a widget to contain all face items
        self.container_widget = QWidget()
        self.container_layout = QGridLayout(self.container_widget)

        # Set container widget to scroll area
        self.scroll_area.setWidget(self.container_widget)

        # Add scroll area to main layout
        main_layout.addWidget(self.scroll_area)

        self.setLayout(main_layout)
        self.setStyleSheet("background-color: #f7f7f7; padding: 10px;")

    def add_face_item(self, image, name, score, time_str, camera_name):
        if image is not None:
            current_count = self.container_layout.count()
            if current_count >= self.max_columns * self.max_rows:
                # Remove the last item in the grid
                item_to_remove = self.container_layout.itemAt(current_count - 1)
                if item_to_remove:
                    widget_to_remove = item_to_remove.widget()
                    if widget_to_remove:
                        self.container_layout.removeWidget(widget_to_remove)
                        widget_to_remove.deleteLater()

            # Calculate the position in the grid
            row = current_count // self.max_columns
            col = current_count % self.max_columns

            # Create and add new FaceItemUI to the grid
            face_item = FaceItemUI(image, name, score, time_str, camera_name)
            self.container_layout.addWidget(face_item, row, col)


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
