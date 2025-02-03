#ifndef RRG_ERRORS_H
#define RRG_ERRORS_H

/**
 * @def MODBUS_ERR
 * @brief General error code for libmodbus failures.
 */
#define MODBUS_ERR -1

/**
 * @brief Global variable to store the last occurred error code.
 */
extern int RRG_GlobalError;

/**
 * @def RRG_OK
 * @brief No error occurred.
 */
#define RRG_OK 0

/**
 * @def RRG_ERR
 * @brief General error. Base to compose other error types.
 */
#define RRG_ERR -1

/**
 * @def ERROR_RRG_FAILED_CONNECT
 * @brief Connection to the MODBUS device failed.
 */
#define ERROR_RRG_FAILED_CONNECT -1001

/**
 * @def ERROR_RRG_FAILED_CREATE_CONTEXT
 * @brief Failed to create a MODBUS-RTU context.
 */
#define ERROR_RRG_FAILED_CREATE_CONTEXT -1002

/**
 * @def ERROR_RRG_FAILED_SET_SLAVE
 * @brief Failed to set MODBUS slave ID.
 */
#define ERROR_RRG_FAILED_SET_SLAVE -1003

/**
 * @def ERROR_RRG_FAILED_SET_TIMEOUT
 * @brief Failed to set MODBUS response timeout.
 */
#define ERROR_RRG_FAILED_SET_TIMEOUT -1004

/**
 * @def ERROR_RRG_FAILED_READ_REGISTER
 * @brief Failed to read a MODBUS register.
 */
#define ERROR_RRG_FAILED_READ_REGISTER -1005

/**
 * @def ERROR_RRG_FAILED_WRITE_REGISTER
 * @brief Failed to write a MODBUS register.
 */
#define ERROR_RRG_FAILED_WRITE_REGISTER -1006

/**
 * @def ERROR_RRG_INVALID_PARAMETER
 * @brief An invalid parameter was passed to the function.
 */
#define ERROR_RRG_INVALID_PARAMETER -1007

/// @brief Resets the global 'RRG_GlobalError' to the status OK.
static inline void _resetGlobalError() { RRG_GlobalError = RRG_OK; }

/// @brief Sets the global 'RRG_GlobalError' to the specified error status.
static inline void _setGlobalError(int error_code) { RRG_GlobalError = error_code; }

#endif // !RRG_ERRORS_H
