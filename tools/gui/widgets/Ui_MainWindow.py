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
        MainWindow.setStyleSheet("")
        MainWindow.setDocumentMode(False)
        MainWindow.setDockNestingEnabled(True)
        self.menubar = QtWidgets.QMenuBar(MainWindow)
        self.menubar.setGeometry(QtCore.QRect(0, 0, 800, 30))
        self.menubar.setObjectName("menubar")
        self.menuHelp = QtWidgets.QMenu(self.menubar)
        self.menuHelp.setObjectName("menuHelp")
        self.menuAttach = QtWidgets.QMenu(self.menubar)
        self.menuAttach.setObjectName("menuAttach")
        self.menuWindows = QtWidgets.QMenu(self.menubar)
        self.menuWindows.setObjectName("menuWindows")
        self.menuSettings = QtWidgets.QMenu(self.menubar)
        self.menuSettings.setObjectName("menuSettings")
        MainWindow.setMenuBar(self.menubar)
        self.actionAbout = QtWidgets.QAction(MainWindow)
        self.actionAbout.setObjectName("actionAbout")
        self.actionPID = QtWidgets.QAction(MainWindow)
        self.actionPID.setObjectName("actionPID")
        self.actionCRIU = QtWidgets.QAction(MainWindow)
        self.actionCRIU.setObjectName("actionCRIU")
        self.actionCurrentHelp = QtWidgets.QAction(MainWindow)
        self.actionCurrentHelp.setObjectName("actionCurrentHelp")
        self.actionHelp = QtWidgets.QAction(MainWindow)
        self.actionHelp.setObjectName("actionHelp")
        self.actionReset = QtWidgets.QAction(MainWindow)
        self.actionReset.setObjectName("actionReset")
        self.actionPreferences = QtWidgets.QAction(MainWindow)
        self.actionPreferences.setObjectName("actionPreferences")
        self.actionMemoryMaps = QtWidgets.QAction(MainWindow)
        self.actionMemoryMaps.setCheckable(True)
        self.actionMemoryMaps.setObjectName("actionMemoryMaps")
        self.actionMemoryView = QtWidgets.QAction(MainWindow)
        self.actionMemoryView.setCheckable(True)
        self.actionMemoryView.setObjectName("actionMemoryView")
        self.actionValueScanner = QtWidgets.QAction(MainWindow)
        self.actionValueScanner.setCheckable(True)
        self.actionValueScanner.setObjectName("actionValueScanner")
        self.actionPointerScanner = QtWidgets.QAction(MainWindow)
        self.actionPointerScanner.setCheckable(True)
        self.actionPointerScanner.setObjectName("actionPointerScanner")
        self.actionCheats = QtWidgets.QAction(MainWindow)
        self.actionCheats.setCheckable(True)
        self.actionCheats.setObjectName("actionCheats")
        self.menuHelp.addAction(self.actionHelp)
        self.menuHelp.addSeparator()
        self.menuHelp.addAction(self.actionCurrentHelp)
        self.menuHelp.addSeparator()
        self.menuHelp.addAction(self.actionAbout)
        self.menuAttach.addAction(self.actionPID)
        self.menuAttach.addAction(self.actionCRIU)
        self.menuWindows.addAction(self.actionMemoryMaps)
        self.menuWindows.addAction(self.actionMemoryView)
        self.menuWindows.addAction(self.actionValueScanner)
        self.menuWindows.addAction(self.actionPointerScanner)
        self.menuWindows.addAction(self.actionCheats)
        self.menuSettings.addAction(self.actionReset)
        self.menuSettings.addSeparator()
        self.menuSettings.addAction(self.actionPreferences)
        self.menubar.addAction(self.menuAttach.menuAction())
        self.menubar.addAction(self.menuWindows.menuAction())
        self.menubar.addAction(self.menuSettings.menuAction())
        self.menubar.addAction(self.menuHelp.menuAction())

        self.retranslateUi(MainWindow)
        self.actionPID.triggered['bool'].connect(MainWindow.onActionPID)
        self.actionCRIU.triggered['bool'].connect(MainWindow.onActionCRIU)
        self.actionMemoryMaps.triggered['bool'].connect(MainWindow.onActionMemoryMaps)
        self.actionMemoryView.triggered['bool'].connect(MainWindow.onActionMemoryView)
        self.actionValueScanner.triggered['bool'].connect(MainWindow.onActionValueScanner)
        self.actionPointerScanner.triggered['bool'].connect(MainWindow.onActionPointerScanner)
        self.actionCheats.triggered['bool'].connect(MainWindow.onActionCheats)
        self.actionReset.triggered['bool'].connect(MainWindow.onActionReset)
        self.actionPreferences.triggered['bool'].connect(MainWindow.onActionPreferences)
        self.actionHelp.triggered['bool'].connect(MainWindow.onActionHelp)
        self.actionCurrentHelp.triggered['bool'].connect(MainWindow.onActionCurrentHelp)
        self.actionAbout.triggered['bool'].connect(MainWindow.onActionAbout)
        QtCore.QMetaObject.connectSlotsByName(MainWindow)

    def retranslateUi(self, MainWindow):
        _translate = QtCore.QCoreApplication.translate
        MainWindow.setWindowTitle(_translate("MainWindow", "pwner-gui"))
        self.menuHelp.setTitle(_translate("MainWindow", "Help"))
        self.menuAttach.setTitle(_translate("MainWindow", "Attach"))
        self.menuWindows.setTitle(_translate("MainWindow", "Window"))
        self.menuSettings.setTitle(_translate("MainWindow", "Settings"))
        self.actionAbout.setText(_translate("MainWindow", "About pwner"))
        self.actionPID.setText(_translate("MainWindow", "PID"))
        self.actionCRIU.setText(_translate("MainWindow", "CRIU"))
        self.actionCurrentHelp.setText(_translate("MainWindow", "Current tool help"))
        self.actionHelp.setText(_translate("MainWindow", "pwner help"))
        self.actionReset.setText(_translate("MainWindow", "Reset layout"))
        self.actionPreferences.setText(_translate("MainWindow", "Preferences"))
        self.actionMemoryMaps.setText(_translate("MainWindow", "Memory maps"))
        self.actionMemoryView.setText(_translate("MainWindow", "Memory view"))
        self.actionValueScanner.setText(_translate("MainWindow", "Value Scanner"))
        self.actionPointerScanner.setText(_translate("MainWindow", "Pointer Scanner"))
        self.actionCheats.setText(_translate("MainWindow", "Cheats"))


