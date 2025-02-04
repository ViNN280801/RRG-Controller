#ifndef RELAY_H
#define RELAY_H

#include "relay_constants.h"
#include "relay_errors.h"
#include "relay_preprocessor_macros.h"

#define RELAY_CHECK_PTR(ptr, checking_result)           \
    if (!ptr)                                           \
    {                                                   \
        RELAY_DEBUG_MSG("Detected NULL pointer")        \
        _setGlobalError(ERROR_RELAY_INVALID_PARAMETER); \
        checking_result = RELAY_ERR;                    \
    }                                                   \
    else                                                \
        checking_result = RELAY_OK;

#define RELAY_CHECK_PTR_WITH_RETURN(ptr)      \
    do                                        \
    {                                         \
        int checking_result = RELAY_OK;       \
        RELAY_CHECK_PTR(ptr, checking_result) \
        if (checking_result == RELAY_ERR)     \
            return checking_result;           \
    } while (0)

RELAY_BEGIN_DECLS

/**
 * @struct Relay_Config
 * @brief Structure containing essential parameters for establishing a
 * connection with the gas flow regulator via MODBUS-RTU.
 */
typedef struct
{
    char *port;   ///< Serial port (e.g., "/dev/ttyUSB0" on Linux or "COM3" on Windows).
    int baudrate; ///< Baud rate for serial communication (e.g., 9600, 19200, 38400).
    int slave_id; ///< MODBUS device ID of the gas regulator (default is often 1).
    int timeout;  ///< Timeout for response (in milliseconds).
} Relay_Config;

/**
 * @struct Relay_Handle
 * @brief Internal handle that stores the communication context with the gas
 * regulator.
 */
typedef struct
{
    void *modbus_ctx; ///< Pointer to the libmodbus context used for communication.
} Relay_Handle;

RELAY_API int RELAY_Init(const Relay_Config *RELAY_RESTRICT config, Relay_Handle *RELAY_RESTRICT handle);

RELAY_API int RELAY_TurnOn(Relay_Handle *RELAY_RESTRICT handle);

RELAY_API int RELAY_TurnOff(Relay_Handle *RELAY_RESTRICT handle);

RELAY_API void RELAY_Close(Relay_Handle *RELAY_RESTRICT handle);

RELAY_API const char *RELAY_GetLastError() RELAY_PURE;

RELAY_END_DECLS

#endif // !RELAY_H
