
#include "Minecraft.h"

extern bool MC_MinecraftIsRunning ;
extern bool MC_AudioAvailable;
extern bool MC_UseLessMemory;
extern int MC_ProgramTick;

extern int MC_MusicVolume;
extern int MC_SoundVolume;
extern int MC_MouseInverted;
extern int MC_ShowFPS;
extern int MC_RenderDistance;
extern int MC_ViewBobbing;
extern int MC_RenderType;
extern int MC_CapFPS;
extern int MC_Difficulty; 
extern int MC_GraphicsMode;
extern int MC_BrightnessValue;

extern bool MC_InGame;
extern bool MC_GamePaused;
extern bool MC_OverlayOpen;

MC_Menu MC_Menu_MainMenu,
		MC_Menu_SinglePlayer,
		MC_Menu_Multiplayer,
		MC_Menu_Tutorial,
		MC_Menu_Options,
		MC_Menu_Controls,
		MC_Menu_Game,
		MC_Menu_Paused;

MC_Menu *MC_MenuSelected;
bool MC_MenuChanged = false;
std::vector<MC_Menu *> MC_LastMenu;

void MC_QuitMinecraft_callback(void){
	MC_MinecraftIsRunning = false;
};

void MC_NullButtonFunction(void){
	MC_LastMenu.push_back(MC_MenuSelected);
};

void MC_PlayGameFunction(){
	MC_InGame = true;
	MC_GamePaused = false;
	MC_OverlayOpen = false;
	MC_LastMenu.clear();
};

void MC_Menu_DoneFun(void){
	if(MC_LastMenu.size()){
		MC_MenuSelected = MC_LastMenu.back();
		MC_LastMenu.pop_back();
		MC_MenuChanged = true;
		return;
	}
	if(MC_GamePaused){
		MC_GamePaused = false;
		MC_OverlayOpen = false;
		MC_LastMenu.clear();
		return;
	}
};

void MC_Menu_QuitGameFun(void){
	MC_GamePaused = false;
	MC_InGame = false;
	MC_OverlayOpen = false;

	MC_LastMenu.clear();
	MC_MenuChanged = false;

	// Set the splash up
//	MC_SetSplash();

};

void MC_SetMenuPause(void){
	MC_MenuSelected = &MC_Menu_Paused;
};

const int SplashX = 180;
const int SplashY = 64;
extern MK2D_Texture2D MC_SplashTexture;

void MC_MainMenuDrawMeFunction(void){
	MK2D_SetRenderBlend(MK2D_BLEND_ALPHA);
	if(MC_GraphicsMode >= MC_GRAPHICS_NORMAL&&!MC_UseLessMemory){
		MK2D_RenderAATexture2D(MC_MinecraftLogo,32,16,0,0,MC_MinecraftLogo.width,MC_MinecraftLogo.height);
		if(MC_GraphicsMode == MC_GRAPHICS_FANCY){
			float SpashSize = 1.0f + (sin((float)MC_ProgramTick/10)*0.25);
			MK2D_ResetMatrix(); 
			MK2D_Scale(SpashSize,SpashSize); 
//			MK2D_Translate(-(MC_SplashTexture.width/2),-(MC_SplashTexture.height/2));
			MK2D_Rotate(11.25*MK2D_DEG2RAD); // Add a tilt
			MK2D_Translate(SplashX,SplashY);
			MK2D_RenderTexture2D(MC_SplashTexture);
		}
	}
	MC_Write("Copyright Mojang AB. Do not distribute.",MK2D_ScreenWidth -(39*8), MK2D_ScreenHeight-10, MK2D_WHITE);
	MK2D_SetRenderBlend(MK2D_BLEND_NONE);
};

void MC_OptionsMenuDrawMeFunction(void){
	MK2D_SetRenderBlend(MK2D_BLEND_ALPHA);
	MC_Write("Options",(MK2D_ScreenWidth / 2)-(7*4), 8, MK2D_WHITE);
	MK2D_SetRenderBlend(MK2D_BLEND_NONE);
};

void MC_SingleplayerDrawMeFunction(void){
	// ???
};

