
#include "Llama_3D.h"

// Pointer to a pixel buffer
MK2D_PixelBuffer *LL3D_PixelBuffer = NULL;
PixelBuffer_f *LL3D_DepthBuffer = NULL;

LL3D_RenderType LL3D_RenderingType;
LL3D_RasterType LL3D_RasteringType;


bool LL3D_SetRenderer(MK2D_PixelBuffer &buffer,PixelBuffer_f &depth){
	LL3D_PixelBuffer = &buffer;
	LL3D_DepthBuffer = &depth;
	if(!LL3D_DepthBuffer){
		LL3D_ThrowError("[LL3D_SetRenderer] Bad Depth Buffer Supplied\n");
		return false;
	}
	if(!LL3D_PixelBuffer){
		LL3D_ThrowError("[LL3D_SetRenderer] Bad Renderer Supplied\n");
		return false;
	}
	return true;
};


/* Macro for drawing a pixel */
#define Draw(x,y,c) if((x)<LL3D_PixelBuffer->width&&(y)<LL3D_PixelBuffer->height&&(x)>=0&&(y)>=0) LL3D_PixelBuffer->pixelbuffer[((y)*LL3D_PixelBuffer->width)+(x)] = c;

#define DrawNC(x,y,c) LL3D_PixelBuffer->pixelbuffer[((y)*LL3D_PixelBuffer->width)+(x)] = c;

#define GetNC(x,y) LL3D_PixelBuffer->pixelbuffer[((y)*LL3D_PixelBuffer->width)+(x)]

/* Draw a line (Probably slow) */
void LL3D_DrawLine(int x1, int y1, int x2, int y2, unsigned char col){
	short x, y, dx, dy, dx1, dy1, px, py, xe, ye, i;
/*
	if(x1 < 0) x1 = 0;
	if(x2 < 0) x2 = 0;
	if(y1 < 0) y1 = 0;
	if(y2 < 0) y2 = 0;
	if(x1 >= LL3D_PixelBuffer->width) x1 = LL3D_PixelBuffer->width-1;
	if(x2 >= LL3D_PixelBuffer->width) x2 = LL3D_PixelBuffer->width-1;
	if(y1 >= LL3D_PixelBuffer->width) y1 = LL3D_PixelBuffer->width-1;
	if(y2 >= LL3D_PixelBuffer->width) y2 = LL3D_PixelBuffer->width-1;
*/
	dx = x2 - x1; 
	dy = y2 - y1;

	// Make sure they are not negitive
	dx1 = dx;
	dy1 = dy;
	if(dx1&0x8000) dx1 = -dx1;
	if(dy1&0x8000) dy1 = -dy1;

	px = (dy1<<1) - dx1;
	py = (dx1<<1) - dy1;
	if (dy1 <= dx1)
	{
		if (dx >= 0)
			{ x = x1; y = y1; xe = x2; }
		else
			{ x = x2; y = y2; xe = x1;}

		Draw(x, y, col)
		
		for (i = 0; x<xe; i++)
		{
			++x;
			if (px<0)
				px += (dy1<<1);
			else
			{
				if ((dx<0 && dy<0) || (dx>0 && dy>0)) {
					++y; 
				}else {
					--y;
				}
				px += ((dy1 - dx1)<<1);
			}

			Draw(x, y, col)
		}
	}
	else
	{
		if (dy >= 0)
			{ x = x1; y = y1; ye = y2; }
		else
			{ x = x2; y = y2; ye = y1; }

		Draw(x, y, col)

		for (i = 0; y<ye; i++)
		{
			++y;
			if (py <= 0)
				py += (dx1<<1);
			else
			{
				if ((dx<0 && dy<0) || (dx>0 && dy>0)) {
					++x; 
				}else {
					--x;
				}
				py += ((dx1 - dy1)<<1);
			}
			Draw(x, y, col)
		}
	}
	/*

	// (Probably really slow)
	int x, y, dx, dy, dx1, dy1, px, py, xe, ye, i;
	dx = x2 - x1; dy = y2 - y1;
	dx1 = LL3D_ABS(dx); 
	dy1 = LL3D_ABS(dy);
	px = 2 * dy1 - dx1;	py = 2 * dx1 - dy1;
	if (dy1 <= dx1)
	{
		if (dx >= 0)
			{ x = x1; y = y1; xe = x2; }
		else
			{ x = x2; y = y2; xe = x1;}

		Draw(x, y, col)
		
		for (i = 0; x<xe; i++)
		{
			x = x + 1;
			if (px<0)
				px = px + 2 * dy1;
			else
			{
				if ((dx<0 && dy<0) || (dx>0 && dy>0)) y = y + 1; else y = y - 1;
				px = px + 2 * (dy1 - dx1);
			}
			Draw(x, y, col)
		}
	}
	else
	{
		if (dy >= 0)
			{ x = x1; y = y1; ye = y2; }
		else
			{ x = x2; y = y2; ye = y1; }

		Draw(x, y, col)

		for (i = 0; y<ye; i++)
		{
			y = y + 1;
			if (py <= 0)
				py = py + 2 * dx1;
			else
			{
				if ((dx<0 && dy<0) || (dx>0 && dy>0)) x = x + 1; else x = x - 1;
				py = py + 2 * (dx1 - dy1);
			}
			Draw(x, y, col)
		}
	}*/
};

