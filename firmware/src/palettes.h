#pragma once
#include <Arduino.h>

String getPaletteList();
uint32_t getPaletteColor(uint8_t pal, uint16_t idx, uint8_t bright);