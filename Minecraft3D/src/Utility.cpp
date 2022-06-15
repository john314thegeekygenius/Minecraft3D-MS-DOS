
#include "Minecraft.h"


extern int MC_GraphicsMode;


extern int mk_fontsize; // From Monkey_2D.cpp

void MC_Write(std::string str,int x,int y,unsigned char color){
	MK2D_RGB col = MK2D_FindPalRGB(color);
	col.red >>= 2; col.green >>= 2; col.blue >>= 2;
	unsigned char darkcol = MK2D_GetPalRGB(col);
	for(int i = 0; i < str.length(); i++){
		if(MC_GraphicsMode >= MC_GRAPHICS_NORMAL)
			MK2D_RenderGliph(str[i],1+x+(i*mk_fontsize),1+y,darkcol);
		MK2D_RenderGliph(str[i],x+(i*mk_fontsize),y,color);
	}
};

void MC_Write(int val, int x,int y,unsigned char color){
	MC_Write(MK2D_Int2Str(val),x,y,color);
};

void MC_Write(float val, int x,int y,unsigned char color){
	MC_Write(MK2D_Int2Str((int)val),x,y,color);
};
