#!/usr/bin/env python
from PyQt5.QtCore import pyqtSlot
from PyQt5.QtWidgets import QMainWindow, QApplication, QDialog, QWidget, QDockWidget
from widgets.Ui_ValueScanner import Ui_ValueScanner

import pwner
import settings


class ValueScanner(QDockWidget, Ui_ValueScanner):
    def __init__(self, parent: QWidget = None):
        super(ValueScanner, self).__init__(parent)

        self.vector_values: list[pwner.value_t] = []
        self.isFirst: bool = True

        # Set up the user interface from Designer.
        self.setupUi(self)

    @pyqtSlot()
    def onActionRegions(self):
        print("onActionRegions not implemented")

    @pyqtSlot()
    def onActionScan(self):
        if self.isFirst:
            self.isFirst = False
            # scanner.scan_regions(matches_first, data_type, uservalue, match_type);
            """ scan_regions(self: pwner.ScannerSequential, arg0: List[pwner.value_t], arg1: pwner.Edata_type, arg2: pwner.Cuservalue, arg3: pwner.Ematch_type) -> bool """
            self.comboScanType
            self.comboValueType
            self.lineValue
            settings.scan_value.scan_regions(self.vector_values)
        else:
            settings.scan_value.scan_recheck(self.vector_values)

    @pyqtSlot()
    def onActionReset(self):
        self.model.setRowCount(0)
        self.isFirst = True

