# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file '/home/user/github.com/game-pwner/game-pwner/tools/gui/dialogs/About.ui'
#
# Created by: PyQt5 UI code generator 5.12.1
#
# WARNING! All changes made in this file will be lost!

from PyQt5 import QtCore, QtGui, QtWidgets


class Ui_About(object):
    def setupUi(self, About):
        About.setObjectName("About")
        About.resize(400, 300)
        self.label = QtWidgets.QLabel(About)
        self.label.setGeometry(QtCore.QRect(130, 100, 131, 18))
        self.label.setObjectName("label")

        self.retranslateUi(About)
        QtCore.QMetaObject.connectSlotsByName(About)

    def retranslateUi(self, About):
        _translate = QtCore.QCoreApplication.translate
        About.setWindowTitle(_translate("About", "About Dialog"))
        self.label.setText(_translate("About", "ABOUT!!!"))


