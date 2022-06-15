
#include "Monkey_2D.h"


bool MK_MouseAvailable = false;

MK2D_Mouse Monkey_Mouse;


#ifdef MK_KEYCODE_NAMES 
const char * MK_KeyCode_Strings[MAX_KEYS] = {
	"None",	"Escape", "1", "2", "3", "4", "5", "6", 
	"7", "8", "9", "0", "-", "=", "Backspace", "Tab", // 16
	"Q", "W", "E", "R", "T", "Y", "U", "I", 
	"O", "P", "[" , "]", "Enter", "Left Ctrl", "A", "S",//32
	"D", "F", "G", "H", "J", "K", "L", ";",
	"'", "`", "Left Shift", "\\", //44
	"Z", "X", "C", "V", "B", "N", "M", ",",
	".", "/", "Right Shift", "Keypad *",//56
	"Left Alt", "Space", "Capslock", "F1",
	"F2", "F3", "F4", "F5", "F6", "F7", "F8",//67
	"F9", "F10", "Num Lock", "Scroll Lock", "Keypad 7",
	"Keypad 8", "Keypad 9", "Keypad -", "Keypad 4",
	"Keypad 5", "Keypad 6", "Keypad +", "Keypad 1",//80
	"Keypad 2", "Keypad 3", "Keypad 0", "Keypad .",
	"SysReq", "Undefined", "Break", "F11",  "F12", //89
	// Escaped Codes
	"Prev Track", "Undefined", "Undefined", "Undefined",
	"Undefined", "Next Track", "Undefined", "Undefined",//97
	// 0x33 + 0xc1
	"Keypad Enter", "Right Ctrl", "Undefined","Undefined",
	"Mute", "Calculator", "Play", "Undefined",
	"Stop", "Undefined", "Undefined", "Undefined",
	"Undefined", "Undefined", "Volume Down", "Undefined",
	"Volume Up", "Undefined", "Web Home", "Undefined",
	"Undefined", "Keypad /", "Undefined", "Undefined",
	"Right Alt", "Undefined", "Undefined", "Undefined",
	"Undefined", "Undefined", "Undefined", "Home", 
	"Up Arrow", "Page Up", "Undefined", "Left Arrow",
	"Undefined", "Right Arrow", "Undefined", "End",
	"Down Arrow", "Page Down", "Insert", "Delete",
	// End of keys?
	"Undefined", "Undefined", "Undefined", "Left GUI",
	"Right GUI", "Apps", "Power", "Sleep",
	"Undefined", "Undefined", "Undefined", "Wake",
	"WWW Stop", "WWW Forward", "WWW Back", "Computer",
	"Email", "Media Select", "Undefined", "Undefined",
};

#endif


#ifdef USE_SDL

// SDL_Scan2PS[SDL_SCANCODE_xx] -> MK_KeyCodes
int SDL_Scan2PS[MAX_KEYS] = {
	0, 0, 0, 0, mkey_a, mkey_b, mkey_c, mkey_d, mkey_e, mkey_f, mkey_g, 
	mkey_g, mkey_i, mkey_j, mkey_k, mkey_l, mkey_m, mkey_n, mkey_o, mkey_p, 
	mkey_q, mkey_r, mkey_s, mkey_t, mkey_u, mkey_v, mkey_w, mkey_x, mkey_y, 
	mkey_z, mkey_1, mkey_2, mkey_3, mkey_4, mkey_5, mkey_6, mkey_7, mkey_8,
	mkey_9, mkey_0, mkey_enter, mkey_escape, mkey_backspace, mkey_tab, mkey_space,
	mkey_minus, mkey_equals, mkey_lbracket, mkey_rbracket, mkey_backslash,
	mkey_backslash, mkey_semicolon, mkey_apostrophe, mkey_tilde, mkey_comma,
	mkey_period, mkey_forwardslash, mkey_capslock, mkey_f1, mkey_f2, mkey_f3, mkey_f4,
	mkey_f5, mkey_f6, mkey_f7, mkey_f8, mkey_f9, mkey_f10, mkey_f11, mkey_f12,
	mkey_sysreq, mkey_scroll_lock, mkey_break, mkey_g_insert, mkey_g_home,
	mkey_g_pageup, mkey_g_delete, mkey_g_end, mkey_g_pagedown, mkey_g_right, 
	mkey_g_left, mkey_g_down, mkey_g_up, mkey_numlock, 
	mkey_keypad_divide, mkey_keypad_multiply, mkey_keypad_minus, mkey_keypad_plus,
	mkey_keypad_enter, mkey_keypad_1, mkey_keypad_2, mkey_keypad_3, mkey_keypad_4,
	mkey_keypad_5, mkey_keypad_6, mkey_keypad_7, mkey_keypad_8, mkey_keypad_9,  mkey_keypad_0,
	mkey_keypad_delete, mkey_backslash, 
};

