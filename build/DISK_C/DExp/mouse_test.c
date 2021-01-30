/*------------------------------------>>>
 *//*很多学C的朋友对C语言下编写的鼠标程序感到神奇，但对它的实现却又很迷惘，其实它的原理很简单，以下就是我编写的一个TC图形下的鼠标菜单界面程序中的鼠标驱动部分，而整个程序也将很快和大家见面。
*//*Author : Cifry
*//*------------------------------------>>>*/
#include <bios.h>
#include <conio.h>
#include <dos.h>
#include <graphics.h>
getmouse() {
  union REGS inregs, outregs;
  while (!bioskey(1)) {
    inregs.x.ax = 3;
    int86(0x33, &inregs, &outregs);
    gotoxy(2, 6);
    printf("%-3d,%-3d", outregs.x.cx, outregs.x.dx);
    mouse_pointer(outregs.x.cx, outregs.x.dx);
  }
}
mouse_pointer(int mouse_x, int mouse_y) {
  int pixi, pixj;
  static int revert_x, revert_y;
  int _x, _y;
  static int savimage[16][16];
  static int flag = 1;
  int mose_pot[16] = {
      0x0003, 0x000D, 0x0032, 0x00C2, 0x0304, 0x0C04, 0x3008, 0xC008,
      0x4010, 0x2010, 0x1020, 0x0820, 0x0440, 0x0240, 0x0180, 0x0080,
  };
  if (flag == 1) {
    revert_x = mouse_x;
    revert_y = mouse_y;
    for (pixi = 0, _y = mouse_y; pixi < 16; pixi++, _y++)
      for (pixj = 0, _x = mouse_x; pixj < 16; pixj++, _x++)
        savimage[pixi][pixj] = getpixel(_x, _y);

    for (pixi = 0; pixi < 16; pixi++) {
      int test = 0x0001;
      for (pixj = 0; pixj < 16; pixj++) {
        if ((mose_pot[pixi] & test) != 0)
          putpixel(mouse_x + pixj, mouse_y + pixi, 15);
        test <<= 1;
      }
    }

    flag = 0;
  }
  if ((mouse_x != revert_x) || (mouse_y != revert_y)) {
    for (pixi = 0; pixi < 16; pixi++) {
      int test = 0x0001;
      for (pixj = 0; pixj < 16; pixj++) {
        if ((mose_pot[pixi] & test) != 0)
          putpixel(revert_x + pixj, revert_y + pixi, savimage[pixi][pixj]);
        test <<= 1;
      }
    }

    revert_x = mouse_x;
    revert_y = mouse_y;
    for (pixi = 0, _y = mouse_y; pixi < 16; pixi++, _y++)
      for (pixj = 0, _x = mouse_x; pixj < 16; pixj++, _x++)
        savimage[pixi][pixj] = getpixel(_x, _y);

    for (pixi = 0; pixi < 16; pixi++) {
      int test = 0x0001;
      for (pixj = 0; pixj < 16; pixj++) {
        if ((mose_pot[pixi] & test) != 0)
          putpixel(mouse_x + pixj, mouse_y + pixi,
                   ~savimage[pixi][pixj] & 0x00FF);
        test <<= 1;
      }
    }
  }
  for (pixi = 0; pixi < 64; pixi++) {
    for (pixj = 0; pixj < 64; pixj++) {
      putpixel(pixj + 4, pixi + 4, savimage[pixi / 4][pixj / 4]);
    }
  }
}
word() {
  int x = 100, y = 150;
  char author[] = "Author : Cifry";
  char qq[] = "OICQ   : 442044866";
  char email[] = "Email : cifry@163.com";

  settextstyle(0, 0, 4);
  setcolor(BLUE);
  outtextxy(x, y, "M");
  setcolor(GREEN);
  outtextxy(x += 32, y, "0");
  setcolor(CYAN);
  outtextxy(x += 32, y, "U");
  setcolor(RED);
  outtextxy(x += 32, y, "S");
  setcolor(MAGENTA);
  outtextxy(x += 32, y, "E");
  setcolor(BLACK);
  outtextxy(x += 32, y, " ");
  setcolor(BROWN);
  outtextxy(x += 32, y, "M");
  setcolor(LIGHTGREEN);
  outtextxy(x += 32, y, "a");
  setcolor(LIGHTCYAN);
  outtextxy(x += 32, y, "g");
  setcolor(LIGHTRED);
  outtextxy(x += 32, y, "n");
  setcolor(LIGHTMAGENTA);
  outtextxy(x += 32, y, "i");
  setcolor(YELLOW);
  outtextxy(x += 32, y, "f");
  setcolor(WHITE);
  outtextxy(x += 32, y, "i");
  setcolor(BLUE);
  outtextxy(x += 32, y, "e");
  setcolor(RED);
  outtextxy(x += 32, y, "r");
  x = 400;
  y = 250;
  settextstyle(0, 0, 0);
  setcolor(WHITE);
  outtextxy(x, y, author);
  outtextxy(x, y += 20, qq);
  outtextxy(x, y += 20, email);
}
main() {
  int gdriver, gmode;

  gdriver = VGA;
  gmode = VGAHI;
  initgraph(&gdriver, &gmode, "C:\\BORLANDC\\BGI");
  rectangle(0, 0, 72, 72);
  word();
  getmouse();
  getch();
}