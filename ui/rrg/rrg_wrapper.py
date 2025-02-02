# -*- coding: utf-8 -*-

import os
import sys
import ctypes
from ctypes import CDLL, POINTER, c_char_p, c_int, c_float, c_void_p

# Determine the library filename based on the platform.
if os.name == "nt":
    lib_filename = "rrg.dll"
else:
    lib_filename = "librrg.so"

# Build the path relative to the current file's directory.
# Assuming this code is in ui/rrg/rrg_wrapper.py, go up one directory and into "resources".
current_dir = os.path.dirname(os.path.abspath(__file__))
lib_path = os.path.join(current_dir, "..", "resources", lib_filename)

# Normalize the path.
lib_path = os.path.abspath(lib_path)

# Optionally print the library path for debugging.
print("Loading shared library from:", lib_path)

# Load the shared library.
try:
    rrg_lib = CDLL(lib_path)
except OSError as e:
    sys.exit(f"Failed to load shared library: {e}")


# --- Define RRGConfig structure ---
class RRGConfig(ctypes.Structure):
    """Represents the configuration parameters for establishing an RRG connection."""

    _fields_ = [
        ("port", c_char_p),  # Serial port (e.g., "COM3" or "/dev/ttyUSB0")
        ("baudrate", c_int),  # Baud rate (e.g., 9600, 19200, 38400)
        ("slave_id", c_int),  # MODBUS slave ID
        ("timeout", c_int),  # Timeout in milliseconds
    ]


# --- Interface for RRG operations ---
class IRRG:
    """Interface defining the methods for interacting with the RRG."""

    def connect(self) -> bool:
        """Establishes a connection to the RRG device."""
        raise NotImplementedError

    def set_flow(self, setpoint: float) -> bool:
        """Sends a new flow setpoint to the RRG."""
        raise NotImplementedError

    def get_flow(self) -> float:
        """Retrieves the current flow rate from the RRG."""
        raise NotImplementedError

    def set_gas(self, gas_id: int) -> bool:
        """Sets the gas type in the RRG."""
        raise NotImplementedError

    def tare(self) -> bool:
        """Performs a tare operation on the RRG."""
        raise NotImplementedError

    def close(self) -> None:
        """Closes the connection to the RRG device."""
        raise NotImplementedError

    def get_last_error(self) -> str:
        """Retrieves the last occurred error message."""
        raise NotImplementedError


# --- Implementation of the RRG Wrapper ---
class RRG(IRRG):
    """Python wrapper for the RRG C API."""

    def __init__(
        self, port: str, baudrate: int = 9600, slave_id: int = 1, timeout: int = 1000
    ):
        """
        Initializes an RRG instance with the given connection parameters.

        :param port: Serial port name (e.g., "COM3" or "/dev/ttyUSB0").
        :param baudrate: Baud rate for communication.
        :param slave_id: MODBUS slave ID of the device.
        :param timeout: Response timeout in milliseconds.
        """
        self._config = RRGConfig(port.encode("utf-8"), baudrate, slave_id, timeout)
        self._handle = c_void_p()  # Handle storing the MODBUS context
        self._setup_functions()

    def _setup_functions(self):
        """Configures ctypes function signatures for the RRG API."""
        rrg_lib.RRG_Init.argtypes = [POINTER(RRGConfig), POINTER(c_void_p)]
        rrg_lib.RRG_Init.restype = c_int

        rrg_lib.RRG_SetFlow.argtypes = [c_void_p, c_float]
        rrg_lib.RRG_SetFlow.restype = c_int

        rrg_lib.RRG_GetFlow.argtypes = [c_void_p, POINTER(c_float)]
        rrg_lib.RRG_GetFlow.restype = c_int

        rrg_lib.RRG_SetGas.argtypes = [c_void_p, c_int]
        rrg_lib.RRG_SetGas.restype = c_int

        rrg_lib.RRG_Tare.argtypes = [c_void_p]
        rrg_lib.RRG_Tare.restype = c_int

        rrg_lib.RRG_Close.argtypes = [c_void_p]
        rrg_lib.RRG_Close.restype = None

        rrg_lib.RRG_GetLastError.restype = c_char_p

    def connect(self) -> bool:
        """Establishes a connection with the RRG device."""
        result = rrg_lib.RRG_Init(
            ctypes.byref(self._config), ctypes.byref(self._handle)
        )
        return result == 0

    def set_flow(self, setpoint: float) -> bool:
        """
        Sends a new flow rate setpoint to the RRG device.

        :param setpoint: Desired flow rate in SCCM (Standard Cubic Centimeters per Minute).
        :return: True if successful, False otherwise.
        """
        result = rrg_lib.RRG_SetFlow(self._handle, c_float(setpoint))
        return result == 0

    def get_flow(self) -> float:
        """
        Retrieves the current gas flow rate from the RRG device.

        :return: The current flow rate in SCCM, or -1.0 if an error occurs.
        """
        flow = c_float()
        result = rrg_lib.RRG_GetFlow(self._handle, ctypes.byref(flow))
        return flow.value if result == 0 else -1.0

    def set_gas(self, gas_id: int) -> bool:
        """
        Sets the gas type in the RRG device.

        :param gas_id: The gas type identifier (e.g., 7 for Helium).
        :return: True if successful, False otherwise.
        """
        result = rrg_lib.RRG_SetGas(self._handle, c_int(gas_id))
        return result == 0

    def tare(self) -> bool:
        """
        Performs a tare operation on the RRG device.

        :return: True if successful, False otherwise.
        """
        result = rrg_lib.RRG_Tare(self._handle)
        return result == 0

    def close(self) -> None:
        """Closes the connection to the RRG device and releases resources."""
        if self._handle:
            rrg_lib.RRG_Close(self._handle)

    def get_last_error(self) -> str:
        """
        Retrieves the last error message from the RRG API.

        :return: A string describing the last error.
        """
        return rrg_lib.RRG_GetLastError().decode("utf-8")
