
#ifndef __MONKEY_2D__
#define __MONKEY_2D__

// Include files go here

#define MONKEY_VER "v.0.1 alpha"

#include <vector>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <list>
#include <time.h>
#include <math.h>
#ifndef USE_SDL
#include <dos.h>
#include <conio.h>
#endif

#ifdef USE_SDL
#include <SDL2/SDL.h>
#endif

#include "Monkey_Input.h"

#define MK2D_ScreenWidth 320
#define MK2D_ScreenHeight 200

void MK2D_ThrowError(std::string errorstr);

extern unsigned char * VGA_MemoryAddress;

typedef struct MK2D_PixelBuffer {
	unsigned short width;
	unsigned short height;
	unsigned short half_width;
	unsigned short half_height;
	unsigned short fastsize;
	float aspect;
	unsigned char *pixelbuffer;

	inline void OnCreateStruct(){
		width = height = half_width = half_height = fastsize = 0;
		pixelbuffer = NULL;
	}

	inline bool Create(int w,int h){
		if(pixelbuffer){
			free(pixelbuffer);
			pixelbuffer = NULL;
		}
		pixelbuffer = (unsigned char *)malloc(w*h);
		if(!pixelbuffer){
			MK2D_ThrowError("Could not allocate new pixel buffer!\n");
			return false;
		}
		width = w; 
		height = h; 

		fastsize = w*h;
		
		half_width = w / 2; 
		half_height = h / 2;
		
		aspect = (float)h / (float)w;

		memset(pixelbuffer,0,w*h); // Clear the buffer for fun
		return true;
	};

	inline void Free(){
		if(pixelbuffer){
			free(pixelbuffer);
			pixelbuffer = NULL;
		}
	};
}MK2D_PixelBuffer;


