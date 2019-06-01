#!/usr/bin/env python

from PyQt5.QtCore import *
from PyQt5.QtWidgets import *
from widgets.Ui_MainWindow import Ui_MainWindow

from dialogs.About import About
from dialogs.AttachProcDialog import AttachProcDialog
from widgets.Maps import Maps
from widgets.PointerScanner import PointerScanner
from widgets.ValueScanner import ValueScanner

from pwner import *
import pwner
import settings

DEBUG = 1


class MainWindow(QMainWindow, Ui_MainWindow):
    def __init__(self, parent: QWidget = None):
        super(MainWindow, self).__init__(parent)
        # Set up the user interface from Designer.
        self.setupUi(self)

        # Windows
        self.window_about = About(self)
        self.window_pid = AttachProcDialog(self)

        # DockWidget
        self.dock1 = Maps(self)
        self.dock2 = PointerScanner(self)
        self.dock3 = ValueScanner(self)

        self.addDockWidget(Qt.TopDockWidgetArea, self.dock1)
        self.addDockWidget(Qt.TopDockWidgetArea, self.dock2)
        self.tabifyDockWidget(self.dock1, self.dock2)
        self.tabifyDockWidget(self.dock1, self.dock3)

        if DEBUG == 1:
            settings.proc = ProcessProcfs("FAKEMEM")
            settings.scan_value = ScannerSequential(settings.proc)
            self.dock3.onActionReset()

        self.timer = QTimer()  # refresh_process_tree() used
        self.timer.timeout.connect(self.onTitleUpdate)
        self.timer.start(1000)
        self.onTitleUpdate()


    @pyqtSlot()
    def onTitleUpdate(self):
        self.setWindowTitle("[" + str(settings.proc.pid()) + "] - " + (settings.proc.cmdline().replace("\0", " ") if settings.proc.running() else "Not running") + " - pwner-gui")

    @pyqtSlot()
    def onActionPID(self):
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
        self.window_about.show()
