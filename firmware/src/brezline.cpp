#include "brezline.h"

void brezLine(int x0, int y0, int x1, int y1, bool add, void* ctx, void (*set)(int x, int y, void* ctx)) {
    int dx = x1 - x0;
    int dy = y1 - y0;
    int sx = 1, sy = 1;
    if (dx < 0) dx = -dx, sx = -1;
    if (dy < 0) dy = -dy, sy = -1;
    int dx2 = dx * 2;
    int dy2 = dy * 2;
    set(x0, y0, ctx);
    if (dx > dy) {
        int err = dy2 - dx;
        while (x0 != x1) {
            x0 += sx;
            if (err >= 0) {
                y0 += sy;
                err -= dx2;
                if (add) set(x0 - sx, y0, ctx);
            }
            err += dy2;
            set(x0, y0, ctx);
        }
    } else {
        int err = dx2 - dy;
        while (y0 != y1) {
            y0 += sy;
            if (err >= 0) {
                x0 += sx;
                err -= dy2;
                if (add) set(x0, y0 - sy, ctx);
            }
            err += dx2;
            set(x0, y0, ctx);
        }
    }
}