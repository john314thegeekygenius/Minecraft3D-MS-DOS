// Template header file

#ifndef __MINECRAFT__
#define __MINECRAFT__

#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>
/*
#include <mem.h>
#include <dos.h>
#include <i86.h>
#include <conio.h>
*/
#include <string.h>
#include <malloc.h>

#define MC_VERSION_NUMBER "0.1.0 alpha"

#include "Llama3D/Llama_3D.h"
#include "Monkey2D/Monkey_2D.h"

void MC_ThrowError(std::string error);

#define MC_GRAPHICS_FAST 0
#define MC_GRAPHICS_NORMAL 1
#define MC_GRAPHICS_FANCY 2

#include "World.h"
#include "Blocks.h"
#include "Menu.h"
#include "Wad.h"
#include "Loader.h"
#include "Gameplay.h"
#include "Utility.h"

#endif

