
#include "Monkey_2D.h"

#ifdef USE_SDL
// The window we'll be rendering to
SDL_Window* Prog_Window = NULL;
// The window surface
SDL_Surface* Prog_Surface = NULL;
SDL_Surface* Prog_WinSurface = NULL;
SDL_Renderer* Prog_Render = NULL;
SDL_Texture* Prog_Texture = NULL;
// Stores the palette
SDL_Color sdl_colors[256];
#endif

// Error stuff
bool MONKEY2D_IsThereError = false;
std::string Monkey_ErrorString = "";

MK2D_PixelBuffer MK_MonkeyBuffer;
MK2D_PixelBuffer MK_ScreenBuffer;

unsigned char * VGA_MemoryAddress = (unsigned char *)0xA0000;

bool MK_WaitForVRT = true;

int MK_RenderBlending = 0x00;
MK2D_RGB MK_RenderBlendRGB;

int MK_FPS = 0;
int MK_FrameTime = 0;
int MK_Frames = 0;
int MK_LastTick = 0;

// Matrix / Scaling / Translation Stuff
float MK_Matrix[4][3][3];
int MK_TargetMatrix = 0;
int MK_SourceMatrix = 0;
bool MK_DirtyMatrix = false;

unsigned char MK_MouseColor = 0xFF;

bool MK2D_Init(void){
	std::cout << "Monkey 2D Graphics Library for MS-DOS " << MONKEY_VER << std::endl;
	std::cout << "Copyright (c) JB Games LLC 2021 - 2022 " << std::endl;

	MK_ScreenBuffer.OnCreateStruct();
	MK_MonkeyBuffer.OnCreateStruct();
	if(!MK_MonkeyBuffer.Create(320,200)){
		return false;
	} // Create a 320x200 pixel buffer

	// Set video mode to VGA mode 13h
	#ifdef USE_SDL

	SDL_Init(SDL_INIT_EVERYTHING);

	Prog_Window = SDL_CreateWindow(
		((std::string)("SDL Program")).c_str(),// Window title
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		640,      // width, in pixels
		400,     // height, in pixels
		0 // Flags
		);

	// Check that the window was successfully created
	if (Prog_Window == NULL) {
		std::cout << "Failed to create SDL Window!" << 
			"SDL-Error:: " << SDL_GetError() << std::endl;
		return false;
	}

	std::cout << ("Created SDL Window!") << std::endl;

	SDL_SetWindowMinimumSize(Prog_Window, 640, 400);
	SDL_SetWindowMaximumSize(Prog_Window, 640, 400);

	Prog_WinSurface = SDL_GetWindowSurface(Prog_Window);
	Prog_Surface = SDL_CreateRGBSurface(SDL_SWSURFACE, 320, 200, 8, 0, 0, 0, 0);
	Prog_Render = SDL_CreateRenderer(Prog_Window, -1, 0);
//	Prog_Texture = SDL_CreateTextureFromSurface(Prog_Render, Prog_Surface);

	if(Prog_Surface == NULL){
		// WHAT???
		std::cout << "Failed to get SDL surface!" << 
			"SDL-Error:: " << SDL_GetError() << std::endl;
		return false;
	}

	#else
	__asm{
		mov ax, 0x13
		int 0x10
	}
	#endif

	MK2D_SetRenderer(MK_MonkeyBuffer);

	MK2D_GenerateRGBPallete();

	MK_WaitForVRT = true;

	MK_RenderBlending = MK2D_BLEND_NONE;
	MK_RenderBlendRGB.SetRGB(0xFF,0xFF,0xFF);

	// Setup input
	if(!MK2D_InitInput()){
		return false;
	}

	return true;
};

void MK2D_Shutdown(){
	// Reset input
	MK2D_ShutdownInput();

	MK_MonkeyBuffer.Free();

	#ifdef USE_SDL
	//SDL_DestroyTexture(Prog_Texture);
	// Close and destroy the window
	if (Prog_Window) {
		SDL_DestroyWindow(Prog_Window);
		Prog_Window = NULL;
		std::cout << ("Destoyed Window") << std::endl;
	}
    //Quit SDL subsystems
    SDL_Quit();
	#else
	//Set the video mode to 3h (80*24 16color)
	__asm{ 
		mov ax,0x03
		int 0x10
	}
	#endif
	MK2D_PrintExitCode();
};


/*

	Rendering Stuff

*/

void MK2D_ResetRenderer(){
	MK_ScreenBuffer = MK_MonkeyBuffer;
};

void MK2D_SetRenderer(MK2D_PixelBuffer &buff){
	if((unsigned char*)buff.pixelbuffer == NULL){
		MK2D_ThrowError("[MK2D_SetRenderer] Bad renderer supplied!\n");
		return;
	}
	MK_ScreenBuffer = buff;
};

void MK2D_WaitVRT(void){
	// Wait for the VGA Retrace
	if(MK_WaitForVRT){
#ifdef USE_SDL
		SDL_Delay(1000/60);
#else
		while((inp(0x03da) & 8));
		while(!(inp(0x03da) & 8));
#endif
	}
};

void MK2D_SetVRT(bool v){
	MK_WaitForVRT = v;
};

void MK2D_SetBlendColor(unsigned char r, unsigned char g, unsigned char b){
	MK_RenderBlendRGB.SetRGB(r,g,b);
};

void MK2D_SetBlendColor(MK2D_RGB c){
	MK_RenderBlendRGB = c;
};

