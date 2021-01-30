#include <dos.h>
// #include <graphics.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bmpfile.h"
#include "image32.c"

#define RGB(r, g, b) (((r) / 8) * 2048 + ((g) / 4) * 32 + (b) / 8)

unsigned char iOldPage;
// unsigned int *pVideoMem;

#define RGB888_RED 0x00ff0000
#define RGB888_GREEN 0x0000ff00
#define RGB888_BLUE 0x000000ff

#define RGB565_RED 0xf800
#define RGB565_GREEN 0x07e0
#define RGB565_BLUE 0x001f

unsigned short RGB888ToRGB565(unsigned int n888Color) {
  unsigned short n565Color = 0;

  // 获取RGB单色，并截取高位
  unsigned char cRed = (n888Color & RGB888_RED) >> 19;
  unsigned char cGreen = (n888Color & RGB888_GREEN) >> 10;
  unsigned char cBlue = (n888Color & RGB888_BLUE) >> 3;

  // 连接
  n565Color = (cRed << 11) + (cGreen << 5) + (cBlue << 0);
  return n565Color;
}

unsigned int RGB565ToRGB888(unsigned short n565Color) {
  unsigned int n888Color = 0;

  // 获取RGB单色，并填充低位
  unsigned char cRed = (n565Color & RGB565_RED) >> 8;
  unsigned char cGreen = (n565Color & RGB565_GREEN) >> 3;
  unsigned char cBlue = (n565Color & RGB565_BLUE) << 3;

  // 连接
  n888Color = (cRed << 16) + (cGreen << 8) + (cBlue << 0);
  return n888Color;
}

void VideoPage(int page) {
  union REGS in, out;
  in.x.bx = 0;
  in.x.dx = page;
  in.x.ax = 0x4F05;
  int86(0x10, &in, &out);
}

void SetPoint(int x, int y, unsigned int iColor) {
  unsigned long iDist, iTemp;
  unsigned int iPos;
  unsigned char iPN;
  // iDist = 800l * y + x;
  // iDist = 1024l * y + x;
  iDist = 320l * y + x;
  iTemp = iDist;
  iPN = 0;
  while (iTemp > 0xFFFF) {
    iTemp = iTemp - 0xFFFF;
    iPN++;
  }
  iPos = iTemp;
  if (iPN != iOldPage) {
    iOldPage = iPN;
    VideoPage(iPN);
  }
  // *(pVideoMem + iPos) = iColor;
  // *((unsigned int*)0xA000 + iPos) = iColor;
  pokeb(0xA000, iPos, iColor & 0xFF);
  pokeb(0xA000, iPos + 1, (iColor >> 8) & 0xFF);
  // ((char far *)((void _seg *)((0xA000)) + (void near *)((iPos))))[0] =
  //     iColor & 0xFF;
  // ((char far *)((void _seg *)((0xA000)) + (void near *)((iPos))))[1] =
  //     (iColor >> 8) & 0xFF;
  // *((char far *)((void _seg *)((0xA000)) + (void near *)((iPos)))) = iColor;
}

void far_memcpy(char far *p, void *src, unsigned long size) {
  unsigned long i;
  for (i = 0; i < size; i++)
    //*((char far *)((char *)(p) + i)) = *((char *)(src) + i);
    *((char far *)((void _seg *)((0xA000)) + (void near *)((i)))) =
        ((char *)(src))[i];
}

void write_video_buf(unsigned int offset, void *src, unsigned long size) {
  char *s;
  unsigned long i;
  s = (char *)src;
  for (i = 0; i < size; i++)
    *((char far *)((void _seg *)((0xA000)) + (void near *)((i + offset)))) =
        *(s++);
}

void write_window(void *src, unsigned int x1, unsigned int y1, unsigned int x2,
                  unsigned int y2) {
  unsigned int pos;
  unsigned char page;
  unsigned long dst, temp;
  unsigned int x, y;

  for (x = x1; x < x2; x++) {
    for (y = y1; y < y2; y++) {
      SetPoint(x, y, *((unsigned int *)(src) + (pos++)));
    }
  }
  return;

  // for (y = y1; y < y2; y++) {
  //   dst = 320l * y + x1;
  //   temp = dst;
  //   page = 0;

  //   while (temp > 0xFFFF) {
  //     temp -= 0xFFFF;
  //     page++;
  //   }
  //   pos = temp;
  //   if (page != iOldPage) {
  //     iOldPage = page;
  //     VideoPage(page);
  //   }
  //   write_video_buf(pos, (void *)((char *)(src) + (y - y1) * (x2 - x1)),
  //                   x2 - x1);
  // }
}

// p.asm 中
// extern void protect_mode();