/* Draw a primitive triangle */
void LL3D_DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, unsigned char c)
{
	LL3D_DrawLine(x1, y1, x2, y2, c);
	LL3D_DrawLine(x2, y2, x3, y3, c);
	LL3D_DrawLine(x3, y3, x1, y1, c);
};
/*
#define drawline(s,e,y,col) \
	if(y>=0&&y<LL3D_PixelBuffer->height){\
		if(s<0) s = 0;\
		if(e>LL3D_PixelBuffer->width) e = LL3D_PixelBuffer->width;\
		if(e-s)\
			memset(LL3D_PixelBuffer->pixelbuffer+(y*LL3D_PixelBuffer->width)+s,col,e-s);\
	}
*/
void drawline(int s,int e,int y,unsigned char col) {
	if(e < 0) return;
	if(s > LL3D_PixelBuffer->width) return;
	if(s<0) s = 0;
	if(e>LL3D_PixelBuffer->width) e = LL3D_PixelBuffer->width;
	if((e-s) && y>=0&&y<LL3D_PixelBuffer->height){
		memset(LL3D_PixelBuffer->pixelbuffer+(y*LL3D_PixelBuffer->width)+s,col,e-s);
	}
};

#define SWAP(a,b) SWAP_V = a; a = b; b = SWAP_V;


/* Draw a filled circle */

void LL3D_FillCircle(int x,int y,int radius,unsigned char color){
	if(radius < 0 || x < -radius || y < -radius || 
		x - LL3D_PixelBuffer->width > radius || y - LL3D_PixelBuffer->height > radius){
		return; // Out of bounds
	}
	if(radius==0){
		Draw(x,y,color);
		return;
	}
	int x0 = 0;
	int y0 = radius;
	int d = 3 - (2 * radius); // PI - 2R

	while (y0 >= x0){
		drawline((x - y0), (x + y0), (y - x0), color);

		if (x0 > 0){
			drawline((x - y0), (x + y0), (y + x0), color);
		}

		if (d < 0)
			d += 4 * (x0++) + 6;
		else
		{
			if (x0 != y0)
			{
				drawline((x - x0), (x + x0), (y - y0), color);
				drawline((x - x0), (x + x0), (y + y0), color);
			}
			d += 4 * ((x0++) - (y0--)) + 10;
		}
	}
};


void LL3D_FillTriangle(int x1,int y1,int x2,int y2,int x3, int y3,int col){
	ll3d_int32 t1x, t2x, y, minx, maxx, t1xp, t2xp;
	char changed1 = 0;
	char changed2 = 0;
	ll3d_int32 signx1, signx2, dx1, dy1, dx2, dy2;
	ll3d_int32 e1, e2;
	ll3d_int32 i;
	ll3d_int32 SWAP_V;
	ll3d_int32 DrawS,DrawE;

	/* Sort vertices*/
	if (y1>y2) { SWAP(y1, y2); SWAP(x1, x2); }
	if (y1>y3) { SWAP(y1, y3); SWAP(x1, x3); }
	if (y2>y3) { SWAP(y2, y3); SWAP(x2, x3); }

	t1x = t2x = x1; y = y1;   /* Starting points*/
	dx1 = (ll3d_int32)(x2 - x1); if (dx1<0) { dx1 = -dx1; signx1 = -1; }
	else signx1 = 1;
	dy1 = (ll3d_int32)(y2 - y1);

	dx2 = (ll3d_int32)(x3 - x1); if (dx2<0) { dx2 = -dx2; signx2 = -1; }
	else signx2 = 1;
	dy2 = (ll3d_int32)(y3 - y1);

	if (dy1 > dx1) {   /* swap values*/
		SWAP(dx1, dy1);
		changed1 = 1;
	}
	if (dy2 > dx2) {   /* swap values*/
		SWAP(dy2, dx2);
		changed2 = 1;
	}

	e2 = (ll3d_int32)(dx2 >> 1);
	/* Flat top, just process the second half */
	if (y1 == y2) goto next;
	e1 = (ll3d_int32)(dx1 >> 1);

	for (i = 0; i < dx1;) {
		t1xp = 0; t2xp = 0;
		if (t1x<t2x) { minx = t1x; maxx = t2x; }
		else { minx = t2x; maxx = t1x; }
		/* process first line until y value is about to change */
		while (i<dx1) {
			i++;
			e1 += dy1;
			while (e1 >= dx1) {
				e1 -= dx1;
				if (changed1) t1xp = signx1;
				else          goto next1;
			}
			if (changed1) break;
			else t1x += signx1;
		}
		/* Move line */
	next1:
		/* process second line until y value is about to change */
		while (1) {
			e2 += dy2;
			while (e2 >= dx2) {
				e2 -= dx2;
				if (changed2) t2xp = signx2;
				else          goto next2;
			}
			if (changed2)     break;
			else              t2x += signx2;
		}
	next2:
		if (minx>t1x) minx = t1x; 
		if (minx>t2x) minx = t2x;
		if (maxx<t1x) maxx = t1x; 
		if (maxx<t2x) maxx = t2x;
		drawline(minx, maxx, y,col);    /*Draw line from min to max points found on the y*/
		/* Now increase y*/
		if (!changed1) t1x += signx1;
		t1x += t1xp;
		if (!changed2) t2x += signx2;
		t2x += t2xp;
		y += 1;
		if (y == y2) break;

	}
next:
	/* Second half */
	dx1 = (ll3d_int32)(x3 - x2); if (dx1<0) { dx1 = -dx1; signx1 = -1; }
	else signx1 = 1;
	dy1 = (ll3d_int32)(y3 - y2);
	t1x = x2;

	if (dy1 > dx1) {   /* swap values */
		SWAP(dy1, dx1);
		changed1 = 1;
	}
	else changed1 = 0;

	e1 = (ll3d_int32)(dx1 >> 1);

	for (i = 0; i <= dx1; i++) {
		t1xp = 0; t2xp = 0;
		if (t1x<t2x) { minx = t1x; maxx = t2x; }
		else { minx = t2x; maxx = t1x; }
		/* process first line until y value is about to change */
		while (i<dx1) {
			e1 += dy1;
			while (e1 >= dx1) {
				e1 -= dx1;
				if (changed1) { t1xp = signx1; break; }
				else          goto next3;
			}
			if (changed1) break;
			else   	   	  t1x += signx1;
			if (i<dx1) i++;
		}
	next3:
		/* process second line until y value is about to change */
		while (t2x != x3) {
			e2 += dy2;
			while (e2 >= dx2) {
				e2 -= dx2;
				if (changed2) t2xp = signx2;
				else          goto next4;
			}
			if (changed2)     break;
			else              t2x += signx2;
		}
	next4:

		if (minx>t1x) minx = t1x; 
		if (minx>t2x) minx = t2x;
		if (maxx<t1x) maxx = t1x; 
		if (maxx<t2x) maxx = t2x;
		drawline(minx, maxx, y,col);
		if (!changed1) t1x += signx1;
		t1x += t1xp;
		if (!changed2) t2x += signx2;
		t2x += t2xp;
		y += 1;
		if (y>y3) return;
	}	
};