#endif

bool MK_KeyBuffer[MAX_KEYS];

unsigned char MK_LastKey = 0;
unsigned char MK_LastKeyCode = 0;

#ifdef USE_SDL

SDL_Event Prog_Events;

void MK_SDL_GetKeys(){
	int scancode = 0;
	Monkey_Mouse.left_button = 0;
	Monkey_Mouse.right_button = 0;
	Monkey_Mouse.middle_button = 0;
	while (SDL_PollEvent(&Prog_Events)) {
		switch (Prog_Events.type) {
			case SDL_QUIT:
				MK2D_ThrowError("Closed Application.");
				break;
			case SDL_KEYDOWN:
				scancode = Prog_Events.key.keysym.scancode;
				if(scancode <= SDL_SCANCODE_KP_PERIOD){
					MK_LastKeyCode = SDL_Scan2PS[scancode];
					MK_KeyBuffer[SDL_Scan2PS[scancode]] = true;
				}
				break;
			case SDL_KEYUP:
				scancode = Prog_Events.key.keysym.scancode;
				if(scancode <= SDL_SCANCODE_KP_PERIOD){
					MK_LastKey = SDL_Scan2PS[scancode];
					MK_KeyBuffer[SDL_Scan2PS[scancode]] = false;
				}
				break;
			case SDL_MOUSEMOTION:
				SDL_GetMouseState(&Monkey_Mouse.real_x,&Monkey_Mouse.real_y);
				Monkey_Mouse.real_y /= 2;
				Monkey_Mouse.real_x /= 2;
			break;
			case SDL_MOUSEBUTTONDOWN:
				if (Prog_Events.button.button == SDL_BUTTON_LEFT) {
					Monkey_Mouse.left_button = 1;
				}
				if (Prog_Events.button.button == SDL_BUTTON_RIGHT) {
					Monkey_Mouse.right_button = 1;
				}
				if (Prog_Events.button.button == SDL_BUTTON_MIDDLE) {
					Monkey_Mouse.middle_button = 1;
				}
				break;
			case SDL_MOUSEBUTTONUP:
				if (Prog_Events.button.button == SDL_BUTTON_LEFT) {
					Monkey_Mouse.left_button = 2;
				}
				if (Prog_Events.button.button == SDL_BUTTON_RIGHT) {
					Monkey_Mouse.right_button = 2;
				}
				if (Prog_Events.button.button == SDL_BUTTON_MIDDLE) {
					Monkey_Mouse.middle_button = 2;
				}
				break;
		}
	}
};

#else
void interrupt	(*MK_OldKeyVect)(void);
static bool mk_key_special = false;

void interrupt MK_KeyboardService(void){

	unsigned char scan_code = inp(0x60); // Get the scan code

	// Tell the XT keyboard controller to clear the key
	unsigned char temp = inp(0x61);
	outp(0x61,temp | 0x80);
	outp(0x61,temp);

	if(scan_code==0xe0){
		// Special key prefix
		mk_key_special = true;
	}else if(scan_code==0xe1){
		// Pause key pressed
	}else{
		// Break code
		if (scan_code & 0x80){
			scan_code &= (unsigned char)0x7f;
			// DEBUG - handle special keys: ctl-alt-delete, print scrn
			MK_LastKey = scan_code;
			MK_KeyBuffer[scan_code] = false;
		}
		else{ 
			// Make code
			MK_LastKeyCode = scan_code;
			MK_KeyBuffer[scan_code] = true;
			mk_key_special = false;
		}
	}

	outp(0x20,0x20); // Tell the interupt we are done
};
#endif

