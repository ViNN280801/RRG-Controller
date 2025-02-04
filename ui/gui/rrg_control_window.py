# -*- coding: utf-8 -*-

import os
import datetime
from PyQt5 import QtWidgets, QtCore, QtGui
from PyQt5.QtWidgets import QMessageBox, QShortcut
from PyQt5.QtGui import QKeySequence
import serial.tools.list_ports
from src.rrg import RRGController
from src.relay import RelayController
from src.config import ConfigLoader
from matplotlib.figure import Figure
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas

RELAY_DEFAULT_BAUDRATE = 115200
RELAY_DEFAULT_TIMEOUT = 10
RELAY_DEFAULT_SLAVE_ID = 6

RRG_DEFAULT_BAUDRATE = 38400
RRG_DEFAULT_TIMEOUT = 50
RRG_DEFAULT_SLAVE_ID = 1

PLOT_UPDATE_TIME_TICK_MS = 5000


class RRGControlWindow(QtWidgets.QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("RRG Control Panel")
        self.resize(1200, 700)

        self.rrg_controller = RRGController()
        self.relay_controller = RelayController()
        self.config_loader = ConfigLoader()

        self.available_ports = self._get_available_ports()

        self._create_toolbar()
        self._create_central_widget()

        QShortcut(QKeySequence("Ctrl+W"), self, activated=self._confirm_close)
        QShortcut(QKeySequence("Ctrl+Q"), self, activated=self._confirm_close)

        if len(self.available_ports) < 2:
            self._log_message("Not enough serial ports available. UI is disabled.")
            self._disable_ui()

        self._load_config_data()

    def closeEvent(self, event: QtGui.QCloseEvent) -> None:
        """
        @brief Overrides the window close event to prompt the user for confirmation
               and safely close connections.
        """
        reply = QMessageBox.question(
            self,
            "Confirm Exit",
            "Are you sure you want to exit?",
            QMessageBox.Yes | QMessageBox.No,
            QMessageBox.No,
        )
        if reply == QMessageBox.Yes:
            self._close_connections()
            event.accept()
        else:
            event.ignore()

    def _init_graph(self):
        """Initializes the Matplotlib graph for displaying flow over time."""
        self.figure = Figure(figsize=(20, 10))
        self.canvas = FigureCanvas(self.figure)
        self.canvas.setMinimumHeight(500)
        self.ax = self.figure.add_subplot(111)

        self.ax.set_xlabel("Time (minutes)")
        self.ax.set_ylabel("Flow (SCCM)")
        self.ax.set_title("Gas Flow over Time")

        self.flow_data = []  # Stores (time in minutes, flow)
        self.start_time = datetime.datetime.now()  # Set start time for reference

        # Add the canvas below UI elements
        self.centralWidget().layout().addWidget(self.canvas)

    def _update_graph(self):
        """
        Queries the current flow from the RRG controller, appends the data point,
        and updates the Matplotlib graph.
        """
        if self.rrg_controller.IsDisconnected():
            return

        err, flow = self.rrg_controller.GetFlow()
        current_time = datetime.datetime.now()
        elapsed_minutes = (
            current_time - self.start_time
        ).total_seconds() / 60  # Convert to minutes

        if err == self.rrg_controller.RRG_OK:
            self.flow_data.append((elapsed_minutes, flow))
            self.flow_data = self.flow_data[-60:]  # Keep only last 60 data points

            times = [t for t, _ in self.flow_data]
            flows = [f for _, f in self.flow_data]

            self.ax.clear()
            self.ax.plot(times, flows, marker="o", linestyle="-")

            self.ax.set_xlabel("Time (minutes)")
            self.ax.set_ylabel("Flow (SCCM)")
            self.ax.set_title("Gas Flow over Time")

            self.ax.minorticks_on()
            self.ax.grid(True, which="major", linestyle="-", linewidth=0.8)
            self.ax.grid(True, which="minor", linestyle="--", linewidth=0.5, alpha=0.5)

            self.canvas.draw()
            self._log_message(
                f"Current flow is {flow} [cm3/min] at time moment {elapsed_minutes:.2f} [min]"
            )
        else:
            self._rrg_show_error_msg()

    def _confirm_close(self):
        """
        @brief Called by keyboard shortcuts (Ctrl+W, Ctrl+Q) to ask for exit confirmation.
        """
        reply = QMessageBox.question(
            self,
            "Confirm Exit",
            "Are you sure you want to exit?",
            QMessageBox.Yes | QMessageBox.No,
            QMessageBox.No,
        )
        if reply == QMessageBox.Yes:
            self._close_connections()
            QtWidgets.QApplication.quit()

    def _open_connections(self):
        # 1. Connect to the relay
        # relay_port = self.combo_port_1.currentText()
        # relay_err = self.relay_controller.TurnOn(
        #     relay_port,
        #     self.relay_config_dict.get('baudrate', RELAY_DEFAULT_BAUDRATE),
        #     self.relay_config_dict.get('slave_id', RELAY_DEFAULT_SLAVE_ID),
        #     self.relay_config_dict.get('timeout', RELAY_DEFAULT_TIMEOUT)
        # )
        # if relay_err != self.relay_controller.RELAY_OK:
        #     self._relay_show_error_msg()
        #     return
        # else:
        #     self._log_message(f"Relay device connected on port {relay_port}.")

        # 2. Connect to the Gas Flow Regulator
        rrg_port = self.combo_port_2.currentText()
        rrg_err = self.rrg_controller.TurnOn(
            "/dev/ttyUSB0",
            baudrate=self.rrg_config_dict.get("baudrate", RRG_DEFAULT_BAUDRATE),
            slave_id=self.rrg_config_dict.get("slave_id", RRG_DEFAULT_SLAVE_ID),
            timeout=self.rrg_config_dict.get("timeout", RRG_DEFAULT_TIMEOUT),
        )
        if rrg_err != self.rrg_controller.RRG_OK:
            self._rrg_show_error_msg()
            self.toggle_rrg_button.setChecked(False)
            self.toggle_rrg_button.setText("Turn RRG ON")
        else:
            self._log_message(
                f"Gas Flow Regulator device connected on port {rrg_port}."
            )
            self.toggle_rrg_button.setText("Turn RRG OFF")

    def _close_connections(self):
        """
        @brief Safely closes the connections for the Gas Flow Regulator and Relay devices.
        @details
        This method calls the appropriate TurnOff/close methods on the controllers.
        """
        # 1. Turn off the Gas Flow Regulator
        if self.rrg_controller.IsConnected():
            rrg_err = self.rrg_controller.TurnOff()
            if rrg_err != self.rrg_controller.RRG_OK:
                self._rrg_show_error_msg()
            else:
                self._log_message("Gas Flow Regulator device disconnected.")
            self.toggle_rrg_button.setText("Turn RRG ON")

        # 2. Turn off the Relay
        if self.relay_controller.IsConnected():
            relay_err = self.relay_controller.TurnOff()
            if relay_err != self.relay_controller.RELAY_OK:
                self._relay_show_error_msg()
            else:
                self._log_message("Relay device disconnected.")

    def _load_config_data(self):
        rrg_config_path = os.path.join(
            os.path.dirname(os.path.dirname(__file__)), "config", "rrg.yaml"
        )
        relay_config_path = os.path.join(
            os.path.dirname(os.path.dirname(__file__)), "config", "relay.yaml"
        )
        try:
            self.rrg_config_dict = self.config_loader.load_config(rrg_config_path)
            self.relay_config_dict = self.config_loader.load_config(relay_config_path)
        except Exception as e:
            self._log_message(f"Failed to load config: {e}")

    def _get_available_ports(self):
        ports = serial.tools.list_ports.comports()
        available = [port.device for port in ports]
        return available

    def _disable_ui(self):
        for widget in self.findChildren(QtWidgets.QWidget):
            widget.setEnabled(False)

    def _create_toolbar(self):
        self.toolbar = QtWidgets.QToolBar("COM Port Selection", self)
        self.addToolBar(self.toolbar)

        self.combo_port_1 = QtWidgets.QComboBox(self)
        self.combo_port_1.setToolTip("Select COM port for the Relay (Реле)")
        self.toolbar.addWidget(self.combo_port_1)

        self.toolbar.addSeparator()

        self.combo_port_2 = QtWidgets.QComboBox(self)
        self.combo_port_2.setToolTip("Select COM port for Gas Flow Regulator (РРГ)")
        self.toolbar.addWidget(self.combo_port_2)

        self._update_combo_boxes(initial=True)

        self.combo_port_1.currentIndexChanged.connect(self._on_combo_changed)
        self.combo_port_2.currentIndexChanged.connect(self._on_combo_changed)

    def _update_combo_boxes(self, initial=False):
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
        self._update_combo_boxes()

    def _create_central_widget(self):
        self.central_widget = QtWidgets.QWidget(self)
        self.setCentralWidget(self.central_widget)

        layout = QtWidgets.QVBoxLayout()
        self.central_widget.setLayout(layout)

        self.toggle_rrg_button = QtWidgets.QPushButton("Turn RRG ON", self)
        self.toggle_rrg_button.setCheckable(True)
        self.toggle_rrg_button.clicked.connect(self._toggle_rrg)
        layout.addWidget(self.toggle_rrg_button)

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

        # --- Initialize the graph ---
        self._init_graph()
        self.graph_timer = QtCore.QTimer(self)
        self.graph_timer.timeout.connect(self._update_graph)
        self.graph_timer.start(PLOT_UPDATE_TIME_TICK_MS)

        self.flow_display = QtWidgets.QTextEdit(self)
        self.flow_display.setReadOnly(True)
        self.flow_display.setPlaceholderText("Current flow will be displayed here...")
        layout.addWidget(self.flow_display)

    @QtCore.pyqtSlot()
    def _toggle_rrg(self):
        if self.toggle_rrg_button.isChecked():
            self._open_connections()
        else:
            self._close_connections()

    @QtCore.pyqtSlot()
    def _send_setpoint(self):
        text = self.setpoint_line_edit.text()
        if text == "":
            self._log_message("Setpoint value is empty.")
            return

        try:
            setpoint = float(text)
        except ValueError:
            self._log_message("Invalid setpoint value entered.")
            return

        if self.rrg_controller.IsDisconnected():
            self._rrg_show_error_msg()
            return

        err = self.rrg_controller.SetFlow(setpoint)
        if err != self.rrg_controller.RRG_OK:
            self._rrg_show_error_msg()
        else:
            self._log_message(f"Setpoint {setpoint} sent successfully.")

    def _log_message(self, message: str):
        self.flow_display.append(message)

    def _rrg_show_error_msg(self):
        rrg_error = self.rrg_controller.GetLastError()
        if isinstance(rrg_error, str):
            QMessageBox.critical(
                self,
                "Gas Flow Regulator Error",
                f"{self.rrg_controller.GetLastError()}",
            )
        elif isinstance(rrg_error, int) and rrg_error == -1:
            QMessageBox.critical(
                self,
                "Gas Flow Regulator Error",
                "Gas Flow Regulator device is not connected",
            )
        else:
            QMessageBox.critical(
                self, "Gas Flow Regulator Error", "Unknown error occured"
            )

    def _relay_show_error_msg(self):
        relay_error = self.relay_controller.GetLastError()
        if isinstance(relay_error, str):
            QMessageBox.critical(
                self,
                "Gas Flow Regulator Error",
                f"{self.relay_controller.GetLastError()}",
            )
        elif isinstance(relay_error, int) and relay_error == -1:
            QMessageBox.critical(
                self, "Gas Flow Regulator Error", "Relay device is not connected"
            )
        else:
            QMessageBox.critical(
                self, "Gas Flow Regulator Error", "Unknown error occured"
            )