void MK2D_RenderBuffer(void){
	MK2D_WaitVRT();
	#ifdef USE_SDL
//	Uint8 *pixels;
//	int pitch;
//	SDL_Rect blitRect = { 0,0,320,200};
//	pixels = SDL_LockTexture(Prog_Texture);
//	memcpy((Uint8*)pixels, MK_ScreenBuffer.pixelbuffer,MK_ScreenBuffer.fastsize);
//	SDL_UnlockTexture(Prog_Texture);
	memcpy((Uint8*)Prog_Surface->pixels,MK_ScreenBuffer.pixelbuffer,MK_ScreenBuffer.fastsize);
	Prog_Texture = SDL_CreateTextureFromSurface(Prog_Render, Prog_Surface);
	SDL_RenderCopy(Prog_Render, Prog_Texture, NULL, NULL);
	SDL_RenderPresent(Prog_Render);
	SDL_DestroyTexture(Prog_Texture);
	#else
	memcpy(VGA_MemoryAddress,MK_ScreenBuffer.pixelbuffer,MK_ScreenBuffer.fastsize);
	#endif

	MK_Frames += 1;

	MK_FrameTime = time(NULL) - MK_FrameTime;

	if(time(NULL) >= MK_LastTick){
		MK_FPS = MK_Frames;
		MK_LastTick = time(NULL) + 1;
		MK_Frames = 0;
	}

	#ifdef USE_SDL
		MK_SDL_GetKeys();
	#endif

};

int MK2D_GetFPS(){
	return MK_FPS;
};

int MK2D_GetFrameTime(){
	return MK_FrameTime;
};

unsigned char MK_MouseMask[16] = {
	0xC0,// '11000000b',
	0xE0,// '11100000b',
	0xF0,// '11110000b',
	0xF8,// '11111000b',
	0xFC,// '11111100b',
	0xFC,// '11111100b',
	0xF8,// '11111000b',
	0xD8,// '11011000b',
	// Inverted mask
	0x00,// '00000000b',
	0x40,// '01000000b',
	0x60,// '01100000b',
	0x70,// '01110000b',
	0x78,// '01111000b',
	0x70,// '01110000b',
	0x50,// '01010000b',
	0x00,// '00000000b',
};

unsigned char BitMask[8] = {0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};

void MK2D_DrawMonkeyMouse(void){
	if(Monkey_Mouse.real_x >= MK_ScreenBuffer.width || Monkey_Mouse.real_y >= MK_ScreenBuffer.height)
		return;
	int mouseX = Monkey_Mouse.real_x;
	int mouseY = Monkey_Mouse.real_y;
	int px = 0;
	int py = 0;
	long off = (mouseY*MK_ScreenBuffer.width);
	py = 0;
	for(int i = 0 ; i < 8; i++){
		px = mouseX;
		for(int e = 0 ; e < 8; e++){
			int poff = off+py+px;
			if(poff>=0&&poff<MK_ScreenBuffer.fastsize&&px>=0&&px<MK_ScreenBuffer.width){
				if(MK_MouseMask[i]&BitMask[e])
					MK_ScreenBuffer.pixelbuffer[poff] = (MK_MouseMask[i+8]&BitMask[e])?MK_MouseColor:0;
				++px;
			}
		}
		py += MK_ScreenBuffer.width;
	}
};

void MK2D_RenderBuffer(MK2D_PixelBuffer &buff){
	MK2D_WaitVRT();
	#ifdef USE_SDL
	SDL_Rect blitRect = { 0,0,640,400};
	SDL_BlitScaled( Prog_WinSurface, NULL, Prog_Surface, &blitRect );
	//Update the surface
	SDL_UpdateWindowSurface( Prog_Window );
	#else
		memcpy(VGA_MemoryAddress,buff.pixelbuffer,buff.fastsize);
	#endif
};

void MK2D_ClearRenderer(unsigned char color){
	memset(MK_ScreenBuffer.pixelbuffer,color,MK_ScreenBuffer.fastsize);
};

void MK2D_BlitMem2Mem(unsigned char *mem_d, int dw, int dh, unsigned char *mem_s,int sx,int sy,int sw,int sh){
	if(!mem_s||!mem_d) return;
	unsigned char *mem_dptr = mem_d + (sy * dw) + (sx);
	unsigned char *mem_sptr = mem_s;
	for(int i = 0; i < sh; i++){
		memcpy(mem_dptr,mem_sptr,sw);
		mem_dptr += dw;
		mem_sptr += sw;
	}
};

void MK2D_BlitPartMem2Mem(unsigned char *mem_d, int dx, int dy, unsigned char *mem_s,int sx,int sy,int sw,int sh, int cw){
	if(!mem_s||!mem_d) return;
	unsigned char *mem_dptr = mem_d + (dy * sw) + dx;
	unsigned char *mem_sptr = mem_s + (sy * cw) + (sx);
	for(int i = 0; i < sh; i++){
		memcpy(mem_dptr,mem_sptr,sw);
		mem_dptr += sw;
		mem_sptr += cw;
	}
};


void MK2D_SetRenderBlend(int b){
	MK_RenderBlending = b;
};

void MK2D_BlitBar(int x,int y,int w,int h, unsigned char color){
	if(x<0) {
		w += x;
		x = 0;
	}
	if(y<0){
		h += y;
		y = 0;
	}
	if(w + x > MK_ScreenBuffer.width){
		w = (MK_ScreenBuffer.width - x);
	}
	if(h + y > MK_ScreenBuffer.height){
		h = (MK_ScreenBuffer.height - y);
	}
	if(h<=0||w<=0) return;
	for(int i = 0; i < h; i++){
		memset(MK_ScreenBuffer.pixelbuffer+((y+i)*MK_ScreenBuffer.width)+x,color,w);
	}
};

