#include "video.h"

#ifndef _WIN32
char far *video_buf = (char far *)0xa0000000L;
#else
char *video_buf = (char *)0xa0000000L;
#endif
u16 old_page = 0;

void setVideoMode(u16 mode, u16 type) {
  union REGS in, out;
  in.x.ax = mode;
  in.x.bx = type;
  int86(0x10, &in, &out);
}

void setVideoPage(u16 page) {
  union REGS in, out;
  in.x.bx = 0;
  in.x.dx = page;
  in.x.ax = 0x4F05;
  int86(0x10, &in, &out);
}

void putpixel(int x, int y, u16 c) { *(video_buf + x + 320 * y) = c; }

void writeVideobuf(u16 offset, void *src, u32 size) {
  char *s;
  unsigned long i;
  s = (char *)src;
  // for (i = 0; i < size; i++)
  //   *((char far *)((void _seg *)((0xA000)) + (void near *)((i + offset)))) =
  //       *(s++);
  for (i = 0; i < size; i++) *(video_buf + i + offset) = (*s++);
}

void setVideoPixel(u16 x, u16 y, u32 color) {
  u32 temp;
  u16 pos;
  u16 page;
  u16 t;
  u8 d;
  temp = (SCREEN_WIDTH * y + x) * sizeof(u32);
  page = 0;
  while (temp >= VIDEO_PAGE_SIZE) {
    temp -= VIDEO_PAGE_SIZE;
    page++;
  }
  pos = temp;
  if (pos < 4) {
    setVideoPage(page - 1);
    for (t = pos; t < 4; t++)
      *(video_buf - 4 + t + 0xFFFF) = (color >> (8 * t)) & 0xFF;
    setVideoPage(page);
    for (t = 0; t < pos; t++)
      *(video_buf + t) = (color >> (8 * (3 - t))) & 0xFF;
  } else {
    if (pos % 4 != 0) pos = pos / 4 * 4;
    if (page != old_page) {
      old_page = page;
      setVideoPage(page);
    }
    // if (page % 2 != 0) color = ~color;
    *(video_buf + pos + 0) = color & 0xFF;
    *(video_buf + pos + 1) = (color >> 8) & 0xFF;
    *(video_buf + pos + 2) = (color >> 16) & 0xFF;
    *(video_buf + pos + 3) = (color >> 24) & 0xFF;
  }
}

void writeVideoWindow(void *src, u16 x1, u16 y1, u16 x2, u16 y2) {
  u16 x, y;
  u16 pos;
  pos = 0;
  for (x = x1; x < x2; x++) {
    for (y = y1; y < y2; y++) {
      setVideoPixel(x, y, *((unsigned int *)(src) + (pos++)));
    }
  }
}

void writeVideoBMP(u16 x, u16 y, bmpfile_t *bmp) {
  u16 i, j;
  if (!bmp || !bmp->colors) return;
  // writeVideoWindow((bmp->colors), x, y, x + bmp_get_width(bmp),
  //                  y + bmp_get_height(bmp));
  // for (i = x; i < x + bmp_get_width(bmp); i++) {
  //   for (j = y; j < y + bmp_get_height(bmp); j++) {
  //     setVideoPixel(i, j, *((u32 *)(&(bmp->pixels[i - x][j - y]))));
  //   }
  // }
  for (i = y; i < y + bmp_get_height(bmp); i++) {
    for (j = x; j < x + bmp_get_width(bmp); j++) {
      setVideoPixel(j, i, *((u32 *)(&(bmp->pixels[j - x][i - y]))));
    }
  }
}

u16 getVideoPixel(u16 x, u16 y) { return *(video_buf + x + 320 * y); }