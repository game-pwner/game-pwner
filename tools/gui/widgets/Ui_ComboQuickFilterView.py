# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file '/home/user/github.com/game-pwner/game-pwner/tools/gui/widgets/ComboQuickFilterView.ui'
#
# Created by: PyQt5 UI code generator 5.12.1
#
# WARNING! All changes made in this file will be lost!

from PyQt5 import QtCore, QtGui, QtWidgets


class Ui_ComboQuickFilterView(object):
    def setupUi(self, ComboQuickFilterView):
        ComboQuickFilterView.setObjectName("ComboQuickFilterView")
        ComboQuickFilterView.resize(378, 20)
        self.horizontalLayout = QtWidgets.QHBoxLayout(ComboQuickFilterView)
        self.horizontalLayout.setContentsMargins(0, 0, 0, 0)
        self.horizontalLayout.setObjectName("horizontalLayout")
        self.lineEdit = QtWidgets.QLineEdit(ComboQuickFilterView)
        self.lineEdit.setObjectName("lineEdit")
        self.horizontalLayout.addWidget(self.lineEdit)
        self.label = QtWidgets.QLabel(ComboQuickFilterView)
        self.label.setObjectName("label")
        self.horizontalLayout.addWidget(self.label)
        self.comboBox = QtWidgets.QComboBox(ComboQuickFilterView)
        self.comboBox.setObjectName("comboBox")
        self.horizontalLayout.addWidget(self.comboBox)

        self.retranslateUi(ComboQuickFilterView)
        QtCore.QMetaObject.connectSlotsByName(ComboQuickFilterView)

    def retranslateUi(self, ComboQuickFilterView):
        _translate = QtCore.QCoreApplication.translate
        ComboQuickFilterView.setWindowTitle(_translate("ComboQuickFilterView", "Form"))
        self.lineEdit.setPlaceholderText(_translate("ComboQuickFilterView", "Quick Filter"))
        self.label.setText(_translate("ComboQuickFilterView", "TextLabel"))


