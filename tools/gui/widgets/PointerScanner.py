#!/usr/bin/env python

from PyQt5.QtWidgets import QMainWindow, QApplication, QDialog, QWidget, QDockWidget
from widgets.Ui_PointerScanner import Ui_PointerScanner


class PointerScanner(QDockWidget, Ui_PointerScanner):
    def __init__(self, parent=None):
        super(PointerScanner, self).__init__(parent)

        # Set up the user interface from Designer.
        self.setupUi(self)