typedef struct MK2D_Texture2D {
	unsigned short width;
	unsigned short height;
	unsigned short half_width;
	unsigned short half_height;
	unsigned char *tex;
	unsigned char *mask;

	inline void OnCreateStruct(){
		width = height = half_width = half_height = 0;
		tex = NULL;
		mask = NULL;
	};

	inline bool LoadBitmap(std::string name,bool load_with_mask){
		FILE *bitmapFile;
		long index;
		unsigned short num_colors, inVal;
		unsigned char bmpByte;
		int x;

		if(tex){
			std::cout << "[WARNING] Texture already allocated!" << std::endl;
			free(tex);
			free(mask);
			tex = NULL;
			mask = NULL;
		}

		bitmapFile = fopen(name.c_str(),"rb");
		if(!bitmapFile){
			MK2D_ThrowError("[MK2D_Texture2D] Could not open bitmap " + name + "\n File does not exist!\n");
			return false;
		}else{
			/* check to see if it is a valid bitmap file */
			if (fgetc(bitmapFile)!='B' || fgetc(bitmapFile)!='M'){
				fclose(bitmapFile);
				MK2D_ThrowError("[MK2D_Texture2D] "+name+" is not a bitmap file.\n");
				return false;
			}
			std::cout << "[MK2D_Texture2D] Opened " << name.c_str() << std::endl;

			/* read in the width and height of the image, and the
			 number of colors used; ignore the rest */

		    for (int i=0; i<16; i++) fgetc(bitmapFile);
			fread(&inVal, sizeof(unsigned short), 1, bitmapFile);
			if(load_with_mask)
 				width = inVal / 2;
 			else
 				width = inVal;
		 	fgetc(bitmapFile);
		 	fgetc(bitmapFile);
			fread(&inVal,sizeof(unsigned short), 1, bitmapFile);
			height = inVal;
		    for (int i=0; i<22; i++) fgetc(bitmapFile);
			fread(&num_colors,sizeof(unsigned short), 1, bitmapFile);
		    for (int i=0; i<6; i++) fgetc(bitmapFile);
			
			bmpByte = fgetc(bitmapFile);
			if(bmpByte=='B'){
				// It's that other format
			   for (int i=0; i<67; i++) fgetc(bitmapFile);
			}else{
				ungetc(bmpByte,bitmapFile);
			}

			/* assume we are working with an 8-bit file */
			if (num_colors==0) num_colors=16;

			index = width;
			index *= height;

			if(index == 0){
				MK2D_ThrowError("[MK2D_Texture2D] Bitmap "+ name +" size is 0!\n");
				return false;				
			}

			if(index > 0x10000){
				MK2D_ThrowError("[MK2D_Texture2D] Bitmap "+ name +" too Large!\n");
				return false;
			}

			/* try to allocate memory */
			tex = (unsigned char *)malloc(index);
			if(!tex){
				fclose(bitmapFile);
				MK2D_ThrowError("[MK2D_Texture2D] Error allocating memory for bitmap texture "+name+"\n");
				return false;
			}
			if(load_with_mask){
				mask = (unsigned char *)malloc(index);
				if(!mask){
					fclose(bitmapFile);
					MK2D_ThrowError("[MK2D_Texture2D] Error allocating memory for bitmap maks "+name+"\n");
					return false;
				}
			}

//			printf("[R3D_LoadBitmap] Allocated Bitmap At %p \n",tex);

			/* Ignore the palette information for now.
			 See palette.c for code to read the palette info. */
		   for (int i=0; i<num_colors*4; i++) fgetc(bitmapFile);

			/* read the bitmap */
			for(index=(height-1)*(width);index>=0;index-=(width)){
				for(x=0;x<width;x++)
					tex[index+x]=(unsigned char)fgetc(bitmapFile);
				if(load_with_mask)
					for(x=0;x<width;x++)
						mask[index+x]=(unsigned char)fgetc(bitmapFile);
			}

			fclose(bitmapFile);

			half_width = width / 2;
			half_height = height / 2;
			return true;
		}
	};

	inline bool Create(int w,int h,bool create_mask){
		if(tex){
			std::cout << "[WARNING] Texture already allocated!" << std::endl;
			free(tex);
			free(mask);
			tex = NULL;
			mask = NULL;
		}

		tex = (unsigned char *)malloc(w*h);
		if(!tex){
			MK2D_ThrowError("Could not allocate new texture!\n");
			return false;
		}
		width = w; 
		height = h; 

		half_width = w / 2; 
		half_height = h / 2;

		if(create_mask){
			mask = (unsigned char *)malloc(w*h);
			if(!mask){
				MK2D_ThrowError("Could not allocate new texture mask!\n");
				return false;
			}
		}
		
		memset(tex,0,w*h); // Clear the texture for fun
		if(create_mask){
			memset(mask,0xFF,w*h); // Clear the mask for fun
		}
		return true;
	};

	inline void Free(){
		if(tex){
			free(tex);
			tex = NULL;
			width = height = half_width = half_height = 0;
		}
		if(mask){
			free(mask);
			mask = NULL;
		}
	};
	inline unsigned char SampleTex(float x,float y){
		if(tex!=NULL){
			int sx = (int)(x * (float)width);
			int sy = (int)(y * (float)height);
			// Used to just not get any value
			// now clamps values inside texture to make it render clean
			if(sx < 0) sx = 0;
			if(sy < 0) sy = 0;
			if(sx >= width) sx = width-1;
			if(sy >= height) sy = height-1;
			return tex[(sy * width) + sx];
		}
		return 0; // Black
	};
	inline unsigned char SampleMask(float x,float y){
		if(mask!=NULL){
			int sx = (int)(x * (float)width);
			int sy = (int)(y * (float)height);
			// Used to just not get any value
			// now clamps values inside texture to make it render clean
			if(sx < 0) sx = 0;
			if(sy < 0) sy = 0;
			if(sx >= width) sx = width-1;
			if(sy >= height) sy = height-1;
			return mask[(sy * width) + sx];
		}
		return 255; //Opaque
	};

	inline unsigned char SampleTexFast(int sx,int sy){
		if(tex!=NULL){
			return tex[(sy * width) + sx];
		}
		return 0; // Black
	};
	inline unsigned char SampleMaskFast(int sx,int sy){
		if(mask!=NULL){
			return mask[(sy * width) + sx];
		}
		return 255; //Opaque
	};
}MK2D_Texture2D;


typedef enum {
	MK2D_BLEND_NONE  = 0x01,
	MK2D_BLEND_ALPHA = 0x02,
	MK2D_BLEND_MOD   = 0x04,
	MK2D_BLEND_25    = 0x08, // 25%
	MK2D_BLEND_50    = 0x10, // 50%
	MK2D_BLEND_75    = 0x20, // 75%
}MK2D_BlendType;

bool MK2D_Init(void);
void MK2D_Shutdown();

void MK2D_SetVRT(bool v);

void MK2D_ResetRenderer();
void MK2D_SetRenderer(MK2D_PixelBuffer &buff);

void MK2D_SetBlendColor(unsigned char r, unsigned char g, unsigned char b);

int MK2D_GetFrameTime();
int MK2D_GetFPS();

void MK2D_DrawMonkeyMouse(void);

void MK2D_RenderBuffer(void);
void MK2D_RenderBuffer(MK2D_PixelBuffer &buff);

void MK2D_SetRenderBlend(int b);

