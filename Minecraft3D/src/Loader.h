#ifndef __MINECRAFT_LOADER__
#define __MINECRAFT_LOADER__

void MC_SetSplash(void);

void MC_LoadAssets(void);
void MC_UnloadAssets(void);

extern MK2D_Texture2D MC_DirtBackground,
				MC_MinecraftLogo,
				MC_ButtonTexture,
				MC_ButtonTextureBlue,
				MC_ButtonTextureNull;
#endif