void LL3D_TexturedTriangle(	int x1, int y1, float u1, float v1, float w1,
							int x2, int y2, float u2, float v2, float w2,
							int x3, int y3, float u3, float v3, float w3,
							MK2D_Texture2D *tex, MK2D_RGB color)
{
	float SWAP_V = 0;
	if (y2 < y1)
	{
		SWAP(y1, y2);
		SWAP(x1, x2);
		SWAP(u1, u2);
		SWAP(v1, v2);
		SWAP(w1, w2);
	}

	if (y3 < y1)
	{
		SWAP(y1, y3);
		SWAP(x1, x3);
		SWAP(u1, u3);
		SWAP(v1, v3);
		SWAP(w1, w3);
	}

	if (y3 < y2)
	{
		SWAP(y2, y3);
		SWAP(x2, x3);
		SWAP(u2, u3);
		SWAP(v2, v3);
		SWAP(w2, w3);
	}

	int dy1 = y2 - y1;
	int dx1 = x2 - x1;
	float dv1 = v2 - v1;
	float du1 = u2 - u1;
	float dw1 = w2 - w1;

	int dy2 = y3 - y1;
	int dx2 = x3 - x1;
	float dv2 = v3 - v1;
	float du2 = u3 - u1;
	float dw2 = w3 - w1;

	float tex_u, tex_v, tex_w;

	float dax_step = 0, dbx_step = 0,
		du1_step = 0, dv1_step = 0,
		du2_step = 0, dv2_step = 0,
		dw1_step=0, dw2_step=0;


	if (dy2) {
		dbx_step = dx2 / (float)LL3D_ABS(dy2);
		du2_step = du2 / (float)LL3D_ABS(dy2);
		dv2_step = dv2 / (float)LL3D_ABS(dy2);
		dw2_step = dw2 / (float)LL3D_ABS(dy2);
	}

	if (dy1){ 
		dax_step = dx1 / (float)LL3D_ABS(dy1);
		du1_step = du1 / (float)LL3D_ABS(dy1);
		dv1_step = dv1 / (float)LL3D_ABS(dy1);
		dw1_step = dw1 / (float)LL3D_ABS(dy1);

		if(y2 >= LL3D_DepthBuffer->height){
			y2 = LL3D_DepthBuffer->height-1;
		}

		for (int i = y1; i <= y2; i++)
		{
			float idif1 = (float)(i - y1);

			int ax = x1 + idif1 * dax_step;
			int bx = x1 + idif1 * dbx_step;

			float tex_su = u1 + idif1 * du1_step;
			float tex_sv = v1 + idif1 * dv1_step;
			float tex_sw = w1 + idif1 * dw1_step;

			float tex_eu = u1 + idif1 * du2_step;
			float tex_ev = v1 + idif1 * dv2_step;
			float tex_ew = w1 + idif1 * dw2_step;

			if (ax > bx)
			{
				SWAP(ax, bx);
				SWAP(tex_su, tex_eu);
				SWAP(tex_sv, tex_ev);
				SWAP(tex_sw, tex_ew);
			}

			tex_u = tex_su;
			tex_v = tex_sv;
			tex_w = tex_sw;

			float tstep = 1.0f / ((float)(bx - ax));
			float t = 0.0f, tv = 1.0f;

			if(ax < 0){
				t += tstep * LL3D_ABS(ax);
				ax = 0;
			}
			if(bx > LL3D_DepthBuffer->width){
				bx = LL3D_DepthBuffer->width;
			}
			tv = (1.0f - t);

			long poffset = (i*LL3D_DepthBuffer->width) + ax;
			
			for (int j = ax; j < bx; j++)
			{
				//float tv = (1.0f - t);
				tex_u = tv * tex_su + t * tex_eu;
				tex_v = tv * tex_sv + t * tex_ev;
				tex_w = tv * tex_sw + t * tex_ew;

				ll3d_int32  smpX = (float)(tex_u / tex_w) * (float)tex->width, 
							smpY = (float)(tex_v / tex_w) * (float)tex->height;
				if(smpX < 0) smpX = 0;
				if(smpY < 0) smpY = 0;
				if(smpX >= tex->width)  smpX = tex->width-1;
				if(smpY >= tex->height) smpY = tex->height-1;

				ll3d_uint8 mask = tex->SampleMaskFast(smpX,smpY);

				if(mask){
					if(poffset >= 0 && poffset < LL3D_PixelBuffer->fastsize){
						if (tex_w > LL3D_DepthBuffer->pixelbuffer[poffset])
						{
							ll3d_uint8 samp = tex->SampleTexFast(smpX,smpY);
							if(mask&0xFF){
								LL3D_DepthBuffer->pixelbuffer[poffset] = tex_w;
							}
							else{
								MK2D_RGB bcol = MK2D_FindPalRGB(LL3D_PixelBuffer->pixelbuffer[poffset]);
								MK2D_RGB fcol = MK2D_FindPalRGB(samp);
								MK2D_RGB newc = MK2D_AddRGB(bcol,fcol,mask);
								samp = MK2D_GetPalRGB(newc);
							}
							MK2D_RGB fcol = MK2D_FindPalRGB(samp);
							MK2D_RGB newc = MK2D_MultiplyRGB(color,fcol);
							samp = MK2D_GetPalRGB(newc);
							LL3D_PixelBuffer->pixelbuffer[poffset] = samp;
						}
					}
				}
				++poffset;
				t += tstep;
				tv -= tstep;
			}

		}
	}

	dy1 = y3 - y2;
	dx1 = x3 - x2;
	dv1 = v3 - v2;
	du1 = u3 - u2;
	dw1 = w3 - w2;

	du1_step = 0, dv1_step = 0;

	if (dy2) dbx_step = dx2 / (float)LL3D_ABS(dy2);	

	if (dy1) {
		dax_step = dx1 / (float)LL3D_ABS(dy1);
		du1_step = du1 / (float)LL3D_ABS(dy1);
		dv1_step = dv1 / (float)LL3D_ABS(dy1);
		dw1_step = dw1 / (float)LL3D_ABS(dy1);

		if(y3 >= LL3D_DepthBuffer->height){
			y3 = LL3D_DepthBuffer->height-1;
		}

		for (int i = y2; i <= y3; i++)
		{
			float idif1 = (float)(i - y1);
			float idif2 = (float)(i - y2);
			int ax = x2 + idif2 * dax_step;
			int bx = x1 + idif1 * dbx_step;

			float tex_su = u2 + idif2 * du1_step;
			float tex_sv = v2 + idif2 * dv1_step;
			float tex_sw = w2 + idif2 * dw1_step;

			float tex_eu = u1 + idif1 * du2_step;
			float tex_ev = v1 + idif1 * dv2_step;
			float tex_ew = w1 + idif1 * dw2_step;

			if (ax > bx)
			{
				SWAP(ax, bx);
				SWAP(tex_su, tex_eu);
				SWAP(tex_sv, tex_ev);
				SWAP(tex_sw, tex_ew);
			}

			tex_u = tex_su;
			tex_v = tex_sv;
			tex_w = tex_sw;

			float tstep = 1.0f / ((float)(bx - ax));
			float t = 0.0f, tv = 1.0f;

			if(ax < 0){
				t += tstep * LL3D_ABS(ax);
				ax = 0;
			}
			if(bx > LL3D_DepthBuffer->width){
				bx = LL3D_DepthBuffer->width;
			}

			tv = (1.0f - t);

			long poffset = (i*LL3D_DepthBuffer->width) + ax;
			for (int j = ax; j < bx; j++)
			{
				//float tv = (1.0f - t);
				tex_u = tv * tex_su + t * tex_eu;
				tex_v = tv * tex_sv + t * tex_ev;
				tex_w = tv * tex_sw + t * tex_ew;

				ll3d_int32  smpX = (float)(tex_u / tex_w) * (float)tex->width, 
							smpY = (float)(tex_v / tex_w) * (float)tex->height;
				if(smpX < 0) smpX = 0;
				if(smpY < 0) smpY = 0;
				if(smpX >= tex->width) smpX = tex->width-1;
				if(smpY >= tex->height) smpY = tex->height-1;

				ll3d_uint8 mask = tex->SampleMaskFast(smpX,smpY);

				if(mask){
					if(poffset >= 0 && poffset < LL3D_PixelBuffer->fastsize){
						if (tex_w > LL3D_DepthBuffer->pixelbuffer[poffset])
						{
							ll3d_uint8 samp = tex->SampleTexFast(smpX,smpY);
							if(mask&0xFF){
								LL3D_DepthBuffer->pixelbuffer[poffset] = tex_w;
							}
							else{
								MK2D_RGB bcol = MK2D_FindPalRGB(LL3D_PixelBuffer->pixelbuffer[poffset]);
								MK2D_RGB fcol = MK2D_FindPalRGB(samp);
								MK2D_RGB newc = MK2D_AddRGB(bcol,fcol,mask);
								samp = MK2D_GetPalRGB(newc);
							}
							MK2D_RGB fcol = MK2D_FindPalRGB(samp);
							MK2D_RGB newc = MK2D_MultiplyRGB(color,fcol);
							samp = MK2D_GetPalRGB(newc);
							LL3D_PixelBuffer->pixelbuffer[poffset] = samp;
						}
					}
				}
				++poffset;
				t += tstep;
				tv -= tstep;
			}
		}	
	}
};

