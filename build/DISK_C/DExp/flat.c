///////////////////////////////////////////////////////
// 4G Memory Access
// This Program Can Access 4G Bytes in DOS Real Mode
///////////////////////////////////////////////////////

// #include <graphics.h>
#include <stdlib.h>
#include <string.h>
#include <dos.h>
#include <stdio.h>

#include "image.c"

#define UINT32  	unsigned long
#define UINT16  	unsigned short
#define UINT8  		unsigned char

#pragma pack(1)

typedef struct {
	UINT16 limit0_15: 16;           //bit[15:0]
	UINT16 base0_15: 16;            //bit[31:16]
	UINT8 base16_23: 8;             //bit[39:32]
	UINT8 access_byte: 8;           //bit[47:40]
//			BYTE ac: 1;				//[bit0] Accessed bit. Just set to 0. The CPU sets this to 1 when the segment is accessed.
//			BYTE rw: 1;				//[bit1] Readable bit/Writable bit.
//									//  Readable bit for code selectors: Whether read access for this segment is allowed.
//									//                                   Write access is never allowed for code segments.
//									//  Writable bit for data selectors: Whether write access for this segment is allowed.
//									//                                   Read access is always allowed for data segments
//			BYTE dc: 1;				//[bit2] Direction bit/Conforming bit.
//			BYTE ex: 1;				//[bit3] Executable bit. If 1 code in this segment can be executed,
//									//  ie. a code selector. If 0 it is a data selector.
//			BYTE reserve: 1;		//[bit4] always be 1
//			BYTE privl: 2; 			//[bit5] Privilege, 2 bits. Contains the ring level,
//									//    0 = highest (kernel),
//									//    3 = lowest (user applications).
//			BYTE pr: 1;				//[bit7] Present bit. This must be 1 for all valid selectors.
	UINT8 limig16_19: 4;            //bit[51:48]
	UINT8 flags: 4;                 //bit[55:52]
//			BYTE reserve: 2;		//[bit0] always be 0
//			BYTE sz: 1; 			//[bit2] Granularity bit. If 0 the limit is in 1 B blocks (byte granularity),
//									//    if 1 the limit is in 4 KiB blocks (page granularity).
//			BYTE gr: 1; 			//[bit3] Size bit. If 0 the selector defines 16 bit protected mode.
//									//    If 1 it defines 32 bit protected mode. You can have both 16 bit and 32 bit selectors at once.
	UINT8 base24_31: 8;         //bit[63:56]
} GDT_ENTRY;

GDT_ENTRY gdt_table[] =
{
	//limit0_15, base0_15, base16_23, access_byte, limig16_19, flags, base24_31
	{         0,        0,         0,           0,          0,     0,         0 },   //NULL   (offset 0x00)
	{    0xFFFF,        0,         0,        0x9A,        0xF,   0xC,         0 },   //Code32 (offset 0x08) Base=0 Limit=4G-1 Size=4G
	{    0xFFFF,        0,         0,        0x92,        0xF,   0xC,         0 },   //Data32 (0ffset 0x10) Base=0 Limit=4G-1 Size=4G
};

//Global Descriptor Table
typedef struct {
	UINT16 size;
	union
	{
		UINT32 offset;
		struct
		{
			UINT16 offset_low;
			UINT16 offset_high;
		} x;
	} o;
} GDTR;

//interrupt descriptor table register
typedef struct {
	UINT16	limit;
	UINT32	base;
} IDTR;


IDTR oldIDTR = { 0, 0 };
IDTR newIDTR = { 0, 0 };
GDTR gdtr = { sizeof(gdt_table) - 1, 0 };

void KeyWait()
{
	int count = 0;

	while (inportb(0x64) & 2) {
		count++;
		if (count >= 3000) {
			break;
		}
	}
}

void A20Enable(void)
{
	KeyWait();
	outportb(0x64, 0xD1);
	KeyWait();
	outportb(0x60, 0xDF);    //Enable A20 with 8042.
	KeyWait();
	outportb(0x64, 0xFF);
	KeyWait();
}

