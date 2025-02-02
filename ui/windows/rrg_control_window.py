# -*- coding: utf-8 -*-
"""
@file rrg_control_window.py
@brief A PyQt5-based UI for controlling an RRG device.
@details
This module implements a main window that contains:
 - A toolbar with two mutually exclusive COM port selection combo boxes.
 - A toggle button for switching the RRG device on/off.
 - A QLineEdit with a float validator for entering a flow setpoint.
 - A button for sending the setpoint command.
 - A text area for displaying the current flow.
 
If fewer than two available serial ports are detected, the UI is disabled.
All device commands are routed through an instance of RRGController.
"""

from PyQt5 import QtWidgets, QtCore, QtGui
import serial.tools.list_ports
from rrg import RRGController


class RRGControlWindow(QtWidgets.QMainWindow):
    """
    @class RRGControlWindow
    @brief Main window for the RRG control interface.
    @details
    This window provides a toolbar for selecting COM ports and a central widget
    with controls for toggling the RRG device, sending a setpoint, and displaying
    the current flow. The two combo boxes in the toolbar are maintained as mutually
    exclusive selections.
    """

    def __init__(self):
        """
        @brief Constructs an RRGControlWindow object.
        @details
        The available serial ports are detected at startup. If fewer than two ports are
        available, the UI is disabled.
        """
        super().__init__()
        self.setWindowTitle("RRG Control Panel")
        self.resize(600, 400)

        # Create a controller instance to manage the RRG device.
        self.controller = RRGController()

        # Query the available serial ports.
        self.available_ports = self.get_available_ports()

        # Initialize UI components.
        self._create_toolbar()
        self._create_central_widget()

        # If less than two ports are available, disable the UI.
        if len(self.available_ports) < 2:
            self._log_message("Not enough serial ports available. UI is disabled.")
            self.disable_ui()

    def get_available_ports(self):
        """
        @brief Detects and returns a list of available serial ports.
        @return A list of port names (e.g., ['COM3', 'COM4'] on Windows or ['/dev/ttyUSB0', '/dev/ttyS0'] on Linux).
        @details
        This method uses pyserial's list_ports.comports() to query the system.
        """
        ports = serial.tools.list_ports.comports()
        # Extract the device name from each port info.
        available = [port.device for port in ports]
        return available

    def disable_ui(self):
        """
        @brief Disables all UI elements.
        @details
        This method disables the toolbar and central widget controls so that the user cannot interact
        with the interface when there are not enough serial ports.
        """
        for widget in self.findChildren(QtWidgets.QWidget):
            widget.setEnabled(False)

    def _create_toolbar(self):
        """
        @brief Creates the toolbar with two mutually exclusive COM port selection combo boxes.
        @details
        This method adds two QComboBox widgets to the toolbar. When a selection in one combo
        box changes, the available options in the other combo are updated to ensure that the same
        COM port cannot be selected in both fields.
        """
        self.toolbar = QtWidgets.QToolBar("COM Port Selection", self)
        self.addToolBar(self.toolbar)

        # First COM port selection combo box.
        self.combo_port_1 = QtWidgets.QComboBox(self)
        self.combo_port_1.setToolTip("Select COM port for interface 1")
        self.toolbar.addWidget(self.combo_port_1)

        # Separator for clarity.
        self.toolbar.addSeparator()

        # Second COM port selection combo box.
        self.combo_port_2 = QtWidgets.QComboBox(self)
        self.combo_port_2.setToolTip("Select COM port for interface 2")
        self.toolbar.addWidget(self.combo_port_2)

        # Populate both combo boxes with available ports.
        self._update_combo_boxes(initial=True)

        # Connect signals for mutual exclusion.
        self.combo_port_1.currentIndexChanged.connect(self._on_combo_changed)
        self.combo_port_2.currentIndexChanged.connect(self._on_combo_changed)

    def _update_combo_boxes(self, initial=False):
        """
        @brief Updates the items in the COM port combo boxes to enforce mutual exclusion.
        @param initial If True, both combo boxes are populated with all available ports.
        @details
        When one combo box has a selected COM port, that port is removed from the list in the other
        combo box. If the current selection becomes invalid, the first available option is selected.
        """
        current1 = self.combo_port_1.currentText()
        current2 = self.combo_port_2.currentText()

        self.combo_port_1.blockSignals(True)
        self.combo_port_2.blockSignals(True)

        self.combo_port_1.clear()
        self.combo_port_2.clear()

        ports_for_combo1 = list(self.available_ports)
        ports_for_combo2 = list(self.available_ports)
        if not initial:
            if current2 in ports_for_combo1:
                ports_for_combo1.remove(current2)
            if current1 in ports_for_combo2:
                ports_for_combo2.remove(current1)

        self.combo_port_1.addItems(ports_for_combo1)
        self.combo_port_2.addItems(ports_for_combo2)

        if current1 in ports_for_combo1:
            self.combo_port_1.setCurrentIndex(ports_for_combo1.index(current1))
        if current2 in ports_for_combo2:
            self.combo_port_2.setCurrentIndex(ports_for_combo2.index(current2))

        self.combo_port_1.blockSignals(False)
        self.combo_port_2.blockSignals(False)

    def _on_combo_changed(self):
        """
        @brief Slot called when either COM port combo box selection changes.
        @details
        This slot updates the other combo box’s available items to ensure that both
        combo boxes cannot have the same COM port selected.
        """
        self._update_combo_boxes()

    def _create_central_widget(self):
        """
        @brief Creates the central widget with device controls and flow display.
        @details
        The central widget contains:
         - A toggle button for turning the RRG device on/off.
         - A QLineEdit for entering a flow setpoint (with float validation).
         - A button for sending the setpoint command.
         - A QTextEdit for displaying the current flow.
        """
        self.central_widget = QtWidgets.QWidget(self)
        self.setCentralWidget(self.central_widget)

        layout = QtWidgets.QVBoxLayout()
        self.central_widget.setLayout(layout)

        # Toggle button for RRG on/off.
        self.toggle_rrg_button = QtWidgets.QPushButton("Turn RRG ON", self)
        self.toggle_rrg_button.setCheckable(True)
        self.toggle_rrg_button.clicked.connect(self._toggle_rrg)
        layout.addWidget(self.toggle_rrg_button)

        # Layout for setpoint input and button.
        form_layout = QtWidgets.QHBoxLayout()

        self.setpoint_line_edit = QtWidgets.QLineEdit(self)
        self.setpoint_line_edit.setPlaceholderText("Enter flow setpoint (float)")
        double_validator = QtGui.QDoubleValidator(self)
        self.setpoint_line_edit.setValidator(double_validator)
        form_layout.addWidget(self.setpoint_line_edit)

        self.send_setpoint_button = QtWidgets.QPushButton("Send Setpoint", self)
        self.send_setpoint_button.clicked.connect(self._send_setpoint)
        form_layout.addWidget(self.send_setpoint_button)

        layout.addLayout(form_layout)

        # QTextEdit for displaying current flow.
        self.flow_display = QtWidgets.QTextEdit(self)
        self.flow_display.setReadOnly(True)
        self.flow_display.setPlaceholderText("Current flow will be displayed here...")
        layout.addWidget(self.flow_display)

    @QtCore.pyqtSlot()
    def _toggle_rrg(self):
        """
        @brief Slot to handle toggling the RRG device on/off.
        @details
        When turning ON, the selected COM port is retrieved from the combo box and used
        to attempt a connection via the RRGController. When turning OFF, the controller
        is asked to close the connection. If an error occurs, a message box is displayed.
        """
        if self.toggle_rrg_button.isChecked():
            # Attempt to turn ON the RRG device using the selected COM port.
            # For this example, we take the selection from combo_port_1.
            selected_port = self.combo_port_1.currentText()
            err = self.controller.TurnOn(
                selected_port, baudrate=9600, slave_id=1, timeout=1000
            )
            if err != self.controller.RRG_OK:
                self._show_error_message(err)
                # Revert the button state if connection fails.
                self.toggle_rrg_button.setChecked(False)
                self.toggle_rrg_button.setText("Turn RRG ON")
            else:
                self._log_message(f"RRG device connected on port {selected_port}.")
                self.toggle_rrg_button.setText("Turn RRG OFF")
        else:
            # Attempt to turn OFF the RRG device.
            err = self.controller.TurnOff()
            if err != self.controller.RRG_OK:
                self._show_error_message(err)
            else:
                self._log_message("RRG device disconnected.")
            self.toggle_rrg_button.setText("Turn RRG ON")

    @QtCore.pyqtSlot()
    def _send_setpoint(self):
        """
        @brief Slot to handle sending the setpoint command to the RRG device.
        @details
        This method reads the setpoint value from the input field, validates it,
        and instructs the RRGController to set the flow. If an error occurs, a message box
        is displayed.
        """
        text = self.setpoint_line_edit.text()
        if text == "":
            self._log_message("Setpoint value is empty.")
            return

        try:
            setpoint = float(text)
        except ValueError:
            self._log_message("Invalid setpoint value entered.")
            return

        err = self.controller.SetFlow(setpoint)
        if err != self.controller.RRG_OK:
            self._show_error_message(err)
        else:
            self._log_message(f"Setpoint {setpoint} sent successfully.")

    def _show_error_message(self, err_code: int):
        """
        @brief Displays an error message in a QMessageBox based on the error code.
        @param err_code The error code returned by an RRGController method.
        @details
        This method retrieves a description of the last error from the RRG device (if available)
        and displays it in a QMessageBox.
        """
        # Attempt to retrieve the error description from the underlying RRG instance.
        error_description = "Unknown error."
        if self.controller._rrg is not None:
            try:
                # Call get_last_error() from the RRG wrapper.
                error_description = self.controller._rrg.get_last_error()
            except Exception:
                error_description = "Failed to retrieve error description."

        msg_box = QtWidgets.QMessageBox(self)
        msg_box.setIcon(QtWidgets.QMessageBox.Critical)
        msg_box.setWindowTitle("RRG Error")
        msg_box.setText(f"Error Code: {err_code}\nDescription: {error_description}")
        msg_box.exec_()

    def _log_message(self, message: str):
        """
        @brief Logs a message to the flow display area.
        @param message The message to log.
        @details
        This method appends the given message to the QTextEdit widget along with a newline.
        """
        self.flow_display.append(message)
