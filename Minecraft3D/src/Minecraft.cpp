
#include "Minecraft.h"

std::vector<std::string> MC_CmdlineArgs;

bool MC_MenuIsOpen = false;
bool MC_InGame = false;
bool MC_GamePaused = false;
bool MC_OverlayOpen = false;
bool MC_MinecraftIsRunning = false;
bool MC_UseLessMemory = false;
bool MC_AudioAvailable = false;
bool MC_ForceAudio = false;
int MC_ProgramTick = 0;


std::string MC_ErrorString = "";
bool MC_WasError = false;

int MC_MusicVolume = 0;
int MC_SoundVolume = 0;
int MC_MouseInverted = 0;
int MC_ShowFPS = 1;
int MC_RenderDistance = 0; // 0 = 1-chunk  1 = 2-chunks  2 = 4-chunks  3 = 6-chunks
int MC_ViewBobbing = 1;
int MC_RenderType = 0; // 0 - triangles only  1 - solid colors (no z-buffer)  2 - solid colors (z-buffer)  3 - textured
int MC_CapFPS = 0;
int MC_Difficulty = 0; // 0 - easy  1 - medium  2 - hard  3 - hardcore??
int MC_GraphicsMode = 0;
int MC_ScreenScale = 1; // Scale of the in game screen
int MC_BrightnessValue = 0; // Brightness value 0 = Moody 1 = Dark 2 = Light 3 = Bright

int MC_HandleArgs(void){
	for(int i = 0; i < MC_CmdlineArgs.size(); i++){
		if(strcmp(MC_CmdlineArgs[i].c_str(),"--ver")==0 ||
			strcmp(MC_CmdlineArgs[i].c_str(),"--version")==0 ||
			strcmp(MC_CmdlineArgs[i].c_str(),"--v")==0 ||
			strcmp(MC_CmdlineArgs[i].c_str(),"--info")==0 ||
			strcmp(MC_CmdlineArgs[i].c_str(),"--about")==0){
			std::cout << "Minecraft for MS-DOS ver " << MC_VERSION_NUMBER << std::endl;
			std::cout << "Engine Copyright (c) 2021 JB Games LLC" << std::endl;
			std::cout << "------------------------------------------------" << std::endl;
			std::cout << "Minecraft is a registered trademark of Mojang AB" << std::endl;
			std::cout << "Minecraft Copyright(c) 2009 - 2021 Mojang AB" << std::endl;
			std::cout << "The Minecraft textures are property of Mojang AB" << std::endl;
			std::cout << "--------(Do Not Distribute This Software)-------" << std::endl;
			return 1;
		}
		if(strcmp(MC_CmdlineArgs[i].c_str(),"--help")==0){
			std::cout << "Minecraft for MS-DOS ver " << MC_VERSION_NUMBER << std::endl;
			std::cout << "List of arguments:" << std::endl;
			std::cout << "  --v, --ver, --info   Show info about the program." << std::endl;
			std::cout << "  --no-audio           Disable audio in the game." << std::endl;
			std::cout << "  --force-audio        Force the game to play audio (assumes A=220 IRQ=7 DMA=1)." << std::endl;
//			std::cout << "  --max-mem [amount]   Set the max amount of memory available in MB." << std::endl;
			std::cout << "  --low-mem            Tell the game to use less memory." << std::endl;
//			std::cout << "  --gfx-mode [mode]    Set the video mode." << std::endl;
			return 1;
		}
		if(strcmp(MC_CmdlineArgs[i].c_str(),"--no-audio")==0){
			MC_AudioAvailable = false;
		}
		if(strcmp(MC_CmdlineArgs[i].c_str(),"--force-audio")==0){
			MC_AudioAvailable = true;
			MC_ForceAudio = true;
		}
		if(strcmp(MC_CmdlineArgs[i].c_str(),"--low-mem")==0){
			MC_UseLessMemory = true;
		}
	}
	return 0;
};

void MC_MinecraftStartup(){

	MC_WasError = false;

	MK2D_Init();
	LL3D_Init();
	MK_HideMouse(); // Don't show the driver mouse cursor

	// Make the width half
	MK_GetMouseSensitivity();
	Monkey_Mouse.sense_x /= 2;
	MK_SetMouseSensitivity();

	MC_LoadAssets(); 
	// TODO:
	// Could show loading screen here

	MC_Setup3DStuff();

	MC_Menu_Setup();

	MC_MenuIsOpen = false;
	MC_InGame = false;
	MC_GamePaused = false;
	MC_OverlayOpen = false;
	MC_MinecraftIsRunning = true;

};

bool MC_MinecraftLoop(){
	clock_t FrameClock = clock() + 1000;
	while(MC_MinecraftIsRunning){

		if(clock() > FrameClock){
			MC_ProgramTick += 1;
			FrameClock = clock() + 1000; // Get the current time
		}

		if(MK2D_IsError()||LL3D_IsError()||MC_WasError){
			return false;
		}
		MK_GetMouse();
		//MK2D_ClearRenderer(0);
		if(!MC_InGame){
			MC_Menu_DrawDirt();
			MC_RenderMenu();
			MK2D_SetVRT(MC_CapFPS);
		}else{
			// the game is running
			MC_Menu_DrawDirt();
			MC_GameLoop();
			MC_RenderMenu();
			MK2D_SetVRT(MC_CapFPS);
		}
		if(MC_ShowFPS){
			MK2D_SetRenderBlend(MK2D_BLEND_ALPHA);
			MC_Write(MK2D_GetFPS(),0,0,MK2D_WHITE);
			MK2D_SetRenderBlend(MK2D_BLEND_NONE);
		}
		if(MC_OverlayOpen||!MC_InGame)
			MK2D_DrawMonkeyMouse();
		MK2D_RenderBuffer();
	}
	return true;
};

void MC_MinecraftShutdown(){
	// Game stuff
	MC_Shutdown3DStuff();
	MC_Menu_Shutdown();
	MC_UnloadAssets();

	// Engine stuff
	LL3D_Shutdown();
	MK2D_Shutdown();
};

void MC_ThrowError(std::string error){
	MC_ErrorString = error;
	MC_WasError = true;
};










int main(int argc, char *args[]){
	for(int i = 1; i < argc; i++){
		MC_CmdlineArgs.push_back(args[i]);
	}
	if(MC_HandleArgs()!=0)
		return 0;
	MC_MinecraftStartup();
	MC_MinecraftLoop();
	MC_MinecraftShutdown();
	if(MC_WasError){
		MK2D_ClearText();
		MK2D_Print("Minecraft has experienced an unexpected error!",0,0);
		MK2D_Print(MC_ErrorString,0,1);
	}else{
		MK2D_ClearText();
		MK2D_Print("Thanks for playing MINE3D!\n",0,0);
	}
	return 0;
};