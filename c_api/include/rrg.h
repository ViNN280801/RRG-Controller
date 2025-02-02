#ifndef RRG_LIB_H
#define RRG_LIB_H

#include "rrg_constants.h"
#include "rrg_errors.h"
#include "rrg_preprocessor_macros.h"

RRG_BEGIN_DECLS

/**
 * @struct RRG_Config
 * @brief Structure containing essential parameters for establishing a
 * connection with the gas flow regulator via MODBUS-RTU.
 */
typedef struct
{
    char *port;   ///< Serial port (e.g., "/dev/ttyUSB0" on Linux or "COM3" on Windows).
    int baudrate; ///< Baud rate for serial communication (e.g., 9600, 19200, 38400).
    int slave_id; ///< MODBUS device ID of the gas regulator (default is often 1).
    int timeout;  ///< Timeout for response (in milliseconds).
} RRG_Config;

/**
 * @struct RRG_Handle
 * @brief Internal handle that stores the communication context with the gas
 * regulator.
 */
typedef struct
{
    void *modbus_ctx; ///< Pointer to the libmodbus context used for communication.
} RRG_Handle;

/**
 * @brief Initializes and establishes a connection to the gas flow regulator.
 *
 * This function sets up a MODBUS-RTU connection over the specified serial port,
 * configures the communication settings, and attempts to connect to the device.
 *
 * @param config Pointer to an `RRG_Config` structure containing connection
 * parameters.
 * @param handle Pointer to an `RRG_Handle` structure that will be populated
 * upon success.
 * @return Returns `RRG_OK` if the connection is successfully established,
 * otherwise an error code.
 */
RRG_API int RRG_Init(const RRG_Config *RRG_RESTRICT config, RRG_Handle *RRG_RESTRICT handle);

/**
 * @brief Sends a new flow rate setpoint to the gas regulator.
 *
 * The setpoint value determines the desired gas flow rate, which the regulator
 * will attempt to maintain. The value is provided in SCCM (Standard Cubic
 * Centimeters per Minute).
 *
 * @param handle Pointer to an initialized `RRG_Handle` structure.
 * @param setpoint Desired gas flow rate in SCCM.
 * @return Returns `RRG_OK` on success, or an error code if the command fails.
 */
RRG_API int RRG_SetFlow(RRG_Handle *RRG_RESTRICT handle, float setpoint) RRG_HOT;

/**
 * @brief Retrieves the current measured gas flow rate.
 *
 * This function queries the gas flow regulator for the real-time flow rate
 * measurement and returns the value in SCCM.
 *
 * @param handle Pointer to an initialized `RRG_Handle` structure.
 * @param flow Pointer to a float variable where the retrieved flow value will
 * be stored.
 * @return Returns `RRG_OK` on success, or an error code if the request fails.
 */
RRG_API int RRG_GetFlow(RRG_Handle *RRG_RESTRICT handle, float *RRG_RESTRICT flow) RRG_HOT;

/**
 * @brief Selects the gas type for the regulator.
 *
 * The regulator supports multiple pre-configured gases. This function allows
 * selecting the active gas calibration profile.
 *
 * @param handle Pointer to an initialized `RRG_Handle` structure.
 * @param gas_id Integer representing the gas type (e.g., 7 for Helium).
 * @return Returns `RRG_OK` if the gas type is successfully set, otherwise an
 * error code.
 */
RRG_API int RRG_SetGas(RRG_Handle *RRG_RESTRICT handle, int gas_id);

/**
 * @brief Performs a tare operation on the gas flow sensor.
 *
 * The tare function zeroes the sensor readings to correct for drift or residual
 * gas flow. This is particularly useful after installation or when
 * environmental conditions change.
 *
 * @param handle Pointer to an initialized `RRG_Handle` structure.
 * @return Returns `RRG_OK` on success, or an error code if the operation fails.
 */
RRG_API int RRG_Tare(RRG_Handle *RRG_RESTRICT handle);

/**
 * @brief Closes the connection to the gas regulator and frees resources.
 *
 * This function terminates the MODBUS-RTU communication session and releases
 * any allocated memory or handles.
 *
 * @param handle Pointer to an initialized `RRG_Handle` structure.
 */
RRG_API void RRG_Close(RRG_Handle *RRG_RESTRICT handle);

/**
 * @brief Retrieves the description of the last occurred error.
 *
 * This function provides a human-readable description of the last error
 * encountered in the RRG API. It is useful for debugging and logging purposes.
 *
 * @return A string containing the error message.
 */
RRG_API const char *RRG_GetLastError() RRG_PURE;

RRG_END_DECLS

#endif // !RRG_LIB_H