void MC_PauseDrawMeFunction(void){
	MK2D_SetRenderBlend(MK2D_BLEND_ALPHA);
	MC_Write("Game menu",(MK2D_ScreenWidth / 2)-(7*4), 64, MK2D_WHITE);
	MK2D_SetRenderBlend(MK2D_BLEND_NONE);
};


const int ButtonWidth = 200; // These should never be changed!
const int ButtonHeight = 20; 
const int HalfButtonWidth = 100; 

void MC_Menu_Setup(void){

	MC_Button button;

	// Main Menu
	button.Setup("Singleplayer",(MK2D_ScreenWidth/2) - HalfButtonWidth, 88, ButtonWidth, true, &MC_Menu_SinglePlayer, MC_NullButtonFunction);
	MC_Menu_MainMenu.AddButton(button);
	button.Setup("Multiplayer",(MK2D_ScreenWidth/2) - HalfButtonWidth, 112, ButtonWidth, false, &MC_Menu_Multiplayer, MC_NullButtonFunction);
	MC_Menu_MainMenu.AddButton(button);
	button.Setup("Play tutorial level",(MK2D_ScreenWidth/2) - HalfButtonWidth, 136, ButtonWidth, false, &MC_Menu_Tutorial, MC_NullButtonFunction);
	MC_Menu_MainMenu.AddButton(button);
	button.Setup("Options",(MK2D_ScreenWidth/2) - HalfButtonWidth, 160, (float)ButtonWidth*0.49f, true, &MC_Menu_Options, MC_NullButtonFunction);//&MC_Menu_Options);
	button.scale = (float)ButtonWidth /  (float)button.width;
	MC_Menu_MainMenu.AddButton(button);
	button.Setup("Quit to DOS",(MK2D_ScreenWidth/2) + 2, 160, (float)ButtonWidth*0.49f, true, NULL,MC_QuitMinecraft_callback);
	button.scale = (float)ButtonWidth /  (float)button.width;
	MC_Menu_MainMenu.AddButton(button);
	MC_Menu_MainMenu.drawme = MC_MainMenuDrawMeFunction;

	// Options

	int ButtonLeftSide = (MK2D_ScreenWidth-4) - ((float)ButtonWidth*0.75f);

	if(MC_AudioAvailable){
		button.Setup("Music", 4, 24, (float)ButtonWidth*0.75f, true, NULL, NULL);
		button.scale = (float)ButtonWidth /  (float)button.width;
		button.SetAsSwitch(0,1,&MC_MusicVolume);
		button.AddValue("OFF");
		button.AddValue("ON");
	}else{
		button.Setup("Music: Disabled", 4, 24, (float)ButtonWidth*0.75f, false, NULL, NULL);		
		button.scale = (float)ButtonWidth /  (float)button.width;
	}
	MC_Menu_Options.AddButton(button);

	if(MC_AudioAvailable){
		button.Setup("Sound",ButtonLeftSide, 24, (float)ButtonWidth*0.75f, true, NULL, NULL);
		button.scale = (float)ButtonWidth /  (float)button.width;
		button.SetAsSwitch(0,1,&MC_SoundVolume);
		button.AddValue("OFF");
		button.AddValue("ON");
	}else{
		button.Setup("Sound: Disabled",ButtonLeftSide, 24, (float)ButtonWidth*0.75f, false, NULL, NULL);		
		button.scale = (float)ButtonWidth /  (float)button.width;
	}
	MC_Menu_Options.AddButton(button);

	button.Setup("Invert mouse", 4, 48, (float)ButtonWidth*0.75f, true, NULL, NULL);
	button.scale = (float)ButtonWidth /  (float)button.width;
	button.SetAsSwitch(0,1,&MC_MouseInverted);
	button.AddValue("OFF");
	button.AddValue("ON");
	MC_Menu_Options.AddButton(button);

	button.Setup("Show FPS",ButtonLeftSide, 48, (float)ButtonWidth*0.75f, true, NULL, NULL);
	button.scale = (float)ButtonWidth /  (float)button.width;
	button.SetAsSwitch(0,1,&MC_ShowFPS);
	button.AddValue("OFF");
	button.AddValue("ON");
	MC_Menu_Options.AddButton(button);

	button.Setup("View", 4, 72, (float)ButtonWidth*0.75f, true, NULL, NULL);
	button.scale = (float)ButtonWidth /  (float)button.width;
	button.SetAsSwitch(0,3,&MC_RenderDistance);
	button.AddValue("VERY SHORT");
	button.AddValue("SHORT");
	button.AddValue("FAR");
	button.AddValue("VERY FAR");
	MC_Menu_Options.AddButton(button);

	button.Setup("View bobbing",ButtonLeftSide, 72, (float)ButtonWidth*0.75f, true, NULL, NULL);
	button.scale = (float)ButtonWidth /  (float)button.width;
	button.SetAsSwitch(0,1,&MC_ViewBobbing);
	button.AddValue("OFF");
	button.AddValue("ON");
	MC_Menu_Options.AddButton(button);

	button.Setup("Brightness", 4, 96, (float)ButtonWidth*0.75f, true, NULL, NULL);
	button.scale = (float)ButtonWidth /  (float)button.width;
	button.SetAsSwitch(0,3,&MC_BrightnessValue);
	button.AddValue("MOODY");
	button.AddValue("DARK");
	button.AddValue("LIGHT");
	button.AddValue("BRIGHT");
	MC_Menu_Options.AddButton(button);

/*
	button.Setup("Render", 4, 96, (float)ButtonWidth*0.75f, true, NULL, NULL);
	button.scale = (float)ButtonWidth /  (float)button.width;
	button.SetAsSwitch(0,3,&MC_RenderType);
	button.AddValue("TRI");
	button.AddValue("SOLID A");
	button.AddValue("SOLID B");
	button.AddValue("TEXTURED");
	MC_Menu_Options.AddButton(button);
	*/

	button.Setup("VSync",ButtonLeftSide, 96, (float)ButtonWidth*0.75f, true, NULL, NULL);
	button.scale = (float)ButtonWidth /  (float)button.width;
	button.SetAsSwitch(0,1,&MC_CapFPS);
	button.AddValue("OFF");
	button.AddValue("ON");
	MC_Menu_Options.AddButton(button);

	button.Setup("Difficulty", 4, 120, (float)ButtonWidth*0.75f, true, NULL, NULL);
	button.scale = (float)ButtonWidth /  (float)button.width;
	button.SetAsSwitch(0,2,&MC_Difficulty);
	button.AddValue("Easy");
	button.AddValue("Normal");
	button.AddValue("Hard");
	MC_Menu_Options.AddButton(button);

	button.Setup("Graphics",ButtonLeftSide, 120, (float)ButtonWidth*0.75f, true, NULL, NULL);
	button.scale = (float)ButtonWidth /  (float)button.width;
	button.SetAsSwitch(0,2,&MC_GraphicsMode);
	button.AddValue("FAST");
	button.AddValue("NORMAL");
	button.AddValue("FANCY");
	MC_Menu_Options.AddButton(button);

	button.Setup("Controls",(MK2D_ScreenWidth/2) - HalfButtonWidth, MK2D_ScreenHeight - ((ButtonHeight + 8)*2), ButtonWidth, true, &MC_Menu_Controls, MC_NullButtonFunction);
	MC_Menu_Options.AddButton(button);

	button.Setup("Done",(MK2D_ScreenWidth/2) - HalfButtonWidth, MK2D_ScreenHeight - (ButtonHeight + 8), ButtonWidth, true, NULL, MC_Menu_DoneFun);
	MC_Menu_Options.AddButton(button);

	MC_Menu_Options.drawme = MC_OptionsMenuDrawMeFunction;

	// Singleplayer menu
	button.Setup("New World",(MK2D_ScreenWidth/2) - HalfButtonWidth, 88, ButtonWidth, true, &MC_Menu_Game, MC_PlayGameFunction);
	MC_Menu_SinglePlayer.AddButton(button);
	button.Setup("Done",(MK2D_ScreenWidth/2) - HalfButtonWidth, MK2D_ScreenHeight - (ButtonHeight + 8), ButtonWidth, true, NULL, MC_Menu_DoneFun);
	MC_Menu_SinglePlayer.AddButton(button);
	MC_Menu_SinglePlayer.drawme = MC_SingleplayerDrawMeFunction;

	// Pause Menu
	button.Setup("Back to game",(MK2D_ScreenWidth/2) - HalfButtonWidth, 88, ButtonWidth, true, &MC_Menu_Game, MC_PlayGameFunction);
	MC_Menu_Paused.AddButton(button);
	button.Setup("Save and quit to title",(MK2D_ScreenWidth/2) - HalfButtonWidth, 88+(ButtonHeight + 8), ButtonWidth, true, &MC_Menu_MainMenu, MC_Menu_QuitGameFun);
	MC_Menu_Paused.AddButton(button);
	button.Setup("Options...",(MK2D_ScreenWidth/2) - HalfButtonWidth, 88+(ButtonHeight + 48), ButtonWidth, true, &MC_Menu_Options, MC_NullButtonFunction);
	MC_Menu_Paused.AddButton(button);
	MC_Menu_Paused.drawme = MC_PauseDrawMeFunction;


	// Game Menu (No Menu)
	MC_Menu_Game.buttons.clear();

	MC_MenuSelected = &MC_Menu_MainMenu;
	MC_LastMenu.clear();
	MC_MenuChanged = false;

	// Set the splash up
	MC_SetSplash();
};