int main() {
  // int driver = DETECT, mode = 0;
  // struct palettetype palette;
  int MaxColors;
  int MaxX;
  int MaxY;
  int error_code;
  unsigned int i, j, k;

  // 尝试进入VESA模式
  union REGS in, out;
  // 远指针
  char far *pVideoMem;
  bmpfile_t *bmp;
  // puts("starting... -> protect mode");
  // getch();
  // protect_mode();
  // puts("done in protect mode");
  // getch();

  printf("0xA000:0x0E -> %lu\n",
         (char far *)((void _seg *)((0xA000)) + (void near *)((0x0E))));
  printf("0xA000:0x0E -> %lu\n", (0xA000l << 16) | 0x0E);
  printf("sizeof(char far *) = %d\n", sizeof(char far *));
  printf("sizeof(char _seg *) = %d\n", sizeof(char _seg *));
  printf("sizeof(char near *) = %d\n", sizeof(char near *));
  getch();
  bmp = bmp_create_from_file("h.bmp");
  if (bmp == NULL) {
    puts("Can not open h.bmp!");
    return -1;
  }
  printf("info: %ldx%ld@%ld\n", bmp_get_width(bmp), bmp_get_height(bmp),
         bmp_get_depth(bmp));
  getch();

  iOldPage = 0;
  memset(&out, 0, sizeof(out));
  in.x.ax = 0x4f02;
  // in.x.bx = 0x117;
  // in.x.bx = 0x114;
  // in.x.bx = 0x103;
  // in.x.bx = 0x10e;
  in.x.bx = 0x10f;
  int86(0x10, &in, &out);
  // VideoPage(0);
  // pVideoMem = (unsigned int *)(MK_FP(0xA000, 0));
  // pVideoMem = (unsigned int *)(MK_FP(0xA0000, 0));
  // pVideoMem = 0xA000;
  // *(pVideoMem) = RGB(0x66, 0xCC, 0xFF);
  pVideoMem = (char far *)((void _seg *)((0xA000)) + (void near *)((0x00)));

  // pokeb(0xA000, 0, RGB(0x66, 0xCC, 0xFF));

  // for (i = 100; i < 200; i++) {
  //   for (j = 100; j < 200; j++) {
  //     SetPoint(i, j, 0xFFFFFF);
  //     // SetPoint(i, j, 0x3F);
  //   }
  // }
  // for (i = 0; i < 64; i++)
  //   for (j = 0; j < 64; j++)
  //     // SetPoint(i, j, ((unsigned int)gImage_image[i * 64 + j] << 8) |
  //     // gImage_image[i * 64 + j + 1]);
  //     SetPoint(i, j, RGB((unsigned int)gImage_image[i * 64 + j],
  //     (unsigned int)gImage_image[i * 64 + j + 1],
  //     (unsigned int)gImage_image[i * 64 + j + 2]));
  // for (i = 0; i < sizeof(gImage_image); i += 3)
  //   pokeb(0xA000, i,
  //         RGB((unsigned int)gImage_image[i], (unsigned int)gImage_image[i +
  //         1],
  //             (unsigned int)gImage_image[i + 2]));
  // memcpy((void *)pVideoMem, gImage_image32, sizeof(gImage_image32));
  // memcpy((char far *)((void _seg *)((0xA000)) + (void near *)((0))), (void
  // *)gImage_image32,
  //        sizeof(gImage_image32));
  // far_memcpy(pVideoMem, gImage_image32, sizeof(gImage_image32));
  write_video_buf(0, gImage_image32, sizeof(gImage_image32));
  write_window(bmp->colors, 0, 0, bmp_get_width(bmp), bmp_get_height(bmp));
  getch();
  // for (i = 0; i < sizeof(gImage_image); i += 2)
  //   pokeb(0xA000, i / 2, *((unsigned int *)(gImage_image + i)));
  // VideoPage(1);

  for (i = 0; i < sizeof(gImage_image32); i++) {
    // pokeb(0xA000, i, gImage_image32[i]);
    // (*((char far *)((void _seg *)((0xA000)) + (void near *)((i)))) =
    //      (char)(gImage_image32[i]));
    // *((char *)((void *)(0xA000) + (void *)(i))) = (char)(gImage_image32[i]);
    // *((char far *)((void _seg *)((0xA000)) + (void near *)((i)))) =
    // (char)(gImage_image32[i]);
    // *((char *)((0xA000l << 16) + i)) = (char)(gImage_image32[i]);
    *(char far *)((void _seg *)((0xA000)) + (void near *)((i))) =
        (char)(gImage_image32[i]);
  }
  for (k = 0; k < 255; k += 100) {
    for (i = 0; i < 50; i++) {
      for (j = 0; j < 50; j++) {
        SetPoint(j, i, RGB(k, 255 - k, k % 2 == 0 ? 255 : 0));
      }
    }
  }
  for (k = 0; k < 255; k += 100) {
    for (i = 0; i < 320; i++) {
      for (j = 0; j < 200; j++) {
        SetPoint(j, i, RGB(k, 255 - k, k % 2 == 0 ? 255 : 0));
      }
    }
  }
  getch();
  in.x.ax = 3;
  in.x.bx = 0;
  int86(0x10, &in, &out);
  getch();
  // 退回字符模式
  printf("p: %p out: 0x%X 0x%X\n", pVideoMem, out.h, out.x);
  getch();

  /*
    initgraph(&driver, &mode, "C:\\BORLANDC\\BGI");
    error_code = graphresult();
    if (error_code != grOk) {
      printf(" Graphics System Error: %s\n", grapherrormsg(error_code));
      exit(1);
    }
    getpalette(&palette);
    MaxColors = getmaxcolor() + 1;
    MaxX = getmaxx();
    MaxY = getmaxy();
    printf("max colors: %d, max_x: %d, max_y: %d", MaxColors, MaxX, MaxY);
    for (i = 0; i < MaxColors; i++) {
      setcolor(i);
      line(0, i * MaxY / MaxColors, MaxX - 1, i * MaxY / MaxColors);
    }
    getch();
    restorecrtmode();
    closegraph();
    getch();
    */
  puts("done.");
  return 0;
}