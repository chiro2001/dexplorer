#include <dos.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defs.h"
#include "bmpfile.h"
#include "video.h"

int main() {
  u32 i, j, k;
  bmpfile_t *bmp;
  bmp = bmp_create_from_file("h.bmp");
  if (bmp == NULL) {
    puts("Can not open h.bmp!");
    return -1;
  }
  printf("info: %ldx%ld@%ld\n", bmp_get_width(bmp), bmp_get_height(bmp),
         bmp_get_depth(bmp));
  getch();
  setVideoMode(VIDEO_MODE_VESA, VIDEO_VESA_320x200_32);
  writeVideoBMP(0, 0, bmp);
  getch();
  setVideoMode(VIDEO_MODE_TEXT, 0);
  puts("Done.");
  getch();
  return 0;
}