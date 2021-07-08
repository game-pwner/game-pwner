#!/usr/bin/env python

from PyQt5.QtWidgets import QMainWindow, QApplication, QDialog, QWidget, QDockWidget
from widgets.Ui_Maps import Ui_Maps


class Maps(QDockWidget, Ui_Maps):
    def __init__(self, parent: QWidget = None):
        super(Maps, self).__init__(parent)

        # Set up the user interface from Designer.
        self.setupUi(self)