void LL3D_TexturedTriangleChunky(	int x1, int y1, float u1, float v1, float w1,
												int x2, int y2, float u2, float v2, float w2,
												int x3, int y3, float u3, float v3, float w3,
												MK2D_Texture2D *tex, MK2D_RGB color)
{
	int yskip = 1;
	int xskip = 1;
	int chunkwidth = 0;

	int blockW = 1;
	if(LL3D_RasteringType==LL3D_RAST_NONE){
		return;
	}
	if(LL3D_RasteringType==LL3D_RAST_DITHER){
		yskip = 2;
		xskip = 2;
	}
	if(LL3D_RasteringType==LL3D_RAST_DITHER2){
		yskip = 4;
		xskip = 4;
	}
	if(LL3D_RasteringType==LL3D_RAST_2PPS){
		blockW = 2;
	}
	if(LL3D_RasteringType==LL3D_RAST_4PPS){
		blockW = 4;
	}
	if(LL3D_RasteringType==LL3D_RAST_6PPS){
		blockW = 6;
	}
	if(LL3D_RasteringType==LL3D_RAST_8PPS){
		blockW = 8;
	}
	if(blockW<1) blockW = 1; // No smaller than 1
	
	float SWAP_V = 0;
	if (y2 < y1)
	{
		SWAP(y1, y2);
		SWAP(x1, x2);
		SWAP(u1, u2);
		SWAP(v1, v2);
		SWAP(w1, w2);
	}

	if (y3 < y1)
	{
		SWAP(y1, y3);
		SWAP(x1, x3);
		SWAP(u1, u3);
		SWAP(v1, v3);
		SWAP(w1, w3);
	}

	if (y3 < y2)
	{
		SWAP(y2, y3);
		SWAP(x2, x3);
		SWAP(u2, u3);
		SWAP(v2, v3);
		SWAP(w2, w3);
	}

	int dy1 = y2 - y1;
	int dx1 = x2 - x1;
	float dv1 = v2 - v1;
	float du1 = u2 - u1;
	float dw1 = w2 - w1;

	int dy2 = y3 - y1;
	int dx2 = x3 - x1;
	float dv2 = v3 - v1;
	float du2 = u3 - u1;
	float dw2 = w3 - w1;

	float tex_u, tex_v, tex_w;

	float dax_step = 0, dbx_step = 0,
		du1_step = 0, dv1_step = 0,
		du2_step = 0, dv2_step = 0,
		dw1_step=0, dw2_step=0;


	if (dy2) {
		dbx_step = dx2 / (float)LL3D_ABS(dy2);
		du2_step = du2 / (float)LL3D_ABS(dy2);
		dv2_step = dv2 / (float)LL3D_ABS(dy2);
		dw2_step = dw2 / (float)LL3D_ABS(dy2);
	}

	if (dy1){ 
		dax_step = dx1 / (float)LL3D_ABS(dy1);
		du1_step = du1 / (float)LL3D_ABS(dy1);
		dv1_step = dv1 / (float)LL3D_ABS(dy1);
		dw1_step = dw1 / (float)LL3D_ABS(dy1);
		
		if(y2 >= LL3D_DepthBuffer->height){
			y2 = LL3D_DepthBuffer->height-1;
		}

		for (int i = y1; i <= y2; i+= yskip)
		{
			float idif1 = (float)(i - y1);

			int ax = x1 + idif1 * dax_step;
			int bx = x1 + idif1 * dbx_step;

			float tex_su = u1 + idif1 * du1_step;
			float tex_sv = v1 + idif1 * dv1_step;
			float tex_sw = w1 + idif1 * dw1_step;

			float tex_eu = u1 + idif1 * du2_step;
			float tex_ev = v1 + idif1 * dv2_step;
			float tex_ew = w1 + idif1 * dw2_step;

			if (ax > bx)
			{
				SWAP(ax, bx);
				SWAP(tex_su, tex_eu);
				SWAP(tex_sv, tex_ev);
				SWAP(tex_sw, tex_ew);
			}

			tex_u = tex_su;
			tex_v = tex_sv;
			tex_w = tex_sw;

			float tstep = 1.0f / ((float)(bx - ax));
			float t = 0.0f, tv = 1.0f;

			if(ax < 0){
				t += tstep * LL3D_ABS(ax);
				tv = (1.0f - t);
				ax = 0;
			}
			if(bx > LL3D_DepthBuffer->width){
				bx = LL3D_DepthBuffer->width;
			}

			long poffset = (i*LL3D_DepthBuffer->width) + ax;
	
			for (int j = ax; j < bx; j+=xskip)
			{
				//float tv = (1.0f - t);
				tex_u = tv * tex_su + t * tex_eu;
				tex_v = tv * tex_sv + t * tex_ev;
				tex_w = tv * tex_sw + t * tex_ew;

				ll3d_int32  smpX = (float)(tex_u / tex_w) * (float)tex->width, 
							smpY = (float)(tex_v / tex_w) * (float)tex->height;
				if(smpX < 0) smpX = 0;
				if(smpY < 0) smpY = 0;
				if(smpX >= tex->width)  smpX = tex->width-1;
				if(smpY >= tex->height) smpY = tex->height-1;
				ll3d_uint8 mask = tex->SampleMaskFast(smpX,smpY);

				if(mask){
					if (tex_w > LL3D_DepthBuffer->pixelbuffer[poffset])
					{
						ll3d_uint8 samp = tex->SampleTexFast(smpX,smpY);
						if(mask&0xFF){
							LL3D_DepthBuffer->pixelbuffer[poffset] = tex_w;
						}
						else{
							MK2D_RGB bcol = MK2D_FindPalRGB(LL3D_PixelBuffer->pixelbuffer[poffset]);
							MK2D_RGB fcol = MK2D_FindPalRGB(samp);
							MK2D_RGB newc = MK2D_AddRGB(bcol,fcol,mask);
							samp = MK2D_GetPalRGB(newc);
						}
						MK2D_RGB fcol = MK2D_FindPalRGB(samp);
						MK2D_RGB newc = MK2D_MultiplyRGB(color,fcol);
						samp = MK2D_GetPalRGB(newc);

						if(j + blockW < bx){
							if(i + blockW < y2){
								for(int vvv = 0; vvv < blockW; vvv++)
									memset(LL3D_PixelBuffer->pixelbuffer+poffset+(vvv*LL3D_PixelBuffer->width),samp,blockW);
							}else{
								memset(LL3D_PixelBuffer->pixelbuffer+poffset,samp,blockW);
							}
						}else{
							LL3D_PixelBuffer->pixelbuffer[poffset] = samp;
						}
					}
				}
				if(j + blockW < bx){
					int bw = blockW-1;
					j += bw;
					poffset += bw;
					t += tstep*bw;
					tv -= tstep*bw;
				}

				poffset += xskip;
				t += tstep*xskip;
				tv -= tstep*xskip;
			}
			if(i + blockW < y2){
				i += blockW-1;
			}
		}
	}

	dy1 = y3 - y2;
	dx1 = x3 - x2;
	dv1 = v3 - v2;
	du1 = u3 - u2;
	dw1 = w3 - w2;

	du1_step = 0, dv1_step = 0;

	if (dy2) dbx_step = dx2 / (float)LL3D_ABS(dy2);	

	if (dy1) {
		dax_step = dx1 / (float)LL3D_ABS(dy1);
		du1_step = du1 / (float)LL3D_ABS(dy1);
		dv1_step = dv1 / (float)LL3D_ABS(dy1);
		dw1_step = dw1 / (float)LL3D_ABS(dy1);

		if(y3 >= LL3D_DepthBuffer->height){
			y3 = LL3D_DepthBuffer->height-1;
		}

		for (int i = y2; i <= y3; i+= yskip)
		{
			float idif1 = (float)(i - y1);
			float idif2 = (float)(i - y2);
			int ax = x2 + idif2 * dax_step;
			int bx = x1 + idif1 * dbx_step;

			float tex_su = u2 + idif2 * du1_step;
			float tex_sv = v2 + idif2 * dv1_step;
			float tex_sw = w2 + idif2 * dw1_step;

			float tex_eu = u1 + idif1 * du2_step;
			float tex_ev = v1 + idif1 * dv2_step;
			float tex_ew = w1 + idif1 * dw2_step;

			if (ax > bx)
			{
				SWAP(ax, bx);
				SWAP(tex_su, tex_eu);
				SWAP(tex_sv, tex_ev);
				SWAP(tex_sw, tex_ew);
			}

			tex_u = tex_su;
			tex_v = tex_sv;
			tex_w = tex_sw;

			float tstep = 1.0f / ((float)(bx - ax));
			float t = 0.0f, tv = 1.0f;

			if(ax < 0){
				t += tstep * LL3D_ABS(ax);
				tv = (1.0f - t);
				ax = 0;
			}
			if(bx > LL3D_DepthBuffer->width){
				bx = LL3D_DepthBuffer->width;
			}

			long poffset = (i*LL3D_DepthBuffer->width) + ax;
			for (int j = ax; j < bx; j+=xskip)
			{
				//float tv = (1.0f - t);
				tex_u = tv * tex_su + t * tex_eu;
				tex_v = tv * tex_sv + t * tex_ev;
				tex_w = tv * tex_sw + t * tex_ew;

				ll3d_int32  smpX = (float)(tex_u / tex_w) * (float)tex->width, 
							smpY = (float)(tex_v / tex_w) * (float)tex->height;
				if(smpX < 0) smpX = 0;
				if(smpY < 0) smpY = 0;
				if(smpX >= tex->width) smpX = tex->width-1;
				if(smpY >= tex->height) smpY = tex->height-1;

				ll3d_uint8 mask = tex->SampleMaskFast(smpX,smpY);

				if(mask){
					//if (tex_w > LL3D_DepthBuffer->pixelbuffer[poffset])
					{
						ll3d_uint8 samp = tex->SampleTexFast(smpX,smpY);
						if(mask==0xFF){
							MK2D_RGB bcol = MK2D_FindPalRGB(LL3D_PixelBuffer->pixelbuffer[poffset]);
							MK2D_RGB fcol = MK2D_FindPalRGB(samp);
							MK2D_RGB newc = MK2D_AddRGB(bcol,fcol,mask);
							samp = MK2D_GetPalRGB(newc);
						}
						MK2D_RGB fcol = MK2D_FindPalRGB(samp);
						MK2D_RGB newc = MK2D_MultiplyRGB(color,fcol);
						samp = MK2D_GetPalRGB(newc);
						if(j + blockW < bx){
							if(i + blockW < y3){
								for(int vvv = 0; vvv < blockW; vvv++){
									memset(LL3D_PixelBuffer->pixelbuffer+poffset+(vvv*LL3D_PixelBuffer->width),samp,blockW);
									/*if(mask==0xFF){
										for(int mm = 0; mm < blockW; mm ++)
											LL3D_DepthBuffer->pixelbuffer[poffset+(vvv*LL3D_PixelBuffer->width)+mm] = tex_w;
									}*/
								}
							}else{
								memset(LL3D_PixelBuffer->pixelbuffer+poffset,samp,blockW);
								/*
								if(mask==0xFF){
									for(int mm = 0; mm < blockW; mm ++)
										LL3D_DepthBuffer->pixelbuffer[poffset+mm] = tex_w;
								}*/
							}
						}else{
							LL3D_PixelBuffer->pixelbuffer[poffset] = samp;
//							LL3D_DepthBuffer->pixelbuffer[poffset] = tex_w;
						}
					}
				}
				if(j + blockW < bx){
					int bw = blockW-1;
					j += bw;
					poffset += bw;
					t += tstep*bw;
					tv -= tstep*bw;
				}

				poffset += xskip;
				t += tstep*xskip;
				tv -= tstep*xskip;
			}
			if(i + blockW < y3){
				i += blockW-1;
			}
		}	
	}

};