void MK2D_BlitBox(int x,int y,int w,int h, unsigned char color){
	h -= 1; // remove 1 because of the way we draw the bottom
	if(x<0) {
		w += x;
		x = 0;
	}
	if(y<0){
		h += y;
		y = 0;
	}
	if(w + x > MK_ScreenBuffer.width){
		w = (MK_ScreenBuffer.width - x);
	}
	if(h + y > MK_ScreenBuffer.height){
		h = (MK_ScreenBuffer.height - y);
	}
	if(h<=0||w<=0) return;
	long off = (y*MK_ScreenBuffer.width)+x;
	// Draw the top 
	memset(MK_ScreenBuffer.pixelbuffer+off,color,w);
	for(int i = 0; i < h; i++){
		MK_ScreenBuffer.pixelbuffer[off] = color; // Left side
		MK_ScreenBuffer.pixelbuffer[off+w-1] = color; // Right size
		off += MK_ScreenBuffer.width;
	}
	// Draw the bottom
	memset(MK_ScreenBuffer.pixelbuffer+off,color,w);
};


void MK2D_BlitTexture2DNoScale(MK2D_Texture2D &tex,int x,int y){
	if(!tex.tex) return;
	if(MK_RenderBlending&MK2D_BLEND_ALPHA){
		if(!tex.mask) return;
	}
	int rendw = tex.width;
	int rendh = tex.height;
	int texx = 0;
	int texy = 0;

	if(x<0) {
		rendw += x;
		texx -= x;
		x = 0;
	}
	if(y<0){
		rendh += y;
		texy -= y;
		y = 0;
	}
	if(rendw + x > MK_ScreenBuffer.width){
		rendw = (MK_ScreenBuffer.width - x);
	}
	if(rendh + y > MK_ScreenBuffer.height){
		rendh = (MK_ScreenBuffer.height - y);
	}

	if(rendh<=0||rendw<=0) return;
	long offset = (y*MK_ScreenBuffer.width)+x;
	long texoff = (texy*tex.width)+texx;
	for(int i = 0; i < rendh; i++){
		memcpy(MK_ScreenBuffer.pixelbuffer+offset,tex.tex+texoff,rendw);
		offset += MK_ScreenBuffer.width;
		texoff += tex.width;
	}
};

void MK2D_BlitTexture2D(MK2D_Texture2D &tex,int x,int y,int scalex, int scaley){
	if(!tex.tex) return;
	if(MK_RenderBlending&MK2D_BLEND_ALPHA){
		if(!tex.mask) return;
	}
	int rendw = tex.width;
	int rendh = tex.height;
	int texx = 0;
	int texy = 0;

	rendh *= scaley;
	rendw *= scalex;

	if(x<0) {
		rendw += x;
		texx -= x;
		x = 0;
	}
	if(y<0){
		rendh += y;
		texy -= y;
		y = 0;
	}
	if(rendw + x > MK_ScreenBuffer.width){
		rendw = (MK_ScreenBuffer.width - x);
	}
	if(rendh + y > MK_ScreenBuffer.height){
		rendh = (MK_ScreenBuffer.height - y);
	}

	if(rendh<=0||rendw<=0) return;

	if(scalex == 1 && scaley == 1 && MK_RenderBlending==MK2D_BLEND_NONE){
		long offset = (y*MK_ScreenBuffer.width)+x;
		long texoff = (texy*tex.width)+texx;
		for(int i = 0; i < rendh; i++){
			memcpy(MK_ScreenBuffer.pixelbuffer+offset,tex.tex+texoff,rendw);
			offset += MK_ScreenBuffer.width;
			texoff += tex.width;
		}
		return;
	}

	int ti = 0;
	long texoff = (texy*tex.width)+texx;
	for(int i = 0; i < rendh; i++){
		int te = 0;
		for(int e = 0; e < rendw; e++){
			unsigned char samp = tex.tex[texoff];

			if(MK_RenderBlending&MK2D_BLEND_ALPHA){
				if(tex.mask[texoff]!=0xFF){
					if((e+1)%scalex==0)
						++texoff;
					continue;
				}
			}
			if(MK_RenderBlending&MK2D_BLEND_MOD){
				MK2D_RGB p = MK2D_FindPalRGB(samp);
				p = MK2D_ModRGB(p,MK_RenderBlendRGB);
				samp = MK2D_GetPalRGB(p);
			}

			MK_ScreenBuffer.pixelbuffer[((i+y)*MK_ScreenBuffer.width)+x+e] = samp;
			if((e+1)%scalex==0)
				++texoff;
		}
		if((i+1)%scaley==0)
			texoff += tex.width - (rendw/scalex);
	}
};

