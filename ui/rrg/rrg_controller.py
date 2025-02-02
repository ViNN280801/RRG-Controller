# -*- coding: utf-8 -*-

"""
@file rrg_controller.py
@brief Provides the RRGController class for managing an RRG device.
@details
This module defines the RRGController class that encapsulates the functionality of the RRG
device by utilizing the RRG wrapper. The controller adheres to OOP and SOLID principles by:
  - Encapsulating the device connection and communication logic.
  - Providing a clear public API for turning the device on/off and controlling the flow.
Each method returns an error code (with 0 indicating success) so that calling code may
handle error conditions appropriately.
"""

from rrg.rrg_wrapper import RRG


class RRGController:
    """
    @class RRGController
    @brief Controller for the RRG device.
    @details
    This class wraps the RRG C API (exposed via the Python RRG wrapper) and provides a simple,
    testable interface with methods:
      - TurnOn(com_port, baudrate, slave_id, timeout)
      - TurnOff()
      - SetFlow(setpoint)
      - GetFlow()
    All methods return an error code (0 for success). The GetFlow method returns a tuple of the
    form (error_code, flow_value) where flow_value is valid only if error_code is 0.
    """

    # Error code constants.
    RRG_OK = 0
    ERROR_RRG_NOT_CONNECTED = -1
    ERROR_RRG_CONNECT_FAILED = -2
    ERROR_RRG_SET_FLOW_FAILED = -3
    ERROR_RRG_GET_FLOW_FAILED = -4

    def __init__(self):
        """
        @brief Initializes an RRGController instance.
        @details
        The RRGController starts in a disconnected state. Call TurnOn() with the desired COM port
        to establish a connection.
        """
        self._rrg = None  # Will hold an instance of RRG after connection

    def TurnOn(
        self,
        com_port: str,
        baudrate: int = 9600,
        slave_id: int = 1,
        timeout: int = 1000,
    ) -> int:
        """
        @brief Connects to the RRG device on the specified COM port.
        @param com_port Serial port name (e.g., "COM3" on Windows or "/dev/ttyUSB0" on Linux).
        @param baudrate Baud rate for communication.
        @param slave_id MODBUS slave ID.
        @param timeout Communication timeout in milliseconds.
        @return RRG_OK on success, or an error code if connection fails.
        @details
        This method instantiates the RRG wrapper with the provided parameters and attempts
        to establish a connection to the device.
        """
        try:
            self._rrg = RRG(com_port, baudrate, slave_id, timeout)
            if self._rrg.connect():
                return self.RRG_OK
            else:
                # Connection failed; ensure _rrg is set to None.
                self._rrg = None
                return self.ERROR_RRG_CONNECT_FAILED
        except Exception as e:
            # Log exception details if needed.
            self._rrg = None
            return self.ERROR_RRG_CONNECT_FAILED

    def TurnOff(self) -> int:
        """
        @brief Closes the connection to the RRG device.
        @return RRG_OK on success, or ERROR_RRG_NOT_CONNECTED if no connection exists.
        @details
        This method shuts down the connection to the RRG device and releases the associated resources.
        """
        if self._rrg is None:
            return self.ERROR_RRG_NOT_CONNECTED

        try:
            self._rrg.close()
            self._rrg = None
            return self.RRG_OK
        except Exception:
            return self.ERROR_RRG_NOT_CONNECTED

    def SetFlow(self, setpoint: float) -> int:
        """
        @brief Sends a new flow setpoint to the RRG device.
        @param setpoint The desired flow setpoint (e.g., in SCCM).
        @return RRG_OK on success, or an error code if the operation fails.
        @details
        This method checks that the device is connected and then instructs the RRG to
        set the specified flow.
        """
        if self._rrg is None:
            return self.ERROR_RRG_NOT_CONNECTED

        try:
            if self._rrg.set_flow(setpoint):
                return self.RRG_OK
            else:
                return self.ERROR_RRG_SET_FLOW_FAILED
        except Exception:
            return self.ERROR_RRG_SET_FLOW_FAILED

    def GetFlow(self):
        """
        @brief Retrieves the current flow rate from the RRG device.
        @return A tuple (error_code, flow_value). On success, error_code is RRG_OK and flow_value
                contains the current flow; on failure, error_code is non-zero and flow_value is -1.0.
        @details
        This method checks that the device is connected and queries it for the current flow rate.
        """
        if self._rrg is None:
            return (self.ERROR_RRG_NOT_CONNECTED, -1.0)

        try:
            flow = self._rrg.get_flow()
            if flow < 0:
                return (self.ERROR_RRG_GET_FLOW_FAILED, -1.0)
            return (self.RRG_OK, flow)
        except Exception:
            return (self.ERROR_RRG_GET_FLOW_FAILED, -1.0)
