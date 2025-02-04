#ifndef RELAY_ERRORS_H
#define RELAY_ERRORS_H

/**
 * @def MODBUS_ERR
 * @brief General error code for libmodbus failures.
 */
#define MODBUS_ERR -1

/**
 * @brief Global variable to store the last occurred error code.
 */
extern int RELAY_GlobalError;

/**
 * @def RELAY_OK
 * @brief No error occurred.
 */
#define RELAY_OK 0

/**
 * @def RELAY_ERR
 * @brief General error. Base to compose other error types.
 */
#define RELAY_ERR -1

/**
 * @def ERROR_RELAY_FAILED_CONNECT
 * @brief Connection to the MODBUS device failed.
 */
#define ERROR_RELAY_FAILED_CONNECT -6001

/**
 * @def ERROR_RELAY_FAILED_CREATE_CONTEXT
 * @brief Failed to create a MODBUS-RTU context.
 */
#define ERROR_RELAY_FAILED_CREATE_CONTEXT -6002

/**
 * @def ERROR_RELAY_FAILED_SET_SLAVE
 * @brief Failed to set MODBUS slave ID.
 */
#define ERROR_RELAY_FAILED_SET_SLAVE -6003

/**
 * @def ERROR_RELAY_FAILED_SET_TIMEOUT
 * @brief Failed to set MODBUS response timeout.
 */
#define ERROR_RELAY_FAILED_SET_TIMEOUT -6004

/**
 * @def ERROR_RELAY_FAILED_WRITE_REGISTER
 * @brief Failed to write a MODBUS register.
 */
#define ERROR_RELAY_FAILED_WRITE_REGISTER -6005

/**
 * @def ERROR_RELAY_INVALID_PARAMETER
 * @brief An invalid parameter was passed to the function.
 */
#define ERROR_RELAY_INVALID_PARAMETER -6006

/// @brief Resets the global 'RELAY_GlobalError' to the status OK.
static inline void _resetGlobalError() { RELAY_GlobalError = RELAY_OK; }

/// @brief Sets the global 'RELAY_GlobalError' to the specified error status.
static inline void _setGlobalError(int error_code) { RELAY_GlobalError = error_code; }

#endif // !RELAY_ERRORS_H
