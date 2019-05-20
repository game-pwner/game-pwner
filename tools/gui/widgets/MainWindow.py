#!/usr/bin/env python

import pwner

from PyQt5.QtCore import *
from PyQt5.QtWidgets import *
from widgets.Ui_MainWindow import Ui_MainWindow

from dialogs.About import About
from dialogs.AttachProcDialog import AttachProcDialog
from widgets.Dashboard import Dashboard
from widgets.PointerScanner import PointerScanner


class MainWindow(QMainWindow, Ui_MainWindow):
    def __init__(self, parent=None):
        super(MainWindow, self).__init__(parent)

        # Set up the user interface from Designer.
        self.setupUi(self)

        # Windows
        self.window_about = About(self)
        self.window_pid = AttachProcDialog(self)

        # DockWidget
        self.dock1 = Dashboard(self)
        self.dock2 = PointerScanner(self)

        self.addDockWidget(Qt.TopDockWidgetArea, self.dock1)
        self.addDockWidget(Qt.TopDockWidgetArea, self.dock2)
        self.tabifyDockWidget(self.dock1, self.dock2)

    @pyqtSlot()
    def onActionPID(self):
        if self.window_pid.isHidden():
            self.window_pid.show()

    @pyqtSlot()
    def onActionCRIU(self):
        print("not implemented")

    @pyqtSlot()
    def onActionMemoryMaps(self, b):
        print("onActionMemoryMaps, b:", b)
        pass

    @pyqtSlot()
    def onActionMemoryView(self):
        pass

    @pyqtSlot()
    def onActionValueScanner(self):
        pass

    @pyqtSlot()
    def onActionPointerScanner(self):
        pass

    @pyqtSlot()
    def onActionCheats(self):
        pass

    @pyqtSlot()
    def onActionReset(self):
        self.dock1.show()
        self.dock2.show()
        self.tabifyDockWidget(self.dock1, self.dock2)

    @pyqtSlot()
    def onActionPreferences(self):
        pass

    @pyqtSlot()
    def onActionHelp(self):
        pass

    @pyqtSlot()
    def onActionCurrentHelp(self):
        pass

    @pyqtSlot()
    def onActionAbout(self):
        if self.window_about.isHidden():
            self.window_about.show()
