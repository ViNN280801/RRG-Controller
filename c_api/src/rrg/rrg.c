#include <errno.h>
#include <modbus/modbus.h>
#include <stdio.h>

#include "rrg.h"
#include "rrg_constants.h"

// Global error variable definition.
int RRG_GlobalError = RRG_OK;

int RRG_Init(const RRG_Config *RRG_RESTRICT config, RRG_Handle *RRG_RESTRICT handle)
{
    // 1. Validate input parameters.
    RRG_CHECK_PTR_WITH_RETURN(config);
    RRG_CHECK_PTR_WITH_RETURN(handle);

    // 2. Initialize MODBUS-RTU context using default serial configuration.
    modbus_t *ctx = modbus_new_rtu(
        config->port, config->baudrate, CONST_RRG_DEFAULT_PARITY,
        CONST_RRG_DEFAULT_DATA_BITS, CONST_RRG_DEFAULT_STOP_BITS);
    if (unlikely(!ctx))
    {
        RRG_MODBUS_DEBUG_MSG;
        _setGlobalError(ERROR_RRG_FAILED_CREATE_CONTEXT);
        return RRG_ERR;
    }

    // 3. Set MODBUS slave ID and check for errors.
    if (unlikely(modbus_set_slave(ctx, config->slave_id) == MODBUS_ERR))
    {
        RRG_MODBUS_DEBUG_MSG;
        modbus_free(ctx);
        _setGlobalError(ERROR_RRG_FAILED_SET_SLAVE);
        return RRG_ERR;
    }

    // 4. Configure response timeout and check for errors.
    if (unlikely(modbus_set_response_timeout(ctx, 0, config->timeout * 1000) == MODBUS_ERR))
    {
        RRG_MODBUS_DEBUG_MSG;
        modbus_free(ctx);
        _setGlobalError(ERROR_RRG_FAILED_SET_TIMEOUT);
        return RRG_ERR;
    }

    // 5. Attempt to connect to the MODBUS device.
    if (modbus_connect(ctx) == MODBUS_ERR)
    {
        RRG_MODBUS_DEBUG_MSG;
        modbus_free(ctx);
        _setGlobalError(ERROR_RRG_FAILED_CONNECT);
        return RRG_ERR;
    }

    // 6. Store the context in the handle and check it on NULL.
    handle->modbus_ctx = ctx;
    RRG_CHECK_PTR_WITH_RETURN(handle->modbus_ctx);

    _resetGlobalError();
    return RRG_OK;
}

int RRG_SetFlow(RRG_Handle *RRG_RESTRICT handle, float setpoint)
{
    // 1. Validate input parameters.
    RRG_CHECK_PTR_WITH_RETURN(handle);
    RRG_CHECK_PTR_WITH_RETURN(handle->modbus_ctx);

    // 2. Convert the floating-point setpoint value to an integer representation.
    // The MODBUS protocol stores 32-bit values across two 16-bit registers.
    // - The high 16 bits (most significant) are stored in one register.
    // - The low 16 bits (least significant) are stored in another register.
    int value = (int)(setpoint * 1000), // Convert setpoint to an integer with three decimal places.
        reg_high = value >> 16,         // Extract the upper 16 bits (shift right by 16).
        reg_low = value & 0xFFFF;       // Extract the lower 16 bits (mask with 0xFFFF).

    // 3. Write setpoint to MODBUS register 2053-2054.
    if (modbus_write_register(handle->modbus_ctx, MODBUS_REGISTER_SETPOINT, reg_high) == MODBUS_ERR)
    {
        RRG_MODBUS_DEBUG_MSG;
        _setGlobalError(ERROR_RRG_FAILED_WRITE_REGISTER);
        return RRG_ERR;
    }
    if (modbus_write_register(handle->modbus_ctx, MODBUS_REGISTER_SETPOINT + 1, reg_low) == MODBUS_ERR)
    {
        RRG_MODBUS_DEBUG_MSG;
        _setGlobalError(ERROR_RRG_FAILED_WRITE_REGISTER);
        return RRG_ERR;
    }

    _resetGlobalError();
    return RRG_OK;
}

int RRG_GetFlow(RRG_Handle *RRG_RESTRICT handle, float *RRG_RESTRICT flow)
{
    // 1. Validate input parameters.
    RRG_CHECK_PTR_WITH_RETURN(handle);
    RRG_CHECK_PTR_WITH_RETURN(handle->modbus_ctx);
    RRG_CHECK_PTR_WITH_RETURN(flow);

    // 2. Read 32-bit flow value from MODBUS register 2103.
    uint16_t data[2];
    if (modbus_read_registers(handle->modbus_ctx, MODBUS_REGISTER_FLOW, 2, data) == MODBUS_ERR)
    {
        RRG_MODBUS_DEBUG_MSG;
        _setGlobalError(ERROR_RRG_FAILED_READ_REGISTER);
        return RRG_ERR;
    }

    // 3. Convert MODBUS data to float.
    *flow = ((data[0] << 16) | data[1]) / 1000.0;

    _resetGlobalError();
    return RRG_OK;
}

int RRG_SetGas(RRG_Handle *RRG_RESTRICT handle, int gas_id)
{
    // 1. Validate input parameters.
    RRG_CHECK_PTR_WITH_RETURN(handle);
    RRG_CHECK_PTR_WITH_RETURN(handle->modbus_ctx);

    // 2. Write gas ID to MODBUS register 2100.
    if (modbus_write_register(handle->modbus_ctx, MODBUS_REGISTER_GAS, gas_id) == MODBUS_ERR)
    {
        RRG_MODBUS_DEBUG_MSG;
        _setGlobalError(ERROR_RRG_FAILED_WRITE_REGISTER);
        return RRG_ERR;
    }

    _resetGlobalError();
    return RRG_OK;
}

void RRG_Close(RRG_Handle *RRG_RESTRICT handle)
{
    if (handle && handle->modbus_ctx)
    {
        modbus_close(handle->modbus_ctx);
        modbus_free(handle->modbus_ctx);
    }
}

const char *RRG_GetLastError()
{
    switch (RRG_GlobalError)
    {
    case RRG_OK:
        return "No error.";
    case ERROR_RRG_FAILED_CONNECT:
        return "Error: Connection to the MODBUS device failed.";
    case ERROR_RRG_FAILED_CREATE_CONTEXT:
        return "Error: Failed to create a MODBUS-RTU context.";
    case ERROR_RRG_FAILED_SET_SLAVE:
        return "Error: Failed to set MODBUS slave ID.";
    case ERROR_RRG_FAILED_SET_TIMEOUT:
        return "Error: Failed to set MODBUS response timeout.";
    case ERROR_RRG_FAILED_READ_REGISTER:
        return "Error: Failed to read a MODBUS register.";
    case ERROR_RRG_FAILED_WRITE_REGISTER:
        return "Error: Failed to write a MODBUS register.";
    case ERROR_RRG_INVALID_PARAMETER:
        return "Error: Invalid parameter provided to function.";
    default:
        return "Unknown error occurred.";
    }
}