void MC_Menu_Shutdown(){
	MC_MenuSelected = NULL;

};

void MC_RenderMenu(void){
	if(!MC_MenuSelected){
		MC_ThrowError("[MC_RenderMenu] Menu selected was NULL!\n");
	}
	int ButtonPushed = -1;

	if(MC_MenuSelected->drawme){
		MC_MenuSelected->drawme();
	}

	for(int i = 0; i < MC_MenuSelected->buttons.size(); i++){
		int butW = MC_MenuSelected->buttons[i].width;
		float butWs = MC_MenuSelected->buttons[i].scale;
		int butX = MC_MenuSelected->buttons[i].x;
		int butY = MC_MenuSelected->buttons[i].y;
		std::string butStr = MC_MenuSelected->buttons[i].name;
		if(MC_MenuSelected->buttons[i].is_switch){
			butStr += ": ";
			if(*MC_MenuSelected->buttons[i].change_value<MC_MenuSelected->buttons[i].values.size())
				butStr += MC_MenuSelected->buttons[i].values[*MC_MenuSelected->buttons[i].change_value];
		}
		int len = butStr.length();

		if(MC_MenuSelected->buttons[i].can_click){
			if(Monkey_Mouse.real_x >= butX && 
				Monkey_Mouse.real_x < butX + butW && 
				Monkey_Mouse.real_y >= butY && 
				Monkey_Mouse.real_y < butY + ButtonHeight){

				if(MC_GraphicsMode == MC_GRAPHICS_FANCY&& !MC_UseLessMemory){
					MK2D_BlitTexture2D_Scale(MC_ButtonTextureBlue,butX,butY,butWs,1.0f);
				}else{
					MK2D_BlitBar(butX,butY,butW,ButtonHeight,110); // blue bar
					MK2D_BlitBox(butX,butY,butW,ButtonHeight,0); 
				}

				if(MK2D_MouseReleased(MK_RIGHT_MOUSE)){
					if(MC_MenuSelected->buttons[i].is_switch){
						*MC_MenuSelected->buttons[i].change_value -= 1;
						if(*MC_MenuSelected->buttons[i].change_value < MC_MenuSelected->buttons[i].s_min){
							*MC_MenuSelected->buttons[i].change_value = MC_MenuSelected->buttons[i].s_max;
						}
					}
				}
				if(MK2D_MouseReleased(MK_LEFT_MOUSE)){
					if(MC_MenuSelected->buttons[i].is_switch){
						*MC_MenuSelected->buttons[i].change_value += 1;
						if(*MC_MenuSelected->buttons[i].change_value > MC_MenuSelected->buttons[i].s_max){
							*MC_MenuSelected->buttons[i].change_value = MC_MenuSelected->buttons[i].s_min;
						}
					}
					ButtonPushed = i;
				}
			}else{
				if(MC_GraphicsMode == MC_GRAPHICS_FANCY&& !MC_UseLessMemory){
					MK2D_BlitTexture2D_Scale(MC_ButtonTexture,butX,butY,butWs,1.0f);
				}else{
					MK2D_BlitBar(butX,butY,butW,ButtonHeight,109); // grey bar
					MK2D_BlitBox(butX,butY,butW,ButtonHeight,0); 
				}
			}
			MK2D_SetRenderBlend(MK2D_BLEND_ALPHA);
			MC_Write(butStr,(butX + butW/2) - len*4,butY+6,MK2D_WHITE);
			MK2D_SetRenderBlend(MK2D_BLEND_NONE);
		}else{
			
			if(MC_GraphicsMode == MC_GRAPHICS_FANCY && !MC_UseLessMemory){
				MK2D_BlitTexture2D_Scale(MC_ButtonTextureNull,butX,butY,butWs,1.0f);
			}else{
				MK2D_BlitBar(butX,butY,butW,ButtonHeight,73); // dark grey bar
				MK2D_BlitBox(butX,butY,butW,ButtonHeight,0); 
			}
			MK2D_SetRenderBlend(MK2D_BLEND_ALPHA);
			MC_Write(butStr,(butX + butW/2) - len*4,butY+6,MK2D_GREY);
			MK2D_SetRenderBlend(MK2D_BLEND_NONE);
		}
	}

	if(MK2D_IsKeyReleased(mkey_escape)){
		MC_Menu_DoneFun(); // Go back
		ButtonPushed = -1; // No buttons should be pressed
	}

	// Moved to prevent tearing?
	if(ButtonPushed!=-1){
		if(MC_MenuSelected->buttons[ButtonPushed].onClick){
			MC_MenuSelected->buttons[ButtonPushed].onClick();
			if(MC_MenuChanged){
				MC_MenuChanged = false;
				return; // Prevent bad things from happening
			}
		}
		if(MC_MenuSelected->buttons[ButtonPushed].to_menu){
			MC_MenuSelected = MC_MenuSelected->buttons[ButtonPushed].to_menu;
		}
	}

};

