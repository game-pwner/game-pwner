#!/usr/bin/env python

from PyQt5.QtWidgets import QMainWindow, QApplication, QDialog
from dialogs.Ui_About import Ui_About


class About(QDialog, Ui_About):
    def __init__(self, parent=None):
        super(About, self).__init__(parent)

        # Set up the user interface from Designer.
        self.setupUi(self)