void MK2D_BlitTexture2D_Scale(MK2D_Texture2D &tex,int x,int y,float scalex ,float  scaley ){
	if(scalex==1.0f&&scaley==1.0f&&MK_RenderBlending&MK2D_BLEND_NONE){
		MK2D_BlitTexture2DNoScale(tex,x,y);
		return;
	}
	if(!tex.tex) return;
	if(MK_RenderBlending&MK2D_BLEND_ALPHA){
		if(!tex.mask) return;
	}
	int rendw = tex.width;
	int rendh = tex.height;
	int texx = 0;
	int texy = 0;

	rendh /= (scaley);
	rendw /= (scalex);

	if(x<0) {
		rendw += x;
		texx -= x;
		x = 0;
	}
	if(y<0){
		rendh += y;
		texy -= y;
		y = 0;
	}
	if(rendw + x > MK_ScreenBuffer.width){
		rendw = (MK_ScreenBuffer.width - x);
	}
	if(rendh + y > MK_ScreenBuffer.height){
		rendh = (MK_ScreenBuffer.height - y);
	}

	if(rendh<=0||rendw<=0) return;

	long pixloff = (y*MK_ScreenBuffer.width)+x;

	float ti = 0;
	for(int i = 0; i < rendh; i++){
		float te = 0;
		for(int e = 0; e < rendw; e++){
			long texoff = (((int)ti+texy)*tex.width)+texx+(int)te;
			unsigned char samp = tex.tex[texoff];

			if(MK_RenderBlending&MK2D_BLEND_ALPHA){
				if(tex.mask[texoff]!=0xFF){
					te += scalex;
					continue;
				}
			}
			if(MK_RenderBlending&MK2D_BLEND_MOD){
				MK2D_RGB p = MK2D_FindPalRGB(samp);
				p = MK2D_ModRGB(p,MK_RenderBlendRGB);
				samp = MK2D_GetPalRGB(p);
			}

			MK_ScreenBuffer.pixelbuffer[pixloff] = samp;
			te += scalex;
			++pixloff;
		}
		pixloff += MK_ScreenBuffer.width - rendw;
		ti += scaley;
	}
};


void MK2D_RenderAATexture2D(MK2D_Texture2D &tex, int x,int y,int ox,int oy,int dw,int dh){
	if(!tex.tex) return;
	if(MK_RenderBlending&MK2D_BLEND_ALPHA){
		if(!tex.mask) return;
	}

	if(dw == 0)
		dw = tex.width;
	if(dh == 0)
		dh = tex.height;

	int texx = ox;
	int texy = oy;

	if(x<0) {
		dw += x;
		texx -= x;
		x = 0;
	}
	if(y<0){
		dh += y;
		texy -= y;
		y = 0;
	}
	if(dw + x > MK_ScreenBuffer.width){
		dw = (MK_ScreenBuffer.width - x);
	}
	if(dh + y > MK_ScreenBuffer.height){
		dh = (MK_ScreenBuffer.height - y);
	}

	if(dw<=0||dh<=0) return;
	if(texx >= tex.width || texy >= tex.height) return;

	long texoff = (texy*tex.width)+texx;
	long pixelOff = (y*MK_ScreenBuffer.width)+x;

	for(int i = 0; i < dh; i++){
		for(int e = 0; e < dw; e++){
			unsigned char samp = tex.tex[texoff];

			if(MK_RenderBlending&MK2D_BLEND_ALPHA){
				if(tex.mask[texoff]!=0xFF){
					++texoff;
					++pixelOff;
					continue;
				}
			}
			if(MK_RenderBlending&MK2D_BLEND_MOD){
				MK2D_RGB p = MK2D_FindPalRGB(samp);
				p = MK2D_ModRGB(p,MK_RenderBlendRGB);
				samp = MK2D_GetPalRGB(p);
			}

			MK_ScreenBuffer.pixelbuffer[pixelOff] = samp;
			++texoff;
			++pixelOff;
		}
		pixelOff += MK_ScreenBuffer.width - dw;
		texoff += tex.width-dw;
	}
};

void MK2D_RenderAATexture2DColored(MK2D_Texture2D &tex, int x,int y,int ox,int oy,int dw,int dh,int col){
	if(!tex.tex) return;
	if(MK_RenderBlending&MK2D_BLEND_ALPHA){
		if(!tex.mask) return;
	}

	if(dw == 0)
		dw = tex.width;
	if(dh == 0)
		dh = tex.height;

	int texx = ox;
	int texy = oy;

	if(x<0) {
		dw += x;
		texx -= x;
		x = 0;
	}
	if(y<0){
		dh += y;
		texy -= y;
		y = 0;
	}
	if(dw + x > MK_ScreenBuffer.width){
		dw = (MK_ScreenBuffer.width - x);
	}
	if(dh + y > MK_ScreenBuffer.height){
		dh = (MK_ScreenBuffer.height - y);
	}

	if(dw<=0||dh<=0) return;
	if(texx >= tex.width || texy >= tex.height) return;
	long pixelOff = (y*MK_ScreenBuffer.width)+x;
	long texoff = (texy*tex.width)+texx;
	for(int i = 0; i < dh; i++){
		for(int e = 0; e < dw; e++){
			unsigned char samp = col;

			if(MK_RenderBlending&MK2D_BLEND_ALPHA){
				if(tex.mask[texoff]!=0xFF){
					++texoff;
					++pixelOff;
					continue;
				}
			}
			if(MK_RenderBlending&MK2D_BLEND_MOD){
				MK2D_RGB p = MK2D_FindPalRGB(samp);
				p = MK2D_ModRGB(p,MK_RenderBlendRGB);
				samp = MK2D_GetPalRGB(p);
			}

			MK_ScreenBuffer.pixelbuffer[pixelOff] = samp;
			++pixelOff;
			++texoff;
		}
		pixelOff += MK_ScreenBuffer.width - dw;
		texoff += tex.width-dw;
	}
};


