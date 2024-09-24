# pip install freetype-py
from freetype import *
from PIL import Image
import math
import sys

face = "arial.ttf"
font_height = 20
threshold = 0.9
scale_f = 85
base_text = "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~АБВГДЕЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯабвгдежзийклмнопрстуфхцчшщъыьэюяЁё"
text = base_text

if len(sys.argv) >= 5:
    text = ""

for i, arg in enumerate(sys.argv):
    match i:
        case 1:
            face = arg
        case 2:
            font_height = int(arg)
        case 3:
            threshold = float(arg)
        case _:
            if len(arg) == 1:
                text += arg
            elif len(arg) == 3:
                char_from = base_text.index(arg[0])
                char_to = base_text.index(arg[2])
                text += base_text[char_from : char_to + 1]

# process
font = Face(face)
font.set_char_size(font_height * scale_f)
height, baseline, width = 0, 0, 0
for c in text:
    font.load_char(c)
    bitmap = font.glyph.bitmap
    height = max(height, bitmap.rows + max(0, -(font.glyph.bitmap_top - bitmap.rows)))
    baseline = max(baseline, max(0, -(font.glyph.bitmap_top - bitmap.rows)))
    width += bitmap.width

index = []
rows = []
i = 0
for c in text:
    print(str(round(100 * i / len(text))) + "%")
    i += 1
    font.load_char(c)
    bitmap = font.glyph.bitmap
    glyph_height = bitmap.rows
    offset = height - baseline - font.glyph.bitmap_top
    if offset < 0:
        glyph_height += offset
        offset = 0

    chunks = [offset, bitmap.width, glyph_height]
    count, value, shift = 0, 0, 0

    def push():
        global chunks
        global count
        global value
        global shift

        chunk = (count << 1) | value

        if shift == 0:
            chunks.append(chunk << 2)
        elif shift == 1:
            chunks[-1] |= chunk >> 4
            chunks.append((chunk << 4) & 0b11110000)
        elif shift == 2:
            chunks[-1] |= chunk >> 2
            chunks.append((chunk << 6) & 0b11000000)
        elif shift == 3:
            chunks[-1] |= chunk

        shift += 1
        if shift == 4:
            shift = 0

    def process(v):
        global value
        global count

        if count == 0:
            count = 1
            value = v
        else:
            if value == v:
                count += 1
                if count == 31:
                    push()
                    count = 0
            else:
                push()
                value = v
                count = 1

    for x in range(bitmap.width):
        for y in range(glyph_height):
            process(int(bitmap.buffer[y * bitmap.width + x] > (1.0 - threshold) * 255))

    if count != 0:
        push()
    rows.append(chunks)

count = 0
for row in rows:
    index.append(count)
    count += len(row)
index.append(count)

# export
fname = face[: face.index(".")].replace(" ", "_") + "_" + str(font_height)
total = count + len(index) * 2 + len(text) + 2

index_txt = ""
for i, val in enumerate(index):
    if (i and i % 16 == 0): index_txt += "\n\t"
    index_txt += hex(val) + ", "

rows_txt = ""
for i, row in enumerate(rows):
    if i > 0:
        rows_txt += "\n\t"
    for num in row:
        rows_txt += hex(num) + ", "
    rows_txt += f"\t// {text[i]} ({i})"

index_index_txt = ""
for i, t in enumerate(text):
    if (i and i % 16 == 0): index_index_txt += "\n\t"
    index_index_txt += hex(base_text.index(t)) + ", "

ranges = ""
for r in sys.argv[4:]:
    ranges += "'" + r + "'" + ' '

# offset, width, height, chunk0, ..chunkN

file = f"""#pragma once
#include <FontGFX.h>

// {fname}
// {face} {font_height} {threshold} {ranges}
// glyphs: {text}
// {total} bytes

static const uint8_t _{fname}_index_index[] PROGMEM = {{
    {index_index_txt}
}};

static const uint16_t _{fname}_index[] PROGMEM = {{
    {index_txt}
}};

static const uint8_t _{fname}_bitpack[] PROGMEM = {{
    {rows_txt}
}};


const gfx_pack_font_t {fname} PROGMEM = {{_{fname}_index_index, _{fname}_index, _{fname}_bitpack, {height}, {len(text)}}};
"""

f = open(fname + ".h", "w", encoding="utf-8")
f.write(file)
f.close()

print(f"Done! {total} Bytes")

exit()

# space = round(font_height / 10)
# img = Image.new("1", (width + len(text) * space, height), "white")
# pixels = img.load()
# xx = 0
# for c in text:
#     font.load_char(c)
#     bitmap = font.glyph.bitmap
#     arr = bytearray(height * bitmap.width)
#     glyph_height = bitmap.rows
#     offset = height - baseline - font.glyph.bitmap_top

#     if offset < 0:
#         glyph_height += offset
#         offset = 0

#     for y in range(glyph_height):
#         for x in range(bitmap.width):
#             if bitmap.buffer[y * bitmap.width + x] > (1.0 - threshold) * 255:
#                 pixels[xx + x, offset + y] = 0

#     xx += bitmap.width + space

# img.show()

# exit()


# def show_char(c):
#     font.load_char(c)
#     bitmap = font.glyph.bitmap
#     render = ""
#     for y in range(bitmap.rows):
#         for x in range(bitmap.width):
#             render += (
#                 "⬛"
#                 if bitmap.buffer[y * bitmap.width + x] > (1.0 - threshold) * 255
#                 else "⬜"
#             )
#         render += "\n"
#     print(render)


# show_char("b")


# def render_char(char):
#     global rows

#     row = rows[base_text.index(char)]
#     data = row[3:]
#     w = row[1]
#     h = row[2]
#     buffer = bytearray(w * h)
#     x = 0
#     y = 0
#     i = 0
#     shift = 0
#     while i < len(data):
#         chunk = 0
#         match (shift):
#             case 0:
#                 chunk = data[i] >> 2
#             case 1:
#                 if i + 1 == len(data):
#                     break
#                 chunk = (data[i] << 4) & 0b00110000
#                 i += 1
#                 chunk |= data[i] >> 4
#             case 2:
#                 if i + 1 == len(data):
#                     break
#                 chunk = (data[i] << 2) & 0b00111100
#                 i += 1
#                 chunk |= data[i] >> 6
#             case 3:
#                 chunk = data[i] & 0b00111111
#                 i += 1

#         shift += 1
#         if shift == 4:
#             shift = 0

#         val = chunk & 1
#         size = chunk >> 1
#         while size:
#             if val:
#                 buffer[y * w + x] = 1
#             y += 1
#             if y == h:
#                 y = 0
#                 x += 1
#             size -= 1

#     render = ""
#     for y in range(h):
#         for x in range(w):
#             render += "⬛" if buffer[y * w + x] else "⬜"
#         render += "\n"

#     print(render)


# render_char("b")
