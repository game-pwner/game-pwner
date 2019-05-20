#!/usr/bin/env python

# https://www.riverbankcomputing.com/static/Docs/PyQt5/designer.html
from PyQt5.QtCore import *
from PyQt5.QtWidgets import *
from widgets.Ui_MainWindow import Ui_MainWindow

from dialogs.About import About
from widgets.Dashboard import Dashboard
from widgets.PointerScanner import PointerScanner


class MainWindow(QMainWindow, Ui_MainWindow):
    def __init__(self, parent=None):
        super(MainWindow, self).__init__(parent)

        # Set up the user interface from Designer.
        self.setupUi(self)

        # Windows
        self.window_about = About(self)

        # DockWidget
        self.dock1 = Dashboard(self)
        self.dock2 = PointerScanner(self)

        self.addDockWidget(Qt.TopDockWidgetArea, self.dock1)
        self.addDockWidget(Qt.TopDockWidgetArea, self.dock2)
        self.tabifyDockWidget(self.dock1, self.dock2)

    @pyqtSlot()
    def onActionAbout(self):
        if self.window_about.isHidden():
            self.window_about.show()

    @pyqtSlot()
    def onActionReset(self):
        self.dock1.show()
        self.dock2.show()
        self.tabifyDockWidget(self.dock1, self.dock2)
