from PyQt5 import QtWidgets
from PyQt5.QtWidgets import QTableWidgetItem, QMessageBox
from PyQt5.QtGui import QPen, QImage
from PyQt5.QtCore import Qt, QPointF
import sys
# IDE: JetBrains CLion: mark ./tools/gui folder as Source Root
from widgets.MainWindow import MainWindow


def main() -> int:
    app = QtWidgets.QApplication(sys.argv)
    w = MainWindow()
    w.show()
    return app.exec_()


if __name__ == "__main__":
    sys.exit(main())
