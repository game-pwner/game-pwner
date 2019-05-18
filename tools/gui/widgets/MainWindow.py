from widgets import Ui_MainWindow
from PyQt5.QtWidgets import QDialog


class MainWindow(QDialog, Ui_MainWindow):
    def __init__(self):
        # super(MainWindow, self).__init__()
        super().__init__()
        # Set up the user interface from Designer.
        self.setupUi(self)
        self.retranslateUi(self)

        # Make some local modifications.
        # self.colorDepthCombo.addItem("2 colors (1 bit per pixel)")

        # Connect up the buttons.
        # self.okButton.clicked.connect(self.accept)
        # self.cancelButton.clicked.connect(self.reject)
