#include <dos.h>
#include <graphics.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RGB(r, g, b) (((r) / 8) * 2048 + ((g) / 4) * 32 + (b) / 8)

unsigned int iOldPage;
unsigned int *pVideoMem;

void VideoPage(unsigned char page) {
  union REGS in, out;
  in.x.bx = 0;
  in.x.dx = page;
  in.x.ax = 0x4F05;
  int86(0x10, &in, &out);
}

void SetPoint(int x, int y, unsigned int iColor) {
  unsigned long iDist, iTemp;
  unsigned int iPos;
  int iPN;
  // iDist = 800l * y + x;
  iDist = 1024l * y + x;
  iTemp = iDist;
  iPN = 0;
  while (iTemp > 32768) {
    iTemp = iTemp - 32768;
    iPN++;
  }
  iPos = iTemp;
  if (iPN != iOldPage) VideoPage(iPN);
  // *(pVideoMem + iPos) = iColor;
  pokeb(0xA000, iPos * 2, iColor);
}

int main() {
  int driver = DETECT, mode = 0;
  struct palettetype palette;
  int MaxColors;
  int MaxX;
  int MaxY;
  int error_code;
  int i, j;

  // 尝试进入VESA模式
  union REGS in, out;

  puts("starting...");
  getch();

  iOldPage = 0;
  memset(&out, 0, sizeof(out));
  in.x.ax = 0x4f02;
  in.x.bx = 0x117;
  // in.x.bx = 0x114;
  // in.x.bx = 0x103;
  int86(0x10, &in, &out);
  // VideoPage(0);
  // pVideoMem = (unsigned int *)(MK_FP(0xA000, 0));
  // pVideoMem = (unsigned int *)(MK_FP(0xA0000, 0));
  pVideoMem = (unsigned int *)(0xA000);
  // *(pVideoMem) = RGB(0x66, 0xCC, 0xFF);

  // pokeb(0xA000, 0, RGB(0x66, 0xCC, 0xFF));

  for (i = 100; i < 200; i++) {
    for (j = 100; j < 200; j++) {
      SetPoint(i, j, RGB(i, j, 255));
      // SetPoint(i, j, 0x3F);
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