void MK2D_RenderTexture2D(MK2D_Texture2D &tex,int tx,int ty,int tw,int th){
	if(!tex.tex) return;

	if(tw == 0) tw = tex.width;
	if(th == 0) th = tex.height;

	// Work out bounding rectangle of sprite
	float ex, ey;
	float sx, sy;		
	float px, py;
	float swapv;

	MK2D_Forward((float)tx, (float)ty, sx, sy);
	px = sx; py = sy;
	sx = std::min(sx, px); sy = std::min(sy, py);
	ex = std::max(ex, px); ey = std::max(ey, py);
	
	MK2D_Forward((float)tw, (float)th, px, py);
	sx = std::min(sx, px); sy = std::min(sy, py);
	ex = std::max(ex, px); ey = std::max(ey, py);

	MK2D_Forward((float)tx, (float)th, px, py);
	sx = std::min(sx, px); sy = std::min(sy, py);
	ex = std::max(ex, px); ey = std::max(ey, py);

	MK2D_Forward((float)tw, (float)ty, px, py);
	sx = std::min(sx, px); sy = std::min(sy, py);
	ex = std::max(ex, px); ey = std::max(ey, py);

	// Perform inversion of transform if required
	MK2D_InvertMatrix();

	if (ex < sx) {
		swapv = ex;
		ex = sx;
		sx = swapv;
	}
	if (ey < sy) {
		swapv = ey;
		ey = sy;
		sy = swapv;
	}

	// Iterate through render space, and sample texture from suitable texel location
	if(sx < 0) sx = 0;
	if(sy < 0) sy = 0;
	if(ex > MK_ScreenBuffer.width) ex = MK_ScreenBuffer.width;
	if(ey > MK_ScreenBuffer.height) ey = MK_ScreenBuffer.height;
	float ox, oy;
	int sampX, sampY;
	for (float i = sx; i < ex; i++)
	{
		for (float j = sy; j < ey; j++)
		{
			MK2D_Backward(i, j, ox, oy);
			sampX = (int)(ceil(ox));
			sampY = (int)(ceil(oy));

			if(sampX >= tx && sampY >= ty && sampX < tw && sampY < th){
				if(MK_RenderBlending&MK2D_BLEND_ALPHA){
					unsigned char masksample = tex.SampleMaskFast(sampX,sampY);
					if(masksample!=0xFF){
						continue;
					}
				}
				unsigned char sample = tex.SampleTexFast(sampX,sampY);
				if(MK_RenderBlending&MK2D_BLEND_MOD){
					MK2D_RGB p = MK2D_FindPalRGB(sample);
					p = MK2D_ModRGB(p,MK_RenderBlendRGB);
					sample = MK2D_GetPalRGB(p);
				}

				MK_ScreenBuffer.pixelbuffer[((int)j*MK_ScreenBuffer.width)+(int)i] = sample;
			}
		}
	}

};

int mk_fontsize = 0;
int mk_gliph_width = 0;
int mk_gliph_height = 0;

MK2D_Texture2D *mk_font_ptr;

void MK2D_SetFont(MK2D_Texture2D &font){
	mk_font_ptr = &font;

	mk_gliph_width = font.width >> 4;
	mk_gliph_height = font.height >> 4;

	mk_fontsize = mk_gliph_width;
};

void MK2D_RenderGliph(unsigned char gliph, int x,int y,unsigned char color){
	int gy = gliph >> 4;
	int gx = gliph - (gy << 4);
	MK2D_RenderAATexture2DColored(*mk_font_ptr,x,y,gx*mk_gliph_width,gy*mk_gliph_height,mk_gliph_width,mk_gliph_height,color);
};

void MK2D_Write(std::string str,int x,int y,unsigned char color){
	for(int i = 0; i < str.length(); i++){
		MK2D_RenderGliph(str[i],x+(i*mk_fontsize),y,color);
	}
};

std::string MK2D_Int2Str(int val){
	std::string outstr = "";
	bool neg = false;
	if(val == 0 ) return "0";
	if(val < 0) {
		neg = true;
		val = -val;
	}
	while(val){
		outstr.push_back(outstr[0]);
		outstr[0] = '0'+(val%10);
		val /= 10;
	}
	if(neg){
		return "-" + outstr;
	}
	return outstr;
};

void MK2D_Write(int val,int x,int y,unsigned char color){
	unsigned char chr;
	int v, i , v2;
	i = 0;
	if(val==0){
		MK2D_RenderGliph('0',x,y,color);
	}
	if(val<0){
		MK2D_RenderGliph('-',x,y,color);
		i += 1;
	}
	v2 = val;
	while(v2){
		v2 /= 10;
		i += 1;
	}
	while(val){
		v = val%10;
		val /= 10;
		chr = '0'+v;
		MK2D_RenderGliph(chr,x+(i*mk_fontsize),y,color);
		--i;
	}
};


/********************************************************
 * 
 * 
 *  RRRR     GGGGG    BBBBB
 *  R   R   G         B    B
 *  R   R   G         BBBBB       Stuff
 *  RRRR    G  GGG    B    B
 *  R   R   G    G    B    B
 *  R   R    GGGGG    BBBBB 
 * 
 * 
 * 
 * *****************************************************/

