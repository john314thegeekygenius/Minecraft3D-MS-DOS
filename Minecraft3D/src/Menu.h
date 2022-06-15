
#ifndef __MINECRAFT_MENU__
#define __MINECRAFT_MENU__

struct MC_Menu;

typedef struct MC_Button {
	int x;
	int y;
	int width;
	float scale;
	std::string name;
	std::vector<std::string> values;
	int *change_value;
	bool is_slider;
	int s_min;
	int s_max;
	struct MC_Menu *to_menu;
	bool can_click;
	bool is_switch;

	void (*onClick)();

	void inline Setup(std::string bname,int bx,int by,int bw, bool bc, struct MC_Menu *menu, void (*click)()){
		name = bname;
		x = bx;
		y = by;
		width = bw;
		to_menu = menu;
		can_click = bc;
		onClick = click;
		scale = 1.0f;
		change_value = NULL;
		s_min = 0;
		s_max = 0;
		is_slider = false;
		is_switch = false;
		values.clear();
	};
	void inline SetAsSlider(int smin,int smax, int *val){
		change_value = val;
		s_min = smin;
		s_max = smax;
		is_slider = true;
	};
	void inline SetAsSwitch(int smin,int smax, int *val){
		change_value = val;
		s_min = smin;
		s_max = smax;
		is_switch = true;
	};
	void inline AddValue(std::string dat){
		values.push_back(dat);
	};

}MC_Button;


typedef struct MC_Menu {
	std::vector<MC_Button> buttons;

	void (*drawme)();

	void inline AddButton(MC_Button &button){
		buttons.push_back(button);
	};
} MC_Menu;


void MC_SetMenuPause(void);
void MC_Menu_Setup(void);
void MC_Menu_Shutdown();
void MC_RenderMenu(void);
void MC_Menu_DrawDirt(void);
void MC_DrawDarkenOverlay(void);


#endif