bool MK2D_InitInput(void){
	// Setup Keyboard vector
	MK2D_ClearKeys();

	#ifdef USE_SDL
	#else
	MK_OldKeyVect = _dos_getvect(9); // Get the old vector
	if(!MK_OldKeyVect){
		MK2D_ThrowError("Could not setup keyboard interrupt\n");
		return false;
	}
	_dos_setvect(9,MK_KeyboardService); // Setup the new one
	#endif

	// Find Mouse Driver
	if(MK_InitMouse()){
		std::cout << "Found Mouse Driver!" << std::endl;
	}else{
		std::cout << "Warning: Could not find mouse driver!" << std::endl;
	}
	return true;
};

void MK2D_ShutdownInput(void){
	#ifdef USE_SDL
	#else
	// Reset the keyboard vector
	if(MK_OldKeyVect){
		_dos_setvect(9,MK_OldKeyVect); // Setup the new one
		MK_OldKeyVect = NULL;
	}
	#endif
};

void MK2D_ClearKeys(void){
	memset(&MK_KeyBuffer,false,MAX_KEYS*sizeof(bool));
};

unsigned char MK2D_KeyPressed(void){
	for(int i = 0; i < MAX_KEYS; i++){
		if(MK_KeyBuffer[i])
			return i;
	}
	return 0;
};

bool MK2D_IsKey(unsigned char key){
	return MK_KeyBuffer[key];
};

bool MK2D_IsKeyReleased(unsigned char key){
	if(MK_LastKey==key){
		MK_LastKey = 0;
		return true;
	}
	return false;
};

unsigned char MK2D_KeyReleased(void){
	unsigned char k = MK_LastKey;
	MK_LastKey = 0; 
	return k;
};

unsigned char MK2D_KeyTyped(void){
	unsigned char k = MK_LastKeyCode;
	MK_LastKeyCode = 0; 
	MK_LastKey = 0;
	return k;
};

char *MK2D_GetKeycodeName(unsigned char key){
	return (char*)MK_KeyCode_Strings[key];
};


/*

	Mouse Stuff

*/

// https://stanislavs.org/helppc/int_33.html

bool MK_InitMouse(void){
	#ifdef USE_SDL
	// Clear the struct
	memset(&Monkey_Mouse,0,sizeof(Monkey_Mouse));

	MK_MouseAvailable = true;

	//SDL_SetRelativeMouseMode(SDL_TRUE);
/*
case SDL_MOUSEMOTION:
// assuming I have a 800x600 Game window, then the result would be:
    static int xpos = SCR_W / 2; // = 400 to center the cursor in the window
    static int ypos = SCR_H / 2; // = 300 to center the cursor in the window
    xpos += e->motion.xrel;
    ypos += e->motion.yrel;
    ...
*/
	#else
	union REGS inregs, outregs;

	// Clear the struct
	memset(&Monkey_Mouse,0,sizeof(Monkey_Mouse));

	// Check for mouse driver
	inregs.w.ax = 0;
	int386( 0x33, &inregs, &outregs );

	if(outregs.w.ax == 0xffff){
		MK_MouseAvailable = true;
		Monkey_Mouse.num_buttons = outregs.w.bx;
	}else{
		MK_MouseAvailable = false;		
	}
	#endif
	return MK_MouseAvailable;
};

void MK_ShowMouse(){
	#ifdef USE_SDL
	SDL_ShowCursor(1);
	#else
	union REGS inregs, outregs;
	// Show the mouse cursor
	inregs.w.ax = 0x1;
	int386( 0x33, &inregs, &outregs );
	#endif
};

void MK_HideMouse(){
	#ifdef USE_SDL
	SDL_ShowCursor(0);
	#else
	union REGS inregs, outregs;
	// Hide the mouse cursor
	inregs.w.ax = 0x2;
	int386( 0x33, &inregs, &outregs );
	#endif
};