void MK2D_GenerateRGBPallete(){
	// Setup a r3g3b2 RGB Pallete for the application
	unsigned char rgb_r, rgb_g, rgb_b;
	unsigned char gen_rgb;

	for(int rgb_index = 0; rgb_index < 256; rgb_index ++){
		gen_rgb = rgb_index;

		rgb_r = (gen_rgb>>5)&0x7;
		rgb_r *= 9;
		rgb_g = (gen_rgb>>2)&0x7;
		rgb_g *= 9;
		rgb_b = (gen_rgb&0x3);
		rgb_b *= 21;

		if(rgb_r > 63) rgb_r = 63;
		if(rgb_g > 63) rgb_g = 63;
		if(rgb_b > 63) rgb_b = 63;
#ifdef USE_SDL
		sdl_colors[rgb_index].r = rgb_r*4;
		sdl_colors[rgb_index].g = rgb_g*4;
		sdl_colors[rgb_index].b = rgb_b*4;
		sdl_colors[rgb_index].a = 255;
#else
		outp(0x03c8,rgb_index);
		outp(0x03c9,rgb_r);
		outp(0x03c9,rgb_g);
		outp(0x03c9,rgb_b);
#endif
	}
#ifdef USE_SDL
	SDL_SetPaletteColors(Prog_Surface->format->palette, sdl_colors, 0, 256);
#endif
};


unsigned char MK2D_GetPalRGB(unsigned char rgb_r,unsigned char rgb_g,unsigned char rgb_b){
	// Returns the closest pallete color to the rgb color
	rgb_r >>= 5; rgb_g >>= 5; rgb_b >>= 6;
	return (rgb_r<<5) + (rgb_g << 2) + (rgb_b);
};

unsigned char MK2D_GetPalRGB(MK2D_RGB rgb){
	return MK2D_GetPalRGB(rgb.red,rgb.green,rgb.blue);
};

MK2D_RGB MK2D_FindPalRGB(unsigned char palid){
	MK2D_RGB rgb;
	rgb.red = (palid>>5)&0x7;
	rgb.red *= 36;
	rgb.green = (palid>>2)&0x7;
	rgb.green *= 36;
	rgb.blue = (palid&0x3);
	rgb.blue *= 85;
	return rgb;
};

MK2D_RGB MK2D_ModRGB(MK2D_RGB A, MK2D_RGB B){
	MK2D_RGB C;
	int r,g,b;
	r = A.red;
	r += B.red;
	g = A.green;
	g += B.green;
	b = A.blue;
	b += B.blue;
	r >>= 1;
	g >>= 1;
	b >>= 1;
	if(r>255) r = 255;
	if(g>255) g = 255;
	if(b>255) b = 255;
	C.red = r;
	C.green = g;
	C.blue = b;
	return C;
};

MK2D_RGB MK2D_ModulateRGB(MK2D_RGB A, MK2D_RGB B){
	MK2D_RGB C;
	int r,g,b;
	if(B.red ==0 || B.green == 0 || B.blue == 0)
		return A;
	r = A.red;
	r %= B.red;
	g = A.green;
	g %= B.green;
	b = A.blue;
	b %= B.blue;
	if(r>255) r = 255;
	if(g>255) g = 255;
	if(b>255) b = 255;
	C.red = r;
	C.green = g;
	C.blue = b;
	return C;
};

MK2D_RGB MK2D_MaxRGB(MK2D_RGB A, MK2D_RGB B){
	MK2D_RGB C;
	C.red = std::max(A.red,B.red);
	C.green = std::max(A.green,B.green);
	C.blue = std::max(A.blue,B.blue);
	return C;
};

MK2D_RGB MK2D_MultiplyRGB(MK2D_RGB A, MK2D_RGB B){
	MK2D_RGB C;
	int r,g,b;
	r = A.red;
	r *= (float)B.red/(float)255;
	g = A.green;
	g *= (float)B.green/(float)255;
	b = A.blue;
	b *= (float)B.blue/(float)255;
	if(r>255) r = 255;
	if(g>255) g = 255;
	if(b>255) b = 255;
	C.red = r;
	C.green = g;
	C.blue = b;
	return C;
};

MK2D_RGB MK2D_AddRGB(MK2D_RGB A, MK2D_RGB B, unsigned char alpha){
	MK2D_RGB C;
	int r,g,b;
	r = A.red;
	r *= (1.0f-((float)alpha / 256.0f));
	r += B.red;
	r *= ((float)alpha / 256.0f);

	g = A.green;
	g *= (1.0f-((float)alpha / 256.0f));
	g += B.green;
	g *= ((float)alpha / 256.0f);

	b = A.blue;
	b *= (1.0f-((float)alpha / 256.0f));
	b += B.blue;
	b *= ((float)alpha / 256.0f);

	if(r>255) r = 255;
	if(g>255) g = 255;
	if(b>255) b = 255;
	C.red = r;
	C.green = g;
	C.blue = b;
	return C;
};


/*

	Matrix Math Stuff

*/

void MK2D_ResetMatrix(){
	MK_TargetMatrix = 0;
	MK_SourceMatrix = 1;
	MK_DirtyMatrix = true;

	// Columns then rows

	// Matrices 0 & 1 are used as swaps in Transform accumulation
	MK_Matrix[0][0][0] = 1.0f; MK_Matrix[0][1][0] = 0.0f; MK_Matrix[0][2][0] = 0.0f;
	MK_Matrix[0][0][1] = 0.0f; MK_Matrix[0][1][1] = 1.0f; MK_Matrix[0][2][1] = 0.0f;
	MK_Matrix[0][0][2] = 0.0f; MK_Matrix[0][1][2] = 0.0f; MK_Matrix[0][2][2] = 1.0f;

	MK_Matrix[1][0][0] = 1.0f; MK_Matrix[1][1][0] = 0.0f; MK_Matrix[1][2][0] = 0.0f;
	MK_Matrix[1][0][1] = 0.0f; MK_Matrix[1][1][1] = 1.0f; MK_Matrix[1][2][1] = 0.0f;
	MK_Matrix[1][0][2] = 0.0f; MK_Matrix[1][1][2] = 0.0f; MK_Matrix[1][2][2] = 1.0f;

	// Matrix 2 is a cache matrix to hold the immediate transform operation
	// Matrix 3 is a cache matrix to hold the inverted transform
};

