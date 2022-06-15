
#ifndef __MONKEY_INPUT__
#define __MONKEY_INPUT__
/*
#include <mem.h>
#include <dos.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <iostream>
#include <i86.h>
#include <io.h>
*/

#define MK_KEYCODE_NAMES

// Keyboard

#define MAX_KEYS 256



// https://www.win.tue.nl/~aeb/linux/kbd/scancodes.html#toc1

enum  {
	mkey_null, mkey_escape, mkey_1, mkey_2, mkey_3, mkey_4, mkey_5, mkey_6,
	mkey_7, mkey_8, mkey_9, mkey_0, mkey_minus, mkey_equals, mkey_backspace, mkey_tab,
	mkey_q, mkey_w, mkey_e, mkey_r, mkey_t, mkey_y, mkey_u, mkey_i,
	mkey_o, mkey_p, mkey_lbracket, mkey_rbracket, mkey_enter, mkey_lctrl, mkey_a, mkey_s,
	mkey_d, mkey_f, mkey_g, mkey_h, mkey_j, mkey_k, mkey_l, mkey_semicolon, 
	mkey_apostrophe, mkey_tilde, mkey_lshift, mkey_backslash, 
	mkey_z, mkey_x, mkey_c, mkey_v, mkey_b, mkey_n, mkey_m, mkey_comma,
	mkey_period, mkey_forwardslash, mkey_rshift, mkey_keypad_multiply,
	mkey_lalt, mkey_space, mkey_capslock, mkey_f1,
	mkey_f2, mkey_f3, mkey_f4, mkey_f5, mkey_f6, mkey_f7, mkey_f8,
	mkey_f9, mkey_f10, mkey_numlock, mkey_scroll_lock, mkey_keypad_7,
	mkey_keypad_8, mkey_keypad_9, mkey_keypad_minus, mkey_keypad_4,
	mkey_keypad_5, mkey_keypad_6, mkey_keypad_plus, mkey_keypad_1,
	mkey_keypad_2, mkey_keypad_3, mkey_keypad_0, mkey_keypad_delete,
	mkey_sysreq, mkey_unknown, mkey_break, mkey_f11, mkey_f12,

	// Extra
	mkey_keypad_enter = mkey_enter,
	mkey_rctrl        = mkey_lctrl,
	mkey_fakelshift   = mkey_lshift,
	mkey_keypad_divide = mkey_forwardslash,
	mkey_fakershift   = mkey_rshift,
	mkey_ctrlprntscr  = mkey_keypad_multiply,
	mkey_ralt         = mkey_lalt,
	mkey_ctrlbreak    = mkey_scroll_lock,
	mkey_g_home       = mkey_keypad_7,
	mkey_g_up         = mkey_keypad_8,
	mkey_g_pageup     = mkey_keypad_9,
	mkey_g_left       = mkey_keypad_4,
	mkey_g_right      = mkey_keypad_6,
	mkey_g_end        = mkey_keypad_1,
	mkey_g_down       = mkey_keypad_2,
	mkey_g_pagedown   = mkey_keypad_3,
	mkey_g_insert     = mkey_keypad_0,
	mkey_g_delete     = mkey_keypad_delete,

}MK_KeyCodes;


#ifdef MK_KEYCODE_NAMES 
extern const char * MK_KeyCode_Strings[MAX_KEYS];
#endif


bool MK2D_InitInput(void);
void MK2D_ShutdownInput(void);
void MK2D_ClearKeys(void);
unsigned char MK2D_KeyPressed(void);
unsigned char MK2D_KeyReleased(void);
bool MK2D_IsKey(unsigned char key);
bool MK2D_IsKeyReleased(unsigned char key);
char *MK2D_GetKeycodeName(unsigned char key);

// Mouse
typedef struct MK2D_Mouse {
	int real_x;
	int real_y;
	int delta_x;
	int delta_y;
	int sense_x;
	int sense_y;
	unsigned char left_button; // 0 - not pressed 1 - pressed 2 - released
	unsigned char middle_button;
	unsigned char right_button;
	int num_buttons;
}MK2D_Mouse;

extern MK2D_Mouse Monkey_Mouse;

#define MK_LEFT_MOUSE 0x01
#define MK_RIGHT_MOUSE 0x02
#define MK_MIDDLE_MOUSE 0x04

#ifdef USE_SDL
void MK_SDL_GetKeys();
#endif

bool MK_InitMouse(void);

void MK_ShowMouse();
void MK_HideMouse();
void MK_SetMouseSpeed(int speed);
void MK_GetMouseSensitivity(void);
void MK_SetMouseSensitivity(void);
void MK_GetMouse();
bool MK2D_MouseReleased(unsigned char button);
bool MK2D_MousePressed(unsigned char button);

#endif