void LL3D_FillDepthTriangle(	int x1, int y1, float w1,
								int x2, int y2, float w2,
								int x3, int y3, float w3,
								unsigned char col)
{
	float SWAP_V = 0;
	if (y2 < y1)
	{
		SWAP(y1, y2);
		SWAP(x1, x2);
		SWAP(w1, w2);
	}

	if (y3 < y1)
	{
		SWAP(y1, y3);
		SWAP(x1, x3);
		SWAP(w1, w3);
	}

	if (y3 < y2)
	{
		SWAP(y2, y3);
		SWAP(x2, x3);
		SWAP(w2, w3);
	}

	int dy1 = y2 - y1;
	int dx1 = x2 - x1;
	float dw1 = w2 - w1;

	int dy2 = y3 - y1;
	int dx2 = x3 - x1;
	float dw2 = w3 - w1;

	float tex_w;

	float dax_step = 0, dbx_step = 0,
		dw1_step=0, dw2_step=0;

	if (dy2){
		dbx_step = dx2 / (float)LL3D_ABS(dy2);
		dw2_step = dw2 / (float)LL3D_ABS(dy2);
	}
	if (dy1) {
		dax_step = dx1 / (float)LL3D_ABS(dy1);
		dw1_step = dw1 / (float)LL3D_ABS(dy1);
		if(y2 >= LL3D_DepthBuffer->height){
			y2 = LL3D_DepthBuffer->height-1;
		}
		for (int i = y1; i <= y2; i++)
		{
			float idif1 = (float)(i - y1);
			int ax = x1 + idif1 * dax_step;
			int bx = x1 + idif1 * dbx_step;

			float tex_sw = w1 + idif1 * dw1_step;
			float tex_ew = w1 + idif1 * dw2_step;

			if (ax > bx)
			{
				SWAP(ax, bx);
				SWAP(tex_sw, tex_ew);
			}

			tex_w = tex_sw;

			float tstep = 1.0f / ((float)(bx - ax));
			float t = 0.0f;
			
			if(ax < 0){
				t += tstep * LL3D_ABS(ax);
				ax = 0;
			}
			if(bx > LL3D_DepthBuffer->width){
				bx = LL3D_DepthBuffer->width;
			}

			long poffset = (i*LL3D_DepthBuffer->width) + ax;
			for (int j = ax; j < bx; j++)
			{
				tex_w = (1.0f - t) * tex_sw + t * tex_ew;
				
				//if(poffset >= 0 && poffset < LL3D_PixelBuffer->fastsize){
					if (tex_w > LL3D_DepthBuffer->pixelbuffer[poffset])
					{
						LL3D_PixelBuffer->pixelbuffer[poffset] = col;
						LL3D_DepthBuffer->pixelbuffer[poffset] = tex_w;
					}

				//}
				++poffset;

				t += tstep;
			}

		}
	}

	dy1 = y3 - y2;
	dx1 = x3 - x2;
	dw1 = w3 - w2;

	if (dy2) dbx_step = dx2 / (float)LL3D_ABS(dy2);

	if (dy1) {
		dax_step = dx1 / (float)LL3D_ABS(dy1);
		dw1_step = dw1 / (float)LL3D_ABS(dy1);
		if(y3 >= LL3D_DepthBuffer->height){
			y3 = LL3D_DepthBuffer->height-1;
		}
		for (int i = y2; i <= y3; i++)
		{
			float idif1 = (float)(i - y1);
			float idif2 = (float)(i - y2);

			int ax = x2 + idif2 * dax_step;
			int bx = x1 + idif1 * dbx_step;

			float tex_sw = w2 + idif2 * dw1_step;
			float tex_ew = w1 + idif1 * dw2_step;

			if (ax > bx)
			{
				SWAP(ax, bx);
				SWAP(tex_sw, tex_ew);
			}

			tex_w = tex_sw;

			float tstep = 1.0f / ((float)(bx - ax));
			float t = 0.0f;

			if(ax < 0){
				t += tstep * LL3D_ABS(ax);
				ax = 0;
			}
			if(bx > LL3D_DepthBuffer->width){
				bx = LL3D_DepthBuffer->width;
			}

			long poffset = (i*LL3D_DepthBuffer->width) + ax;
			for (int j = ax; j < bx; j++)
			{
				tex_w = (1.0f - t) * tex_sw + t * tex_ew;

				//if(poffset >= 0 && poffset < LL3D_PixelBuffer->fastsize){
					if (tex_w > LL3D_DepthBuffer->pixelbuffer[poffset])
					{
						LL3D_PixelBuffer->pixelbuffer[poffset] = col;
						LL3D_DepthBuffer->pixelbuffer[poffset] = tex_w;
					}
				//}
				++poffset;
				t += tstep;
			}
		}	
	}	
};