void MK2D_MultiplyMatrix(){
	for(int c = 0; c < 3; c++){
		for(int r = 0; r < 3; r++){
			MK_Matrix[MK_TargetMatrix][c][r] =  MK_Matrix[2][0][r] * MK_Matrix[MK_SourceMatrix][c][0] + 
												MK_Matrix[2][1][r] * MK_Matrix[MK_SourceMatrix][c][1] + 
												MK_Matrix[2][2][r] * MK_Matrix[MK_SourceMatrix][c][2];
		}
	}
	int oldt = MK_SourceMatrix;
	MK_SourceMatrix = MK_TargetMatrix;
	MK_TargetMatrix = oldt;
	MK_DirtyMatrix = true; // Any transform multiply dirties the inversion
};

void MK2D_Rotate(float theta){
	// Construct Roation Matrix
	MK_Matrix[2][0][0] =  MK2D_Cos(theta);	MK_Matrix[2][1][0] = MK2D_Sin(theta);	MK_Matrix[2][2][0] = 0.0f;
	MK_Matrix[2][0][1] = -MK2D_Sin(theta);	MK_Matrix[2][1][1] = MK2D_Cos(theta);	MK_Matrix[2][2][1] = 0.0f;
	MK_Matrix[2][0][2] =  0.0f; 			MK_Matrix[2][1][2] = 0.0f;				MK_Matrix[2][2][2] = 1.0f;
	MK2D_MultiplyMatrix();
};

void MK2D_Scale(float sx, float sy){
	// Construct Scale Matrix
	MK_Matrix[2][0][0] = sx;    MK_Matrix[2][1][0] = 0.0f;  MK_Matrix[2][2][0] = 0.0f;
	MK_Matrix[2][0][1] = 0.0f;  MK_Matrix[2][1][1] = sy;    MK_Matrix[2][2][1] = 0.0f;
	MK_Matrix[2][0][2] = 0.0f;	MK_Matrix[2][1][2] = 0.0f;	MK_Matrix[2][2][2] = 1.0f;
	MK2D_MultiplyMatrix();
};

void MK2D_Shear(float sx, float sy){
	// Construct Shear Matrix
	MK_Matrix[2][0][0] = 1.0f;	MK_Matrix[2][1][0] = sx;	MK_Matrix[2][2][0] = 0.0f;
	MK_Matrix[2][0][1] = sy;	MK_Matrix[2][1][1] = 1.0f;	MK_Matrix[2][2][1] = 0.0f;
	MK_Matrix[2][0][2] = 0.0f;	MK_Matrix[2][1][2] = 0.0f;	MK_Matrix[2][2][2] = 1.0f;
	MK2D_MultiplyMatrix();
};

void MK2D_Translate(float ox, float oy){
	// Construct Translate Matrix
	MK_Matrix[2][0][0] = 1.0f;  MK_Matrix[2][1][0] = 0.0f;  MK_Matrix[2][2][0] = ox;
	MK_Matrix[2][0][1] = 0.0f;  MK_Matrix[2][1][1] = 1.0f;  MK_Matrix[2][2][1] = oy;
	MK_Matrix[2][0][2] = 0.0f;	MK_Matrix[2][1][2] = 0.0f;	MK_Matrix[2][2][2] = 1.0f;
	MK2D_MultiplyMatrix();
};

void MK2D_Perspective(float ox, float oy){
	// Construct Translate Matrix
	MK_Matrix[2][0][0] = 1.0f;  MK_Matrix[2][1][0] = 0.0f;  MK_Matrix[2][2][0] = 0.0f;
	MK_Matrix[2][0][1] = 0.0f;  MK_Matrix[2][1][1] = 1.0f;  MK_Matrix[2][2][1] = 0.0f;
	MK_Matrix[2][0][2] = ox;	MK_Matrix[2][1][2] = oy;	MK_Matrix[2][2][2] = 1.0f;
	MK2D_MultiplyMatrix();
};


void MK2D_Forward(float in_x, float in_y, float &out_x, float &out_y){
	out_x = in_x * MK_Matrix[MK_SourceMatrix][0][0] + in_y * MK_Matrix[MK_SourceMatrix][1][0] + MK_Matrix[MK_SourceMatrix][2][0];
	out_y = in_x * MK_Matrix[MK_SourceMatrix][0][1] + in_y * MK_Matrix[MK_SourceMatrix][1][1] + MK_Matrix[MK_SourceMatrix][2][1];
	float out_z = in_x * MK_Matrix[MK_SourceMatrix][0][2] + in_y * MK_Matrix[MK_SourceMatrix][1][2] + MK_Matrix[MK_SourceMatrix][2][2];
	if (out_z != 0){
		out_x /= out_z;
		out_y /= out_z;
	}
};

void MK2D_Backward(float in_x, float in_y, float &out_x, float &out_y){
	out_x = in_x * MK_Matrix[3][0][0] + in_y * MK_Matrix[3][1][0] + MK_Matrix[3][2][0];
	out_y = in_x * MK_Matrix[3][0][1] + in_y * MK_Matrix[3][1][1] + MK_Matrix[3][2][1];
	float out_z = in_x * MK_Matrix[3][0][2] + in_y * MK_Matrix[3][1][2] + MK_Matrix[3][2][2];
	if (out_z != 0){
		out_x /= out_z;
		out_y /= out_z;
	}
};