void MC_Menu_DrawDirt(void){
	MK2D_SetRenderBlend(MK2D_BLEND_NONE);

	if(MC_GraphicsMode >= MC_GRAPHICS_NORMAL && !MC_UseLessMemory){
		for(int e = 0; e < 7; e++){
			for(int i = 0; i < 5; i++){
				MK2D_BlitTexture2D(MC_DirtBackground,i*64,e*32,1,1);
				MK2D_BlitTexture2D(MC_DirtBackground,(i*64)+32,e*32,1,1);
			}
		}
	}else{
		MK2D_ClearRenderer(32);
	}
};

void MC_DrawDarkenOverlay(void){
	if(!MK_ScreenBuffer.pixelbuffer) return;
	if(MC_GraphicsMode == MC_GRAPHICS_FANCY){
		MK2D_RGB rgbcol;
		MK2D_RGB blackrgb;
		blackrgb.SetRGB(128,128,128);

		for(int i = 0; i < 64000; i++){
			rgbcol = MK2D_FindPalRGB(MK_ScreenBuffer.pixelbuffer[i]);
			rgbcol = MK2D_MultiplyRGB(rgbcol,blackrgb);
			// Draw a shaded pixel here
			MK_ScreenBuffer.pixelbuffer[i] = MK2D_GetPalRGB(rgbcol);
		}
	}else{
		for(int e = 0; e < 200; e++){
			for(int i = 0; i+(e%2) < 320; i+=2){
				// Draw a black pixel here
				MK_ScreenBuffer.pixelbuffer[(e*MK_ScreenBuffer.width)+i+(e%2)] = MK2D_BLACK;
			}
		}
	}
};