extern LL3D_Camera * ProjectionCamera;

extern LL3D_Matrix4x4 matView;
extern LL3D_Matrix4x4 matWorld;

extern LL3D_Vector3D WorldTransform;

void LL3D_RenderLine(LL3D_Vector3D &p1, LL3D_Vector3D &p2, unsigned char col)
{
	// Coordinates are assumed to be in world space
	LL3D_Vector3D t1, t2;

	// Transform into world space
	t1 = Matrix_MultiplyVector(matWorld, p1);
	t2 = Matrix_MultiplyVector(matWorld, p2);
	
	// Transform into view
	t1 = Matrix_MultiplyVector(matView, t1);
	t2 = Matrix_MultiplyVector(matView, t2);

	// Project onto screen
	t1 = Matrix_MultiplyVector(ProjectionCamera->matProj, t1);
	t2 = Matrix_MultiplyVector(ProjectionCamera->matProj, t2);

	// Project
	t1.x = t1.x / t1.w;
	t1.y = t1.y / t1.w;
	t1.z = t1.z / t1.w;

	t2.x = t2.x / t2.w;
	t2.y = t2.y / t2.w;
	t2.z = t2.z / t2.w;

	t1.x *= -1.0f;
	t2.x *= -1.0f;
	t1.y *= -1.0f;
	t2.y *= -1.0f;
	
	LL3D_Vector3D vOffsetView;
	vOffsetView.Set(1,1,0);
	t1 = Vector_Add(t1, vOffsetView);
	t2 = Vector_Add(t2, vOffsetView);
	
	t1.x *= LL3D_PixelBuffer->half_width;
	t1.y *= LL3D_PixelBuffer->half_height;
	t2.x *= LL3D_PixelBuffer->half_width;
	t2.y *= LL3D_PixelBuffer->half_height;
	
//	vOffsetView.Set(fViewX,fViewY,0);
//	t1 = Vector_Add(t1, vOffsetView);
//	t2 = Vector_Add(t2, vOffsetView);

	LL3D_DrawLine(t1.x, t1.y, t2.x, t2.y, col);
	
};

