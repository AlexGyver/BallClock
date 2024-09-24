#pragma once
#include <Arduino.h>

#include "reader.h"

// len, data_0, ...data_n
typedef const uint8_t gfxicon_t;

// widthLSB, widthMSB, heightLSB, heightMSB, lenLSB, lenMSB, data_0, ...data_n
typedef const uint8_t gfxpack_t;

// widthLSB, widthMSB, heightLSB, heightMSB, data_0, ...data_n
typedef const uint8_t gfxmap_t;

// 0 map | 1 pack, ..., data_0, ...data_n
typedef const uint8_t gfximage_t;

namespace gfx {

struct sizes_t {
    uint16_t width, height;
};

// получить размеры image
uint8_t iconWidth(gfxicon_t *icon, bool pgm = true);

// получить размеры image
sizes_t imageSizes(gfximage_t *img, bool pgm = true);

// получить размеры bitmap
sizes_t bitmapSizes(gfxmap_t *bitmap, bool pgm = true);

// получить размеры bitpack
sizes_t bitpackSizes(gfxpack_t *bitpack, bool pgm = true);

}  // namespace  gfx