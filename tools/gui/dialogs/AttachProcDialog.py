#!/usr/bin/env python

from PyQt5.QtWidgets import QMainWindow, QApplication, QDialog
from dialogs.Ui_AttachProcDialog import Ui_AttachProcDialog


class AttachProcDialog(QDialog, Ui_AttachProcDialog):
    def __init__(self, globals, parent=None):
        super(AttachProcDialog, self).__init__(parent)
        print("t:", type(parent))
        self.globals = globals

        # Set up the user interface from Designer.
        self.setupUi(self)

    def accept(self) -> None:
        print("accept")
        self.hide()

    def reject(self) -> None:
        print("reject")
        self.hide()
