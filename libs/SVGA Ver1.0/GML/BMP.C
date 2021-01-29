#include "BMP.h"
#include <graphics.h>
/***检查BMP图像是否符合要求***/
void BMP_check(bmp_picture image)
{
	if(image.file.bfType!=0X4D42)  //检查是否为bmp文件
	{
		printf("Not a BMP file!\n");
		printf("image.file.bfType=%u\n",image.file.bfType);
		delay(5000);
		exit(1);
	}
	if(image.info.biCompression!=0) //检查是否为压缩文件
	{
		printf("Can Not Open a Compressed bmp file!\n");
		printf("image.info.biCompression=%u\n",image.info.biCompression);
		delay(5000);
		exit(1);
	}
	if(image.info.biBitCount!=8) //检查是否为256色文件
	{
		printf("Not a 256 color bmp file\n");
		printf("image.info.biBitCount=%u",image.info.biBitCount);
		delay(5000);
		exit(1);
	}
	return;
}
/***换页函数***/
void Select_page(unsigned char page)
{
	union REGS regs;
	regs.x.ax=0x4f05;
	regs.x.bx=0;
	regs.x.dx=page;
	int86(0x10,&regs,&regs);
	return;
}
/***设置SVGA显示模式****/
unsigned char set_SVGA_mode(int vmode)
{
	union REGS regs;
	regs.x.ax=0x4f02;
	regs.x.bx=vmode;
	int86(0x10,&regs,&regs);
	return(regs.h.ah);
}
/***获取SVGA显示模式***/
unsigned int get_SVGA_mode(void)
{
	union REGS regs;
	regs.x.ax=0x4f03;
	int86(0x10,&regs,&regs);
	return(regs.x.bx);
}
/***设置调色板***/
void set_SVGA_palette(unsigned char RED[],unsigned char GREEN[],unsigned char BLUE[])
{
	int i;
	for(i=0;i<256;i++)
	{
		outportb(0X03C8,i);
		outportb(0X03C9,RED[i]>>2);
		outportb(0X03C9,GREEN[i]>>2);
		outportb(0X03C9,BLUE[i]>>2);
	}
	return;  
}
/***读取BMP图像***/
void Read_BMP(char *filename)
{
	FILE *fp;
	long ptrwidth, ptrheight;
	bmp_picture bmp256;
	char * buffer;
	unsigned int savemode;
	unsigned char np=0, op=0;
	unsigned long pos;
	unsigned int linebyte;
	unsigned char red[256],green[256],blue[256];
	int i,j;	
	if((fp=fopen(filename,"rb"))==NULL)
	{
		printf("%s BMP File Open Fail!\n",filename);
		delay(5000);
		exit(1);
	}
    fread((char *)&bmp256.file,sizeof(bitmapfile),1,fp);
	fread((char *)&bmp256.info,sizeof(bitmapinfo),1,fp);  //将bmp图像信息调至结构bmp256中
	fread((char *)&bmp256.palette[0],1024,1,fp);           //将bmp调色板调至结构bmp256中
	BMP_check(bmp256);  //检查bmp文件
	ptrwidth=bmp256.info.biWidth;
	ptrheight=bmp256.info.biHeight;
	linebyte=((ptrwidth*(long)bmp256.info.biBitCount+31)/32)*4;  //宽度修正	
	savemode=get_SVGA_mode();  //保存SVGA显示模式
	set_SVGA_mode(0x117);      //设置SVGA显示模式为
	for(i=0;i<256;i++)         //将图片调色板信息进行处理准备
	{
		red[i]=bmp256.palette[i].red;
		green[i]=bmp256.palette[i].green;
		blue[i]=bmp256.palette[i].blue;
	}
	set_SVGA_palette(red,green,blue);  //开始设置调色板
	fseek(fp,(long)bmp256.file.bfOffset,SEEK_SET);
	if((buffer=(char *)malloc(linebyte))==NULL)  //分配动态内存
	{
		printf("Malloc Func Used Fail!");
		delay(5000);
		exit(1);
	}
	for(i=ptrheight-1;i>=0;i--)
	{
		fread(buffer,linebyte,1,fp);
		for(j=0;j<ptrwidth;j++)
		{
			pos=i*(long)640l+j;
			np=pos/65536l;
			if(np!=op)  //SVGA显示换页
			{
				Select_page(np);
				op=np;
			}

			pokeb(0xa000,pos%65536l,buffer[j]);
		}
	}
	free(buffer);
	fclose(fp);
	getch();
	set_SVGA_mode(savemode);
	return;
}
void main()
{
	int gd=DETECT,gm;
	initgraph(&gd,&gm,"c:\\borlandc\\bgi");
	Read_BMP("aaa.bmp");
	bar(300,300,400,400);
    getch();
}