void LL3D_RenderCircle(LL3D_Vector3D &p1, float r, unsigned char col)
{
	// Coordinates are assumed to be in world space
	LL3D_Vector3D t1;
	LL3D_Vector3D t2;
	t2.Set(p1.x + r, p1.y, p1.z );

	// Transform into world space
	t1 = Matrix_MultiplyVector(matWorld, p1);
	t2 = Matrix_MultiplyVector(matWorld, t2);
	
	// Transform into view
	t1 = Matrix_MultiplyVector(matView, t1);
	t2 = Matrix_MultiplyVector(matView, t2);

	// Project onto screen
	t1 = Matrix_MultiplyVector(ProjectionCamera->matProj, t1);
	t2 = Matrix_MultiplyVector(ProjectionCamera->matProj, t2);

	// Project
	if(t1.w != 0.0f){
		t1.x = t1.x / t1.w;
		t1.y = t1.y / t1.w;
		t1.z = t1.z / t1.w;
	}else{
		return; // No circle
	}

	if(t2.w != 0.0f){
		t2.x = t2.x / t2.w;
		t2.y = t2.y / t2.w;
		t2.z = t2.z / t2.w;
	}else{
		return; // No circle
	}


	t1.x *= -1.0f;
	t2.x *= -1.0f;
	t1.y *= -1.0f;
	t2.y *= -1.0f;
	
	LL3D_Vector3D vOffsetView;
	vOffsetView.Set(1,1,0);
	t1 = Vector_Add(t1, vOffsetView);
	t2 = Vector_Add(t2, vOffsetView);
	
	t1.x *= LL3D_PixelBuffer->half_width;
	t1.y *= LL3D_PixelBuffer->half_height;
	t2.x *= LL3D_PixelBuffer->half_width;
	t2.y *= LL3D_PixelBuffer->half_height;

//	vOffsetView = { fViewX,fViewY,0 };
//	t1 = Math::Vec_Add(t1, vOffsetView);
//	t2 = Math::Vec_Add(t2, vOffsetView);

	LL3D_FillCircle(t1.x, t1.y, LL3D_ABS(t2.x - t1.x), col);

};

