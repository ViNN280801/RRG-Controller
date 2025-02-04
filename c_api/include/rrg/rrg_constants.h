#ifndef RRG_CONSTANTS_H
#define RRG_CONSTANTS_H

/**
 * @def RRG_DEFAULT_PARITY
 * @brief Default parity configuration for MODBUS-RTU communication.
 */
#define RRG_DEFAULT_PARITY 'N'

/**
 * @def RRG_DEFAULT_BAUDRATE
 * @brief Default baudrate for the MODBUS-RTU communication for the RRG.
 */
#define RRG_DEFAULT_BAUDRATE 38400

/**
 * @def RRG_DEFAULT_SLAVE_ID
 * @brief Default slave ID for the MODBUS-RTU communication for the RRG.
 */
#define RRG_DEFAULT_SLAVE_ID 1

/**
 * @def RRG_DEFAULT_DATA_BITS
 * @brief Default number of data bits for MODBUS-RTU communication.
 */
#define RRG_DEFAULT_DATA_BITS 8

/**
 * @def RRG_DEFAULT_STOP_BITS
 * @brief Default number of stop bits for MODBUS-RTU communication.
 */
#define RRG_DEFAULT_STOP_BITS 1

/**
 * @def RRG_DEFAULT_TIMEOUT_MS
 * @brief Default timeout in milliseconds for MODBUS-RTU communication.
 */
#define RRG_DEFAULT_TIMEOUT_MS 50

/**
 * @def MODBUS_REGISTER_SETPOINT
 * @brief MODBUS register for setting the flow setpoint (2053-2054).
 */
#define MODBUS_REGISTER_SETPOINT 2053

/**
 * @def MODBUS_REGISTER_FLOW
 * @brief MODBUS register for reading the current flow (2103).
 */
#define MODBUS_REGISTER_FLOW 2103

/**
 * @def MODBUS_REGISTER_GAS
 * @brief MODBUS register for setting the gas type (2100).
 */
#define MODBUS_REGISTER_GAS 2100

#endif // !RRG_CONSTANTS_H
