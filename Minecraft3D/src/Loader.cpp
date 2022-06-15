
#include "Minecraft.h"


extern bool MC_UseLessMemory;

MK2D_Texture2D MC_DirtBackground,
				MC_MinecraftLogo,
				MC_ButtonTexture,
				MC_ButtonTextureBlue,
				MC_ButtonTextureNull,
				MC_GameFont;

MK2D_Texture2D MC_BlocksAtlas;

MK2D_Texture2D MC_SplashTexture;
MK2D_PixelBuffer SplashSurface;

void MC_SetSplash(void){
	if(MC_UseLessMemory)
		return; // No splashes
//	time_t t;
//	time(&t);
	// TODO:
	// Add splash text loading
	std::string SplashText = "Now on MS-DOS!";
	MC_SplashTexture.Create((SplashText.length()*8)+1,9,true);
	SplashSurface.Create((SplashText.length()*8)+1,9);

	MK2D_SetRenderer(SplashSurface);
	MK2D_SetRenderBlend(MK2D_BLEND_ALPHA);
	// Create Texture
	MK2D_ClearRenderer(1);
	MK2D_Write(SplashText,1,1,208); // dark yellow
	MK2D_Write(SplashText,0,0,MK2D_YELLOW);
	memcpy(MC_SplashTexture.tex,SplashSurface.pixelbuffer,SplashSurface.fastsize);
	// Create Mask
	MK2D_ClearRenderer(MK2D_BLACK);
	MK2D_Write(SplashText,0,0,MK2D_WHITE);
	MK2D_Write(SplashText,1,1,MK2D_WHITE);
	memcpy(MC_SplashTexture.mask,SplashSurface.pixelbuffer,SplashSurface.fastsize);
	MK2D_ResetRenderer();
	MK2D_SetRenderBlend(MK2D_BLEND_NONE);
};

void MC_LoadAssets(void ){

	// Load the game font
	MC_GameFont.OnCreateStruct();
	MC_GameFont.LoadBitmap("wad/default.bmp",true);
	MK2D_SetFont(MC_GameFont);

	MC_BlocksAtlas.OnCreateStruct();

	// Load the graphics
	if(!MC_UseLessMemory){
		MC_DirtBackground.OnCreateStruct();
		MC_DirtBackground.LoadBitmap("wad/dirt.bmp",false);

		MC_MinecraftLogo.OnCreateStruct();
		MC_MinecraftLogo.LoadBitmap("wad/gui/logo.bmp",true);

		MK2D_Texture2D GUI_BMP;

		GUI_BMP.OnCreateStruct();
		GUI_BMP.LoadBitmap("wad/gui/gui.bmp",true);

		MC_ButtonTextureNull.OnCreateStruct();
		MC_ButtonTextureNull.Create(200,20,false);

		MK2D_BlitPartMem2Mem(MC_ButtonTextureNull.tex,0,0,GUI_BMP.tex,0,46,200,20,GUI_BMP.width);

		MC_ButtonTexture.OnCreateStruct();
		MC_ButtonTexture.Create(200,20,false);

		MK2D_BlitPartMem2Mem(MC_ButtonTexture.tex,0,0,GUI_BMP.tex,0,66,200,20,GUI_BMP.width);

		MC_ButtonTextureBlue.OnCreateStruct();
		MC_ButtonTextureBlue.Create(200,20,false);

		MK2D_BlitPartMem2Mem(MC_ButtonTextureBlue.tex,0,0,GUI_BMP.tex,0,86,200,20,GUI_BMP.width);

		GUI_BMP.Free(); // We don't need you anymore!

		// Splash Stuff
		MC_SplashTexture.OnCreateStruct();
		SplashSurface.OnCreateStruct();

		// Game Block Textures
		MC_BlocksAtlas.LoadBitmap("wad/terrain.bmp",true);
	}
};

void MC_UnloadAssets(void){
	MC_ButtonTexture.Free();
	MC_ButtonTextureBlue.Free();
	MC_ButtonTextureNull.Free();
	MC_MinecraftLogo.Free();
	MC_DirtBackground.Free();
	MC_GameFont.Free();
	MC_SplashTexture.Free();
	SplashSurface.Free();
};
