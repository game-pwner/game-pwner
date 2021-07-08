#!/usr/bin/env python

from PyQt5.QtWidgets import QMainWindow, QApplication, QDialog, QWidget
from dialogs.Ui_About import Ui_About
import pwner, settings


class About(QDialog, Ui_About):
    def __init__(self, parent: QWidget = None):
        super(About, self).__init__(parent)
        # Set up the user interface from Designer.
        self.setupUi(self)
