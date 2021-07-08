# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file '/home/user/github.com/game-pwner/game-pwner/tools/gui/dialogs/AttachProcDialog.ui'
#
# Created by: PyQt5 UI code generator 5.12.1
#
# WARNING! All changes made in this file will be lost!

from PyQt5 import QtCore, QtGui, QtWidgets


class Ui_AttachProcDialog(object):
    def setupUi(self, AttachProcDialog):
        AttachProcDialog.setObjectName("AttachProcDialog")
        AttachProcDialog.resize(693, 504)
        self.verticalLayout_2 = QtWidgets.QVBoxLayout(AttachProcDialog)
        self.verticalLayout_2.setContentsMargins(2, 5, 2, 2)
        self.verticalLayout_2.setSpacing(2)
        self.verticalLayout_2.setObjectName("verticalLayout_2")
        self.procTreeView = QtWidgets.QTreeView(AttachProcDialog)
        self.procTreeView.setObjectName("procTreeView")
        self.verticalLayout_2.addWidget(self.procTreeView)
        self.filterLineEdit = QtWidgets.QLineEdit(AttachProcDialog)
        self.filterLineEdit.setObjectName("filterLineEdit")
        self.verticalLayout_2.addWidget(self.filterLineEdit)
        self.buttonBox = QtWidgets.QDialogButtonBox(AttachProcDialog)
        self.buttonBox.setOrientation(QtCore.Qt.Horizontal)
        self.buttonBox.setStandardButtons(QtWidgets.QDialogButtonBox.Cancel|QtWidgets.QDialogButtonBox.Ok)
        self.buttonBox.setObjectName("buttonBox")
        self.verticalLayout_2.addWidget(self.buttonBox)

        self.retranslateUi(AttachProcDialog)
        self.buttonBox.accepted.connect(AttachProcDialog.accept)
        self.buttonBox.rejected.connect(AttachProcDialog.reject)
        QtCore.QMetaObject.connectSlotsByName(AttachProcDialog)

    def retranslateUi(self, AttachProcDialog):
        _translate = QtCore.QCoreApplication.translate
        AttachProcDialog.setWindowTitle(_translate("AttachProcDialog", "Select process to attach..."))
        self.filterLineEdit.setPlaceholderText(_translate("AttachProcDialog", "Quick Filter"))


