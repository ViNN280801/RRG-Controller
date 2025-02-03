#ifndef RRG_LIB_H
#define RRG_LIB_H

#include "rrg_constants.h"
#include "rrg_errors.h"
#include "rrg_preprocessor_macros.h"

/**
 * @def RRG_CHECK_PTR(ptr, checking_result)
 * @brief Macro for validating a pointer and handling null values.
 *
 * This macro is designed to check whether a given pointer is valid (i.e., not `NULL`).
 * If the pointer is `NULL`, it logs a debug message, sets a global error variable,
 * and assigns `RRG_ERR` to the specified return value. Otherwise, it assigns `RRG_OK`.
 *
 * @note This macro should be used in functions where pointer validation is required
 *       before performing operations on the pointer. It ensures robust error handling
 *       by setting an error status and providing debugging information.
 *
 * @param[in]  ptr     The pointer to be validated. If `NULL`, an error is logged.
 * @param[out] checking_result  The variable to store the return status (`RRG_OK` or `RRG_ERR`).
 *
 * @warning Using this macro within control structures (e.g., `if` statements)
 *          without enclosing it in curly braces `{}` may cause unintended behavior
 *          due to its multi-line nature.
 *
 * @attention This macro modifies `checking_result` directly. Ensure `checking_result` is a valid
 *            variable before passing it to the macro.
 *
 * @code
 * // Example usage in a function:
 * int process_data(void *data)
 * {
 *     int status;
 *     RRG_CHECK_PTR(data, status);
 *     if (status == RRG_ERR)
 *         return status;
 *
 *     // Proceed with valid data
 *     return RRG_OK;
 * }
 * @endcode
 *
 * @see _setGlobalError(), RRG_DEBUG_MSG()
 */
#define RRG_CHECK_PTR(ptr, checking_result)           \
    if (!ptr)                                         \
    {                                                 \
        RRG_DEBUG_MSG("Detected NULL pointer")        \
        _setGlobalError(ERROR_RRG_INVALID_PARAMETER); \
        checking_result = RRG_ERR;                    \
    }                                                 \
    else                                              \
        checking_result = RRG_OK;

#define RRG_CHECK_PTR_WITH_RETURN(ptr)      \
    do                                      \
    {                                       \
        int checking_result = RRG_OK;       \
        RRG_CHECK_PTR(ptr, checking_result) \
        if (checking_result == RRG_ERR)     \
            return checking_result;         \
    } while (0)

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
