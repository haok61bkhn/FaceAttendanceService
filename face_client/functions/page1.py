from PyQt5 import QtWidgets
from PyQt5.QtGui import QStandardItemModel, QStandardItem
from PyQt5.QtCore import Qt


class Page1(QtWidgets.QTabWidget):
    def __init__(self, parent, ui):
        super().__init__(parent)
        self.parent = parent
        self.ui = ui
        self.hide()

    def first_init(self):
        # TODO
        self.creat_event()
        pass

    def creat_event(self):
        # TODO
        pass

    def init(self):
        # TODO
        pass

    def clear(self):
        # TODO
        pass
