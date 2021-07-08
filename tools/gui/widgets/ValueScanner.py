#!/usr/bin/env python
from typing import List

from PyQt5.QtCore import pyqtSlot, QTimer, Qt
from PyQt5.QtGui import QStandardItemModel
from PyQt5.QtWidgets import QMainWindow, QApplication, QDialog, QWidget, QDockWidget
from widgets.Ui_ValueScanner import Ui_ValueScanner

import pwner
import settings


class ValueScanner(QDockWidget, Ui_ValueScanner):
    def __init__(self, parent: QWidget = None):
        super(ValueScanner, self).__init__(parent)
        self.setupUi(self)

        self.ADDRESS, self.VALUE = range(2)
        self.model = self.create_values_model(self)
        self.treeValues.setModel(self.model)

        # self.procTreeView.doubleClicked.connect(self.accept)

        # self.timer = QTimer()  # refresh_process_tree() used
        # self.timer.timeout.connect(self.refresh_values_tree)

        self.matches: pwner.VectorValues = pwner.VectorValues()
        self.isFirst: bool = True

        self.data_type: pwner.Edata_type = pwner.Edata_type.ANYNUMBER
        self.match_type: pwner.Ematch_type = pwner.Ematch_type.MATCHANY
        self.uservalue: List[pwner.Cuservalue] = []

    def create_values_model(self, parent: QWidget = None) -> QStandardItemModel:
        model = QStandardItemModel(0, 2, parent)
        model.setHeaderData(self.ADDRESS, Qt.Horizontal, "Address")
        model.setHeaderData(self.VALUE, Qt.Horizontal, "Value")
        return model

    @pyqtSlot()
    def refresh_values_tree(self):
        print("refresh_values_tree not fully implemented")



    @pyqtSlot()
    def onActionRegions(self):
        print("onActionRegions not fully implemented")

    @pyqtSlot()
    def notifyProcChange(self):
        print("notifyProcChange not fully implemented")
        settings.scan_value = pwner.ScannerSequential(settings.proc)

    @pyqtSlot()
    def onUservalueChange(self):
        print("onUservalueChange not fully implemented")
        index_scan_type = self.comboScanType.currentIndex()
        index_value_type = self.comboValueType.currentIndex()

        if False: pass
        elif index_value_type == 0:  self.data_type = pwner.Edata_type.BYTEARRAY
        elif index_value_type == 1:  self.data_type = pwner.Edata_type.STRING
        elif index_value_type == 2:  self.data_type = pwner.Edata_type.ANYNUMBER
        elif index_value_type == 3:  self.data_type = pwner.Edata_type.ANYINTEGER
        elif index_value_type == 4:  self.data_type = pwner.Edata_type.INTEGER8
        elif index_value_type == 5:  self.data_type = pwner.Edata_type.INTEGER16
        elif index_value_type == 6:  self.data_type = pwner.Edata_type.INTEGER32
        elif index_value_type == 7:  self.data_type = pwner.Edata_type.INTEGER64
        elif index_value_type == 8:  self.data_type = pwner.Edata_type.ANYFLOAT
        elif index_value_type == 9:  self.data_type = pwner.Edata_type.FLOAT32
        elif index_value_type == 10: self.data_type = pwner.Edata_type.FLOAT64
        search_for: str = self.lineValue.text()
        try:
            self.match_type, self.uservalue = pwner.string_to_uservalue(self.data_type, search_for)
        except RuntimeError as e:
            print("exception behaviour not implemented, e:", e)

    @pyqtSlot()
    def onActionScan(self):
        if self.isFirst:
            self.isFirst = False
            print("First Scan")
            settings.scan_value.scan_regions(self.matches, self.data_type, self.uservalue, self.match_type)
            print("matches:", len(self.matches))
        else:
            print("Next Scan")
            settings.scan_value.scan_recheck(self.matches, self.data_type, self.uservalue, self.match_type)
            print("matches:", len(self.matches))

    @pyqtSlot()
    def onActionReset(self):
        self.model.setRowCount(0)
        self.data_matches = pwner.VectorValues()
        self.isFirst = True

