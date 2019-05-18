# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file '/home/user/github.com/game-pwner/game-pwner/tools/gui/widgets/StringsWidget.ui'
#
# Created by: PyQt5 UI code generator 5.12.1
#
# WARNING! All changes made in this file will be lost!

from PyQt5 import QtCore, QtGui, QtWidgets


class Ui_StringsWidget(object):
    def setupUi(self, StringsWidget):
        StringsWidget.setObjectName("StringsWidget")
        StringsWidget.resize(400, 300)
        StringsWidget.setWindowTitle("Strings")
        self.dockWidgetContents = QtWidgets.QWidget()
        self.dockWidgetContents.setObjectName("dockWidgetContents")
        self.verticalLayout = QtWidgets.QVBoxLayout(self.dockWidgetContents)
        self.verticalLayout.setContentsMargins(0, 0, 0, 0)
        self.verticalLayout.setSpacing(0)
        self.verticalLayout.setObjectName("verticalLayout")
        self.stringsTreeView = CutterTreeView(self.dockWidgetContents)
        sizePolicy = QtWidgets.QSizePolicy(QtWidgets.QSizePolicy.Expanding, QtWidgets.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.stringsTreeView.sizePolicy().hasHeightForWidth())
        self.stringsTreeView.setSizePolicy(sizePolicy)
        self.stringsTreeView.setStyleSheet("QTreeWidget::item\n"
"{\n"
"    padding-top: 1px;\n"
"    padding-bottom: 1px;\n"
"}")
        self.stringsTreeView.setFrameShape(QtWidgets.QFrame.NoFrame)
        self.stringsTreeView.setLineWidth(0)
        self.stringsTreeView.setIndentation(8)
        self.stringsTreeView.setSortingEnabled(True)
        self.stringsTreeView.setObjectName("stringsTreeView")
        self.verticalLayout.addWidget(self.stringsTreeView)
        self.quickFilterView = ComboQuickFilterView(self.dockWidgetContents)
        sizePolicy = QtWidgets.QSizePolicy(QtWidgets.QSizePolicy.Preferred, QtWidgets.QSizePolicy.Maximum)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.quickFilterView.sizePolicy().hasHeightForWidth())
        self.quickFilterView.setSizePolicy(sizePolicy)
        self.quickFilterView.setObjectName("quickFilterView")
        self.verticalLayout.addWidget(self.quickFilterView)
        StringsWidget.setWidget(self.dockWidgetContents)
        self.actionCopy_Address = QtWidgets.QAction(StringsWidget)
        self.actionCopy_Address.setObjectName("actionCopy_Address")
        self.actionCopy_String = QtWidgets.QAction(StringsWidget)
        self.actionCopy_String.setObjectName("actionCopy_String")
        self.actionX_refs = QtWidgets.QAction(StringsWidget)
        self.actionX_refs.setObjectName("actionX_refs")
        self.actionFilter = QtWidgets.QAction(StringsWidget)
        self.actionFilter.setObjectName("actionFilter")

        self.retranslateUi(StringsWidget)
        QtCore.QMetaObject.connectSlotsByName(StringsWidget)

    def retranslateUi(self, StringsWidget):
        _translate = QtCore.QCoreApplication.translate
        self.actionCopy_Address.setText(_translate("StringsWidget", "Copy Address"))
        self.actionCopy_String.setText(_translate("StringsWidget", "Copy String"))
        self.actionX_refs.setText(_translate("StringsWidget", "Xrefs"))
        self.actionFilter.setText(_translate("StringsWidget", "Filter"))


from widgets.ComboQuickFilterView import ComboQuickFilterView
from widgets.CutterTreeView import CutterTreeView
