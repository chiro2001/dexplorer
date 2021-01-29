#include "svgasub.h"
#include <graphics.h>
#include <stdio.h>

int main(int argc, char const *argv[]) {
    //Step 1: initialize svga driver;
    SVGA_Init();
    //Step 2: Set system palette as windows default palette
    Set_Pal_File("win.act");

    //Step 3: draw as uaual
    setcolor(56);
    line(1, 1, 1023, 767);
    setcolor(77);
    rectangle(50, 60, 500, 600);
    setcolor(15);
    bar3d(20, 700, 1000, 750, 10, 77);

    getch();
    SVGA_Deinit();
    return 0;
}
