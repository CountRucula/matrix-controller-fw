#include <Arduino.h>
#include <FastLED.h>

#include "MatrixLink.hpp"
#include "MatrixCom.hpp"

#define N_LEDS 10
#define LED_PIN 15

CRGB leds[N_LEDS];

matrix::MatrixLink matrixlink(Serial);
matrix::MatrixCOM matrix_com(matrixlink);
uint8_t buffer[1024];

matrix::FrameType_t frameType;
uint8_t *payload;
size_t payload_length;

void bind_com_signals(void);

void setup()
{
    // put your setup code here, to run once:
    Serial.begin(9600);

    // add leds
    FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, N_LEDS);
    FastLED.show();

    // add calbacks
    bind_com_signals();

    // set buffer
    matrixlink.StartReceiving(buffer, sizeof(buffer));
}

void loop()
{
    // handle incoming data
    size_t len = matrixlink.HandleIncoming();

    // len > 0 -> frame successful received
    if (len > 0)
    {
        frameType = matrixlink.GetType();
        payload = matrixlink.GetPayload(payload_length);

        matrix_com.HandleFrame(frameType, payload, payload_length);
    }

    // sleep for 10ms
    delay(10);
}

void bind_com_signals(void)
{
    matrix_com.Signals().SetBaudrate.connect([](uint32_t baudrate) {
        Serial.updateBaudRate(baudrate);
    });

    matrix_com.Signals().GetFwVersion.connect([]() {
        matrix_com.RespondGetFwVersion(1, 5);
    });

    matrix_com.Signals().GetSize.connect([]() {
        matrix_com.RespondGetSize(10, 1);
    });

    matrix_com.Signals().ClrFrame.connect([]() {
        for(int i = 0; i < N_LEDS; i++) {
            leds[i] = 0;
        }

        FastLED.show();
    });

    matrix_com.Signals().LedData.connect([](uint8_t *values, size_t n_leds){
        n_leds = MIN(n_leds, N_LEDS);

        for(size_t i = 0; i < n_leds; i++) {
            leds[i].r = values[3*i + 0];
            leds[i].g = values[3*i + 1];
            leds[i].b = values[3*i + 2];
        }

        FastLED.show();
    });
}