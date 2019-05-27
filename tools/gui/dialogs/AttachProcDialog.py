#!/usr/bin/env python
from PyQt5.QtCore import pyqtSlot, Qt, QTimer, QItemSelectionModel, QModelIndex
from PyQt5.QtGui import QStandardItemModel, QShowEvent, QHideEvent
from PyQt5.QtWidgets import QMainWindow, QApplication, QDialog, QWidget
from dialogs.Ui_AttachProcDialog import Ui_AttachProcDialog
import pwner
import settings


class AttachProcDialog(QDialog, Ui_AttachProcDialog):
    def __init__(self, parent: QWidget = None):
        super(AttachProcDialog, self).__init__(parent)
        self.setupUi(self)

        self.PID, self.USER, self.COMMAND = range(3)
        self.model = self.create_proc_model(self)
        self.procTreeView.setModel(self.model)

        self.procTreeView.doubleClicked.connect(self.accept)

        self.timer = QTimer()  # refresh_process_tree() used
        self.timer.timeout.connect(self.refresh_process_tree)

    def showEvent(self, a0: QShowEvent) -> None:
        self.refresh_process_tree()
        self.timer.start(1000)

    def hideEvent(self, a0: QHideEvent) -> None:
        self.timer.stop()

    @pyqtSlot()
    def refresh_process_tree(self) -> None:
        # fixme[low]: refresh_process_tree to rework: refresh processes like KSysGuard
        # todo[low]: evaluate privileges, change PID, change cmdline
        # fixme[med]: reword CProcess to ???
        # todo[low]: show window title column
        # todo[low]: externals.hh to another module
        selected_rows = self.procTreeView.selectionModel().selectedRows()
        # selected_pids = [self.model.data(self.model.index(row.row(), self.PID)) for row in selected_rows]

        self.model.setRowCount(0)
        ps: list[pwner.CProcess] = pwner.getProcesses()
        for process in ps:
            if self.filterLineEdit.text() in process.pid or self.filterLineEdit.text() in process.command:
                self.add_proc(process)
        for sel_row in selected_rows:
            self.procTreeView.selectionModel().select(sel_row, QItemSelectionModel.Select | QItemSelectionModel.Rows)

    def create_proc_model(self, parent: QWidget = None) -> QStandardItemModel:
        model = QStandardItemModel(0, 3, parent)
        model.setHeaderData(self.PID, Qt.Horizontal, "PID")
        model.setHeaderData(self.USER, Qt.Horizontal, "User")
        model.setHeaderData(self.COMMAND, Qt.Horizontal, "Command")
        return model

    def add_proc(self, cp: pwner.CProcess) -> None:
        i = self.model.rowCount()
        self.model.insertRow(i)
        self.model.setData(self.model.index(i, self.PID), cp.pid)
        self.model.setData(self.model.index(i, self.USER), cp.user)
        self.model.setData(self.model.index(i, self.COMMAND), cp.command)

    def accept(self) -> None:
        index = self.procTreeView.selectedIndexes()
        if index:
            pid = int(self.model.data(self.model.index(index[0].row(), self.PID)))
            settings.proc = pwner.ProcessProcfs(pid)
            settings.scan_value = pwner.ScannerSequential(settings.proc)
            # TODO[critical]: MainWindow.py: notify everyone that PID updated
        self.hide()

    def reject(self) -> None:
        self.hide()