void LoadFSLimit4G(void)
{
	A20Enable();    //Enable A20

	//**************************************
	//*       Disable interrupts           *
	//**************************************
	asm	{
		//Disable inerrupts
		CLI           	                    //Clear Interrupt Flag, interrupts disabled when interrupt flag cleared.

		//Save IDT
		SIDT	oldIDTR	                    //Store interrupt descriptor table (IDT) register

		//Set up empty IDT. (Disable any interrupts, include NMI)
		LIDT	newIDTR	                	//Load interrupt descriptor table (IDT) register
	}   //Include NMI.

	//***************************************
	//* 	Load GDTR	*
	//***************************************
	asm {
		//set "gdt_table" linear address to "gdtr.offset"
		db	0x66	                        //32 bit Operation Prefix in 16 Bit DOS.
		MOV	CX,DS	                        //MOV	ECX,DS

		db	0x66	                        //Get Data segment physical Address
		SHL	CX,4	                        //SHL	ECX,4

		db	0x66
		XOR	AX,AX	                        //XOR	EAX,EAX
		MOV	AX,offset gdt_table	            //MOV	AX,offset GDT_Table

		db	0x66
		ADD	AX,CX	                        //ADD	EAX,ECX

		MOV	word ptr gdtr[2], AX     		//gdtr.x.offset_low

		db	0x66
		SHR	AX,16	                        //SHR	EAX,16

		MOV	word ptr gdtr[4],AX	    		//gdtr.x.offset_high

		LGDT	gdtr	                	//Load GDTR
	}

	//**************************************
	//*  Enter 32 bit Flat Protected Mode  *
	//**************************************
	//	Set CR0 Bit-0 to 1 Enter 32 Bit Protection
	//Mode,And NOT Clear machine perform cache,It Meaning
	//the after Code HAD Ready To RUN in 32 Bit Flat Mode,
	//Then Load Flat Selector to FS and Description into it's
	//Shadow register,After that,ShutDown Protection Mode
	//And ReEnter Real Mode immediately.
	//	The FS holds Base=0 Size=4G Description and
	//it can Work in Real Mode as same as Pretect Mode,
	//untill FS be reloaded.
	//	In that time All the other Segment Registers are
	//Not Changed,except FS.(They are ERROR Value holded in CPU).
	asm	{
		MOV	DX,0x10	            //The Data32 Selector (defined in gdt_table)

		db	0x66,0x0F,0x20,0xC0	//MOV	EAX,CR0

		db	0x66
		MOV	BX,AX	            //MOV	EBX,EAX

		OR	AX,1
		db	0x66,0x0F,0x22,0xC0	//MOV	CR0,EAX	//Set Protection enable bit

		JMP	Flush
	}   //Clear machine perform cache.

Flush:  //Now In Flat Mode,But The CS is Real Mode Value.

	asm	{	            //And it's attrib is 16-Bit Code Segment.
		db	0x66
		MOV	AX,BX	            //MOV	EAX,EBX

		db	0x8E,0xE2	        //MOV	FS,DX	//Load FS now

		db	0x66,0x0F,0x22,0xC0	//MOV	CR0,EAX	//Return Real Mode.Now FS's Base=0 Size=4G

		LIDT	oldIDTR	        //Restore IDTR

		STI						//Enable INTR
	}
}


//With FS can Access All 4G Memory Now.But if FS be reloaded in Real Mode
//It's Limit will Be Set to FFFFh(Size=64K),then Can not used it to Access
//4G bytes Memory Again,Because FS is Segment:Offset Memory type after that.
//If Use it to Access large than 64K will generate Execption 0D.
UINT8 MmioRead8(UINT32 Address)
{
	asm	db 0x66
	asm	mov	di,word ptr Address			//MOV EDI,Address
	asm db 0x67, 0x64, 0x8a, 0x07		//MOV AL,FS:[EDI]
	return	_AL;
}