void MK_SetMouseSpeed(int speed){
	#ifdef USE_SDL
	#else
	union REGS inregs, outregs;
	// Change the speed of the mouse (default = 64)
	inregs.w.ax = 0x13;
	inregs.w.dx = speed;
	int386( 0x33, &inregs, &outregs );
	#endif
};

void MK_GetMouseSensitivity(void){
	#ifdef USE_SDL
	#else
	union REGS inregs, outregs;
	// Change the speed of the mouse (default = 64)
	inregs.w.ax = 0x1B;
	int386( 0x33, &inregs, &outregs );
	Monkey_Mouse.sense_x = outregs.w.bx;
	Monkey_Mouse.sense_y = outregs.w.cx;
	#endif
};

void MK_SetMouseSensitivity(void){
	#ifdef USE_SDL
	#else
	union REGS inregs, outregs;
	// Change the speed of the mouse (default = 64)
	inregs.w.ax = 0x1A;
	inregs.w.bx = Monkey_Mouse.sense_x;
	inregs.w.cx = Monkey_Mouse.sense_y;
	int386( 0x33, &inregs, &outregs );
	#endif
};


void MK_GetMouse(){
	#ifdef USE_SDL
	#else
	union REGS inregs, outregs;
	// Get the mouse cursor state
	inregs.w.ax = 0x3;
	int386( 0x33, &inregs, &outregs );
	Monkey_Mouse.real_x = outregs.w.cx;
	Monkey_Mouse.real_y = outregs.w.dx;
	/*
	  	|F-8|7|6|5|4|3|2|1|0|  Button Status
		  |  | | | | | | | `---- left button (1 = pressed)
		  |  | | | | | | `----- right button (1 = pressed)
		  |  | | | | | `-----  middle button (1 = pressed)
		  `------------------- unused
	*/
	if(outregs.w.bx & MK_LEFT_MOUSE){
		Monkey_Mouse.left_button = 1;
	}else{
		if(Monkey_Mouse.left_button==1){
			Monkey_Mouse.left_button = 2;
		}else{
			Monkey_Mouse.left_button = 0;
		}
	}

	if(Monkey_Mouse.num_buttons>1){
		if(outregs.w.bx & MK_RIGHT_MOUSE){
			Monkey_Mouse.right_button = 1;
		}else{
			if(Monkey_Mouse.right_button==1){
				Monkey_Mouse.right_button = 2;
			}else{
				Monkey_Mouse.right_button = 0;
			}
		}

		if(Monkey_Mouse.num_buttons==3){
			if(outregs.w.bx & MK_MIDDLE_MOUSE){
				Monkey_Mouse.middle_button = 1;
			}else{
				if(Monkey_Mouse.middle_button==1){
					Monkey_Mouse.middle_button = 2;
				}else{
					Monkey_Mouse.middle_button = 0;
				}
			}
		}
	}
	// Get the mouse delta positoin
	inregs.w.ax = 0xB;
	int386( 0x33, &inregs, &outregs );
	// count values are 1/200 inch intervals (1/200 in. = 1 mickey)
	Monkey_Mouse.delta_x = (signed short)outregs.w.cx;
	Monkey_Mouse.delta_y = (signed short)outregs.w.dx;
	#endif
};

bool MK2D_MouseReleased(unsigned char button){
	if(button == MK_LEFT_MOUSE)
		return (Monkey_Mouse.left_button==2);
	if(button == MK_RIGHT_MOUSE)
		return (Monkey_Mouse.right_button==2);
	if(button == MK_MIDDLE_MOUSE)
		return (Monkey_Mouse.middle_button==2);
	return false;
};

bool MK2D_MousePressed(unsigned char button){
	if(button == MK_LEFT_MOUSE)
		return (Monkey_Mouse.left_button==1);
	if(button == MK_RIGHT_MOUSE)
		return (Monkey_Mouse.right_button==1);
	if(button == MK_MIDDLE_MOUSE)
		return (Monkey_Mouse.middle_button==1);
	return false;
};
