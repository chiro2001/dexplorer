#ifndef _SVGA_SUBSYSTEM_H_
#define _SVGA_SUBSYSTEM_H_

#include <graphics.h>
#include <dos.h>
#include <conio.h>
#include <stdlib.h>
#include <stdio.h>

/*
typedef unsigned char BYTE;
typedef unsigned int HWORD;
typedef unsigned long WORD;

typedef unsigned char uint8;
typedef unsigned int uint16;
typedef unsigned long uint32;

typedef struct {
	BYTE R;
	BYTE G;
	BYTE B;
} SVGA_COLOR;
*/

typedef struct {
	unsigned char R;
	unsigned char G;
	unsigned char B;
} SVGA_COLOR;


int SVGA_Init(void);
void SVGA_Deinit(void);
void Set_Pal_File(const char * path);

#endif
