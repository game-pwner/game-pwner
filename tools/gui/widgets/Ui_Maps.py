# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file '/home/user/github.com/game-pwner/game-pwner/tools/gui/widgets/Maps.ui'
#
# Created by: PyQt5 UI code generator 5.12.1
#
# WARNING! All changes made in this file will be lost!

from PyQt5 import QtCore, QtGui, QtWidgets


class Ui_Maps(object):
    def setupUi(self, Maps):
        Maps.setObjectName("Maps")
        Maps.resize(408, 363)
        Maps.setStyleSheet("")
        Maps.setWindowTitle("Dashboard")
        self.dockWidgetContents = QtWidgets.QWidget()
        self.dockWidgetContents.setObjectName("dockWidgetContents")
        self.horizontalLayout = QtWidgets.QHBoxLayout(self.dockWidgetContents)
        self.horizontalLayout.setContentsMargins(0, 0, 0, 0)
        self.horizontalLayout.setSpacing(0)
        self.horizontalLayout.setObjectName("horizontalLayout")
        self.treeMaps = QtWidgets.QTreeView(self.dockWidgetContents)
        self.treeMaps.setObjectName("treeMaps")
        self.horizontalLayout.addWidget(self.treeMaps)
        Maps.setWidget(self.dockWidgetContents)

        self.retranslateUi(Maps)
        QtCore.QMetaObject.connectSlotsByName(Maps)

    def retranslateUi(self, Maps):
        pass


