#include "sizes.h"

namespace gfx {

static sizes_t _getSizes(Reader reader) {
    uint16_t wh[2];  // width, height
    reader.read(wh);
    return sizes_t{wh[0], wh[1]};
}
static sizes_t _imageSizes(Reader reader) {
    reader.read();  // skip type
    return _getSizes(reader);
}

uint8_t iconWidth(gfxicon_t *icon, bool pgm) {
    return pgm ? pgm_read_byte(icon) : *icon;
}

sizes_t imageSizes(gfximage_t *img, bool pgm) {
    return _imageSizes(Reader(img, pgm));
}
sizes_t bitmapSizes(gfxmap_t *bitmap, bool pgm) {
    return _getSizes(Reader(bitmap, pgm));
}
sizes_t bitpackSizes(gfxpack_t *bitpack, bool pgm) {
    return _getSizes(Reader(bitpack, pgm));
}

}