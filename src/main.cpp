// pico sdk
#include <hardware/adc.h>
#include <hardware/gpio.h>

// arduino libraries
#include <Arduino.h>
#include <FastLED.h>
#include <tusb.h>

// FreeRTOS
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

// My Stuff
#include "Controller.h"
#include "Poti.h"
#include "Button.h"
#include "Joystick.h"

// define hardware
hardware::Poti poti_left(26, 0);
hardware::Poti poti_right(27, 1);
hardware::Button btn(10, hardware::InputLogic::HighActive);
hardware::Joystick joystick({
    .left   = hardware::Button(11, hardware::InputLogic::HighActive),
    .right  = hardware::Button(12, hardware::InputLogic::HighActive),
    .top    = hardware::Button(13, hardware::InputLogic::HighActive),
    .bottom = hardware::Button(14, hardware::InputLogic::HighActive),
});

// serial communication
uint8_t buffer[10*1024];
com::SerialLink serial_link(Serial);
Controller controller(serial_link, poti_left, poti_right, btn, joystick);

// tasks
static TaskHandle_t tsk_blinky;
static TaskHandle_t tsk_input;

static void blinky_entry(void *param);
static void input_entry(void *param);

void setup()
{
    // init hardware
    Serial1.begin(115200);

    adc_init();

    gpio_init(13);
    gpio_set_dir(13, true);

    // add tasks
    xTaskCreate(blinky_entry, "Blinky", 256, NULL, 0, &tsk_blinky);
    xTaskCreate(input_entry, "Input", 256, NULL, 0, &tsk_input);

    controller.SetFwVersion(VERSION_MAJOR, VERSION_MINOR);

    // set buffer
    serial_link.StartReceiving(buffer, sizeof(buffer));
}

void loop()
{
    size_t len = serial_link.HandleIncoming();
}

static void blinky_entry(void *param)
{
    gpio_init(LED_BUILTIN);
    gpio_set_dir(LED_BUILTIN, true);

    while(1) {
        gpio_put(LED_BUILTIN, true);
        vTaskDelay(pdMS_TO_TICKS(100));

        gpio_put(LED_BUILTIN, false);
        vTaskDelay(pdMS_TO_TICKS(900));
    }
}

static void input_entry(void *param)
{
    // clear events
    btn.Update();
    btn.GetEvent();

    joystick.Update();
    joystick.GetEvent();

    while (1)
    {
        controller.UpdateInput();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}