void MK2D_ClearRenderer(unsigned char color);

void MK2D_RenderTexture2D(MK2D_Texture2D &tex, int ox = 0,int oy = 0,int dw = 0,int dh = 0);

void MK2D_RenderGliph(unsigned char gliph, int x,int y,unsigned char color);
void MK2D_BlitBar(int x,int y,int w,int h, unsigned char color);
void MK2D_BlitBox(int x,int y,int w,int h, unsigned char color);

// Blit a block of memory to a location in another block of memory
void MK2D_BlitMem2Mem(unsigned char *mem_d, int dw, int dh, unsigned char *mem_s,int sx,int sy,int sw,int sh);
void MK2D_BlitPartMem2Mem(unsigned char *mem_d, int dx, int dy, unsigned char *mem_s,int sx,int sy,int sw,int sh, int cw);
void MK2D_BlitTexture2D(MK2D_Texture2D &tex,int x,int y,int scalex = 1, int scaley = 1);
void MK2D_BlitTexture2D_Scale(MK2D_Texture2D &tex,int x,int y,float  scalex = 1.0f, float  scaley = 1.0f);
void MK2D_RenderAATexture2D(MK2D_Texture2D &tex, int x,int y,int ox,int oy,int dw,int dh);
void MK2D_RenderAATexture2DColored(MK2D_Texture2D &tex, int x,int y,int ox,int oy,int dw,int dh,int col);

void MK2D_SetFont(MK2D_Texture2D &font);

std::string MK2D_Int2Str(int val);

void MK2D_Write(std::string str,int x,int y,unsigned char color);
void MK2D_Write(int val,int x,int y,unsigned char color);

bool MK2D_IsError();
void MK2D_PrintExitCode();

extern MK2D_PixelBuffer MK_ScreenBuffer;


/*

	RGB Stuff

*/
typedef struct MK2D_RGB {
	unsigned char red;
	unsigned char green;
	unsigned char blue;
	inline void SetRGB(unsigned char r,unsigned char g, unsigned char b){
		red = r;
		green = g;
		blue = b;
	};
}MK2D_RGB;

// Define some colors

#define MK2D_BLACK 		0x00
#define MK2D_DARK_GREY 	0x49
#define MK2D_GREY 		0x92
#define MK2D_LIGHT_GREY 0xd7
#define MK2D_WHITE 		0xff

#define MK2D_RED 			0xe0
#define MK2D_GREEN 			0x3c
#define MK2D_DARK_GREEN 	0x30
#define MK2D_BLUE 			0x03
#define MK2D_DARK_BLUE 		0x01
#define MK2D_LIGHT_BLUE 	0x1f
#define MK2D_YELLOW 		0xfc
#define MK2D_PINK 			0xc3


/*      RGB Pallete Stuff      */
void MK2D_GenerateRGBPallete();

unsigned char MK2D_GetPalRGB(unsigned char rgb_r,unsigned char rgb_g,unsigned char rgb_b);
unsigned char MK2D_GetPalRGB(MK2D_RGB rgb);

MK2D_RGB MK2D_FindPalRGB(unsigned char palid);
MK2D_RGB MK2D_AddRGB(MK2D_RGB a, MK2D_RGB b, unsigned char alpha);
MK2D_RGB MK2D_ModRGB(MK2D_RGB a, MK2D_RGB b);
MK2D_RGB MK2D_ModulateRGB(MK2D_RGB a, MK2D_RGB b);
MK2D_RGB MK2D_MultiplyRGB(MK2D_RGB a, MK2D_RGB b);
MK2D_RGB MK2D_MaxRGB(MK2D_RGB A, MK2D_RGB B);


// Renderer thing
void MK2D_SetBlendColor(MK2D_RGB c);

/* Math stuff */

#define MK2D_DEG2RAD (float)(0.017453293)

#define MK2D_Sin(x) sin(x)
#define MK2D_Cos(x) cos(x)

// Useful drawing functions
void MK2D_ResetMatrix();
void MK2D_Rotate(float theta);
void MK2D_Scale(float sx, float sy);
void MK2D_Shear(float sx, float sy);
void MK2D_Translate(float ox, float oy);

// Backend stuff
void MK2D_Perspective(float ox, float oy);
void MK2D_Forward(float in_x, float in_y, float &out_x, float &out_y);
void MK2D_Backward(float in_x, float in_y, float &out_x, float &out_y);
void MK2D_InvertMatrix();


/*

	Text mode stuff

*/

void MK2D_ClearText(void);
void MK2D_PositionCursor(int x,int y);
void MK2D_Print(std::string str,int x,int y);

#endif

