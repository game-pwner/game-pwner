#!/usr/bin/env python

from PyQt5.QtWidgets import QMainWindow, QApplication, QDialog, QWidget, QDockWidget
from widgets.Ui_Dashboard import Ui_Dashboard


class Dashboard(QDockWidget, Ui_Dashboard):
    def __init__(self, parent=None):
        super(Dashboard, self).__init__(parent)

        # Set up the user interface from Designer.
        self.setupUi(self)
