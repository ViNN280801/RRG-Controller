#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "rrg.h"

#define INPUT_BUFFER_SIZE 32

/**
 * @brief Function to dynamically scan available serial ports.
 * @return A valid serial port string (e.g., "/dev/ttyUSB0") or NULL if no active ports found.
 */
char *get_active_serial_port()
{
    static char port[32];

    FILE *fp = popen("ls /dev/ttyUSB* 2>/dev/null", "r"); // Get active USB serial devices (Linux)
    if (!fp)
    {
        printf("Error: Unable to execute system command.\n");
        return NULL;
    }

    if (fgets(port, sizeof(port), fp) != NULL)
    {
        port[strcspn(port, "\n")] = '\0'; // Remove newline character
        pclose(fp);
        return port;
    }

    pclose(fp);
    return NULL;
}

int main()
{
    char *port;
    RRG_Config config;
    RRG_Handle handle;
    char input[INPUT_BUFFER_SIZE];

    printf("Scanning for active serial ports...\n");

    while (1)
    {
        // Get an available active serial port
        port = get_active_serial_port();
        if (!port)
        {
            printf("No active serial ports found. Retrying...\n");
            sleep(2);
            continue;
        }

        // Configure the RRG connection
        config.port = port;
        config.baudrate = CONST_RRG_DEFAULT_BAUDRATE;
        config.slave_id = 1;
        config.timeout = CONST_RRG_DEFAULT_TIMEOUT_MS;

        printf("Found active port: %s\n", port);

        // Try to establish connection
        if (RRG_Init(&config, &handle) != RRG_OK)
        {
            printf("Failed to connect: %s\n", RRG_GetLastError());
            sleep(2);
            continue;
        }

        printf("Connected successfully to %s!\n", port);
        break;
    }

    // Main loop for user input
    while (1)
    {
        printf("\nEnter flow setpoint (or type 'exit' to quit): ");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = '\0'; // Remove newline

        if (strcmp(input, "exit") == 0)
        {
            printf("Exiting...\n");
            break;
        }

        float setpoint = atof(input);
        if (setpoint < 0)
        {
            printf("Invalid flow value. Must be a positive number.\n");
            continue;
        }

        // Set the flow value
        if (RRG_SetFlow(&handle, setpoint) == RRG_OK)
        {
            printf("Flow successfully set to %.3f SCCM\n", setpoint);
        }
        else
        {
            printf("Error setting flow: %s\n", RRG_GetLastError());
        }
    }

    // Cleanup
    RRG_Close(&handle);
    return 0;
}