UINT16 MmioRead16(UINT32 Address)
{
	asm	db 0x66
	asm	mov	di,word ptr Address			//MOV EDI,Address
	asm db 0x64, 0x67, 0x8b, 0x07		//MOV AX,FS:[EDI]
	return	_AX;
}

UINT32 MmioRead32(UINT32 Address)
{
	UINT32 rrr;

	asm	db 0x66
	asm	mov	di,word ptr Address			//MOV EDI,Address
	asm db 0x64, 0x67, 0x66, 0x8b, 0x07	//MOV EAX,FS:[EDI]

	asm db 0x66
	asm mov word ptr rrr, ax

	return rrr;
}


void MmioWrite8(UINT32 Address, UINT8 data)
{
	asm mov al, data
	asm	db 0x66
	asm	mov	di,word ptr Address			//MOV EDI,Address
	asm db 0x64, 0x67, 0x88, 0x07		//MOV FS:[EDI],AL
}

void MmioWrite16(UINT32 Address, UINT16 data)
{
	asm mov ax, data
	asm	db 0x66
	asm	mov	di,word ptr Address			//MOV EDI,Address
	asm db 0x64, 0x67, 0x89, 0x07		//MOV FS:[EDI],AX
}

void MmioWrite32(UINT32 Address, UINT32 data)
{
	UINT16 hw = data >> 16;
	UINT16 lw = data & 0xFFFF;

	asm mov ax, hw
	asm	db 0x66
	asm shl ax, 16
	asm mov ax, lw

	asm	db 0x66
	asm	mov	di,word ptr Address			//MOV EDI,Address
	asm db 0x64, 0x67, 0x66, 0x89, 0x07	//MOV FS:[EDI],EAX
}

////////////////////////////////////////////////////////
void Dump4G(UINT32 Address)
{
	int	i;
	int	j;
	for (i = 0; i < 20; i++) {
		printf("%08lX: ", (Address + i * 16));
		for (j = 0; j < 16; j++) printf("%02X ", MmioRead8(Address + i * 16 + j));
		printf("    ");
		for (j = 0; j < 16; j++) {
			if (MmioRead8(Address + i * 16 + j) < 0x20) printf(".");
			else
				printf("%c", MmioRead8(Address + i * 16 + j));
		}
		printf("\n");
	}
}

void main(void)
{
	// char	KeyBuffer[256];
	// UINT32 	Address = 0;
	// UINT32	tmp;
  unsigned long pVideoMem;
  union REGS in, out;
  UINT16 i;

	LoadFSLimit4G();
	printf("Now you can Access The Machine All 4G Memory.\n\n");
	// printf("Input the Start Memory Physical to DUMP.\n");
	// printf("Press D to Cuntinue DUMP,0 to End & Quit.\n");
	// do	{
	// 	printf("-");
	// 	gets(KeyBuffer);
	// 	sscanf(KeyBuffer, "%lX", &tmp);
	// 	if (KeyBuffer[0] == 'q') break;
	// 	if (KeyBuffer[0] == 'd') Address += (20 * 16);
	// 	else
	// 		Address = tmp;
	// 	Dump4G(Address);
	// }while (Address != 0);

  in.x.ax = 0x4f02;
  // in.x.bx = 0x117;
  // in.x.bx = 0x114;
  // in.x.bx = 0x103;
  in.x.bx = 0x10e;
  int86(0x10, &in, &out);
  pVideoMem = 0xa000000l;

  // memcpy((void *)pVideoMem, gImage_image, sizeof(gImage_image));
  for (i = 0; i < sizeof(gImage_image); i++)
    MmioWrite8((UINT32)pVideoMem, gImage_image[i]);

  getchar();

  in.x.ax = 3;
  in.x.bx = 0;
  int86(0x10, &in, &out);
  
  puts("done");
  getchar();

	return;
}
