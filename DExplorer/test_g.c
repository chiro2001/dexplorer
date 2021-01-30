#include <dos.h>
// #include <graphics.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MODE_VGA13H  0x13
#define MODE_TEXT       0x03

void setmode(unsigned int mode)      //设置视频模式的函数
{
    // asm mov ah,0x00    //调用00h功能设置视频模式
    // asm mov al,mode    //AL寄存器放欲设置的视频模式号，这里以13h视频模式为例
    asm mov ax,0x4f02    //调用00h功能设置视频模式
    asm mov bx,mode    //AL寄存器放欲设置的视频模式号，这里以13h视频模式为例
    asm int    0x10        //调用10h中断
}

char far * vediobuf=(char far*)0xa0000000L;    //指向显存地址的指针
unsigned int iOldPage = 0;
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
  *(vediobuf+x+320*y)=iColor;
}

void putpixel(int x,int y,unsigned int c)
{
    *(vediobuf+x+320*y)=c;
}

int getpixel(int x,int y)              //取点的函数
{
    return *(vediobuf+x+320*y);
}

void main(void)
{
    int i;
    int j;

    //设置VGA13H视频模式
    // setmode(MODE_VGA13H);
    setmode(0x10f);

    //用一个二重循环画满屏幕
    for(i=0;i<200;i++)
        for(j=0;j<320;j++)
            SetPoint(j,i, (unsigned long)j << 8 + i);

    getch();
    //返回DOS文本模式
    setmode(MODE_TEXT);
}