void MK2D_InvertMatrix(){
	// Obviously costly so only do if needed
	if(MK_DirtyMatrix){
		float det = MK_Matrix[MK_SourceMatrix][0][0] * (MK_Matrix[MK_SourceMatrix][1][1] * MK_Matrix[MK_SourceMatrix][2][2] - MK_Matrix[MK_SourceMatrix][1][2] * MK_Matrix[MK_SourceMatrix][2][1]) -
			        MK_Matrix[MK_SourceMatrix][1][0] * (MK_Matrix[MK_SourceMatrix][0][1] * MK_Matrix[MK_SourceMatrix][2][2] - MK_Matrix[MK_SourceMatrix][2][1] * MK_Matrix[MK_SourceMatrix][0][2]) +
			        MK_Matrix[MK_SourceMatrix][2][0] * (MK_Matrix[MK_SourceMatrix][0][1] * MK_Matrix[MK_SourceMatrix][1][2] - MK_Matrix[MK_SourceMatrix][1][1] * MK_Matrix[MK_SourceMatrix][0][2]);

		float idet = 1.0f / det;
		MK_Matrix[3][0][0] = (MK_Matrix[MK_SourceMatrix][1][1] * MK_Matrix[MK_SourceMatrix][2][2] - MK_Matrix[MK_SourceMatrix][1][2] * MK_Matrix[MK_SourceMatrix][2][1]) * idet;
		MK_Matrix[3][1][0] = (MK_Matrix[MK_SourceMatrix][2][0] * MK_Matrix[MK_SourceMatrix][1][2] - MK_Matrix[MK_SourceMatrix][1][0] * MK_Matrix[MK_SourceMatrix][2][2]) * idet;
		MK_Matrix[3][2][0] = (MK_Matrix[MK_SourceMatrix][1][0] * MK_Matrix[MK_SourceMatrix][2][1] - MK_Matrix[MK_SourceMatrix][2][0] * MK_Matrix[MK_SourceMatrix][1][1]) * idet;
		MK_Matrix[3][0][1] = (MK_Matrix[MK_SourceMatrix][2][1] * MK_Matrix[MK_SourceMatrix][0][2] - MK_Matrix[MK_SourceMatrix][0][1] * MK_Matrix[MK_SourceMatrix][2][2]) * idet;
		MK_Matrix[3][1][1] = (MK_Matrix[MK_SourceMatrix][0][0] * MK_Matrix[MK_SourceMatrix][2][2] - MK_Matrix[MK_SourceMatrix][2][0] * MK_Matrix[MK_SourceMatrix][0][2]) * idet;
		MK_Matrix[3][2][1] = (MK_Matrix[MK_SourceMatrix][0][1] * MK_Matrix[MK_SourceMatrix][2][0] - MK_Matrix[MK_SourceMatrix][0][0] * MK_Matrix[MK_SourceMatrix][2][1]) * idet;
		MK_Matrix[3][0][2] = (MK_Matrix[MK_SourceMatrix][0][1] * MK_Matrix[MK_SourceMatrix][1][2] - MK_Matrix[MK_SourceMatrix][0][2] * MK_Matrix[MK_SourceMatrix][1][1]) * idet;
		MK_Matrix[3][1][2] = (MK_Matrix[MK_SourceMatrix][0][2] * MK_Matrix[MK_SourceMatrix][1][0] - MK_Matrix[MK_SourceMatrix][0][0] * MK_Matrix[MK_SourceMatrix][1][2]) * idet;
		MK_Matrix[3][2][2] = (MK_Matrix[MK_SourceMatrix][0][0] * MK_Matrix[MK_SourceMatrix][1][1] - MK_Matrix[MK_SourceMatrix][0][1] * MK_Matrix[MK_SourceMatrix][1][0]) * idet;
		MK_DirtyMatrix = false;
	}
};


/*

	Text stuff

*/


unsigned char *TXT_Mem = (unsigned char *)0xB0000;

void MK2D_ClearText(void){
#ifdef USE_SDL
#else
	union REGS regs;
    regs.w.cx = 0;
    regs.w.dx = 0x1850;
    regs.h.bh = 7;
    regs.w.ax = 0x0600;
    int386( 0x10, &regs, &regs );
#endif
};

void MK2D_PositionCursor(int x,int y){
#ifdef USE_SDL
#else
	union REGS regs;
	if(x<=0) x = 1;
	if(y<=0) y = 1;
    regs.w.dx = ( y << 8 ) + x - 0x0101;
    regs.h.bh = 0;
    regs.h.ah = 2;
    int386( 0x10, &regs, &regs );
#endif
};

void MK2D_Print(std::string str,int x,int y){
	MK2D_PositionCursor(x+1,y+1);
    printf("%s", str.c_str() );
    fflush( stdout );
};


/*

	Error Stuff

*/

bool MK2D_IsError(){
	return MONKEY2D_IsThereError;
};

void MK2D_ThrowError(std::string errorstr){
	MONKEY2D_IsThereError = true;
	Monkey_ErrorString = errorstr;
};

void MK2D_PrintExitCode(){
	if(MONKEY2D_IsThereError){
		std::cout << "Runtime Error: " << Monkey_ErrorString.c_str() << std::endl;
	}
	else{
		std::cout << "Thanks for using Monkey2D!" << std::endl;
	}
};
