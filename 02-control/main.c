#include <stdio.h>
#include "pico/stdlib.h"
#include "stdio-task/stdio-task.h"
#include "protocol-task/protocol-task.h"
#include "led-task/led-task.h"

#define DEVICE_NAME "my-pico-device"
#define DEVICE_VRSN "v0.2.0"

void version_callback(const char* args);
void led_on_callback(const char* args);
void led_off_callback(const char* args);
void led_blink_callback(const char* args);
void led_blink_set_period_ms_callback(const char* args);
void help_callback(const char* args);
void mem_callback(const char* args);
void wmem_callback(const char* args);

api_t device_api[] =
{
    {"version",    version_callback,                  "get device name and firmware version"},
    {"on",         led_on_callback,                   "turn LED on"},
    {"off",        led_off_callback,                  "turn LED off"},
    {"blink",      led_blink_callback,                "make LED blink"},
    {"set_period", led_blink_set_period_ms_callback,  "set blink period in milliseconds"},
    {"help",       help_callback,                     "show this help"},
    {"mem",        mem_callback,                      "read 32-bit word from address (hex)"},
    {"wmem",       wmem_callback,                     "write 32-bit word to address (hex)"},
    {NULL, NULL, NULL}
};

void version_callback(const char* args) {
    printf("device name: '%s', firmware version: %s\n", DEVICE_NAME, DEVICE_VRSN);
}

void led_on_callback(const char* args) {
    led_task_state_set(LED_STATE_ON);
    printf("LED turned on\n");
}

void led_off_callback(const char* args) {
    led_task_state_set(LED_STATE_OFF);
    printf("LED turned off\n");
}

void led_blink_callback(const char* args) {
    led_task_state_set(LED_STATE_BLINK);
    printf("LED blinking\n");
}

void led_blink_set_period_ms_callback(const char* args) {
    uint32_t period_ms = 0;
    if (sscanf(args, "%u", &period_ms) != 1 || period_ms == 0) {
        printf("Error: invalid period. Usage: set_period <milliseconds>\n");
        return;
    }
    led_task_set_blink_period_ms(period_ms);
    printf("Blink period set to %u ms\n", period_ms);
}

void help_callback(const char* args) {
    printf("Available commands:\n");
    for (int i = 0; device_api[i].command_name != NULL; i++) {
        printf("  %-12s - %s\n", device_api[i].command_name, device_api[i].command_help);
    }
}

void mem_callback(const char* args) {
    uint32_t addr;
    if (sscanf(args, "%x", &addr) != 1) {
        printf("Error: invalid address. Usage: mem <hex_address>\n");
        return;
    }
    uint32_t value = *(volatile uint32_t*)addr;
    printf("Memory at 0x%08X: 0x%08X\n", addr, value);
}

void wmem_callback(const char* args) {
    uint32_t addr, value;
    if (sscanf(args, "%x %x", &addr, &value) != 2) {
        printf("Error: invalid arguments. Usage: wmem <hex_address> <hex_value>\n");
        return;
    }
    *(volatile uint32_t*)addr = value;
    printf("Written 0x%08X to 0x%08X\n", value, addr);
}


int main() {
    stdio_init_all();
    stdio_task_init();
    protocol_task_init(device_api);
    led_task_init();

    while (1) {
        char* cmd = stdio_task_handle();
        if (cmd != NULL) {
            protocol_task_handle(cmd);
        }
        led_task_handle();
    }
    return 0;
}