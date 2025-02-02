#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
from PyQt5 import QtWidgets
from windows import RRGControlWindow


def main():
    """
    @brief Main function to run the RRG control interface.
    @details
    This function initializes the Qt application and displays the RRGControlWindow.
    """
    app = QtWidgets.QApplication(sys.argv)
    window = RRGControlWindow()
    window.show()
    sys.exit(app.exec_())


if __name__ == "__main__":
    main()
