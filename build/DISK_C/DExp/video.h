#include <dos.h>

#include "bmpfile.h"
#include "defs.h"

#define VIDEO_MODE_TEXT 0x03
#define VIDEO_MODE_VESA 0x4f02
#define VIDEO_VESA_320x200_32 0x10f

#define VIDEO_PAGE_SIZE 0xFFFF

#define SCREEN_WIDTH 320l
#define SCREEN_HEIGHT 200l

#define RGB(r, g, b) \
  ((u32)(((u32)0 << 24) + ((u32)r << 16) + ((u32)g << 8) + (b)))
#define RGBA(r, g, b, a) \
  ((u32)(((u32)a << 24) + ((u32)r << 16) + ((u32)g << 8) + (b)))

void setVideoMode(u16 mode, u16 type);

void setVideoPage(u16 page);

void putpixel(int x, int y, u16 c);

void writeVideobuf(u16 offset, void *src, u32 size);

void setVideoPixel(u16 x, u16 y, u32 color);

void writeVideoWindow(void *src, u16 x1, u16 y1, u16 x2, u16 y2);

void writeVideoBMP(u16 x, u16 y, bmpfile_t *bmp);

u16 getVideoPixel(u16 x, u16 y);