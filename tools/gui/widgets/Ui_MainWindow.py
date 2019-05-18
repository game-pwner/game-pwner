# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file '/home/user/github.com/game-pwner/game-pwner/tools/gui/widgets/MainWindow.ui'
#
# Created by: PyQt5 UI code generator 5.12.1
#
# WARNING! All changes made in this file will be lost!

from PyQt5 import QtCore, QtGui, QtWidgets


class Ui_MainWindow(object):
    def setupUi(self, MainWindow):
        MainWindow.setObjectName("MainWindow")
        MainWindow.resize(800, 600)
        self.centralwidget = QtWidgets.QWidget(MainWindow)
        self.centralwidget.setObjectName("centralwidget")
        self.verticalLayout_2 = QtWidgets.QVBoxLayout(self.centralwidget)
        self.verticalLayout_2.setObjectName("verticalLayout_2")
        MainWindow.setCentralWidget(self.centralwidget)
        self.menuBar = QtWidgets.QMenuBar(MainWindow)
        self.menuBar.setGeometry(QtCore.QRect(0, 0, 809, 30))
        self.menuBar.setObjectName("menuBar")
        self.menuFile = QtWidgets.QMenu(self.menuBar)
        self.menuFile.setObjectName("menuFile")
        self.menuHelp = QtWidgets.QMenu(self.menuBar)
        self.menuHelp.setObjectName("menuHelp")
        self.menuVies = QtWidgets.QMenu(self.menuBar)
        self.menuVies.setObjectName("menuVies")
        MainWindow.setMenuBar(self.menuBar)
        self.actionAttachPID = QtWidgets.QAction(MainWindow)
        self.actionAttachPID.setObjectName("actionAttachPID")
        self.actionCriu = QtWidgets.QAction(MainWindow)
        self.actionCriu.setObjectName("actionCriu")
        self.actionExit = QtWidgets.QAction(MainWindow)
        self.actionExit.setObjectName("actionExit")
        self.actionAbout = QtWidgets.QAction(MainWindow)
        self.actionAbout.setObjectName("actionAbout")
        self.actionPwnerHelp = QtWidgets.QAction(MainWindow)
        self.actionPwnerHelp.setObjectName("actionPwnerHelp")
        self.actionResetLayout = QtWidgets.QAction(MainWindow)
        self.actionResetLayout.setObjectName("actionResetLayout")
        self.menuFile.addAction(MainWindow.actionAttach_PID)
        self.menuFile.addAction(self.actionCriu)
        self.menuFile.addSeparator()
        self.menuFile.addAction(self.actionExit)
        self.menuHelp.addAction(MainWindow.actionpwner_help)
        self.menuHelp.addAction(self.actionAbout)
        self.menuVies.addAction(MainWindow.actionReset_layout)
        self.menuBar.addAction(self.menuFile.menuAction())
        self.menuBar.addAction(self.menuVies.menuAction())
        self.menuBar.addAction(self.menuHelp.menuAction())

        self.retranslateUi(MainWindow)
        QtCore.QMetaObject.connectSlotsByName(MainWindow)

    def retranslateUi(self, MainWindow):
        _translate = QtCore.QCoreApplication.translate
        MainWindow.setWindowTitle(_translate("MainWindow", "pwner-gui"))
        self.menuFile.setTitle(_translate("MainWindow", "File"))
        self.menuHelp.setTitle(_translate("MainWindow", "Help"))
        self.menuVies.setTitle(_translate("MainWindow", "View"))
        self.actionAttachPID.setText(_translate("MainWindow", "Attach PID"))
        self.actionCriu.setText(_translate("MainWindow", "Criu"))
        self.actionExit.setText(_translate("MainWindow", "Exit"))
        self.actionAbout.setText(_translate("MainWindow", "About"))
        self.actionPwnerHelp.setText(_translate("MainWindow", "pwner help"))
        self.actionResetLayout.setText(_translate("MainWindow", "Reset layout"))


