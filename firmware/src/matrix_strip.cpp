#include <Adafruit_NeoPixel.h>

#include "config.h"
#include "matrix.h"

static Adafruit_NeoPixel strip(MX_LED_AMOUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

void BallMatrix::setLED(uint8_t idx, uint32_t color) {
    strip.setPixelColor(idx, color);
}

uint32_t BallMatrix::getLED(uint8_t idx) {
    return strip.getPixelColor(idx);
}

void BallMatrix::begin() {
    strip.begin();
}

void BallMatrix::update() {
    strip.show();
}

void BallMatrix::setBright(uint8_t bright) {
    strip.setBrightness(bright);
}

void BallMatrix::clear() {
    strip.clear();
}