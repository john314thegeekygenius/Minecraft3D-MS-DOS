// Llama 3D Graphics Header

#ifndef __LLAMA_GFX__
#define __LLAMA_GFX__


typedef enum {
	LL3D_REND_NONE = 0,
	LL3D_REND_WIRE = 1,
	LL3D_REND_SOLID_NDEPTH = 2,
	LL3D_REND_SOLID = 3,
	LL3D_REND_TEX = 4,
}LL3D_RenderType;

typedef enum {
	LL3D_RAST_NONE = 0, // ???
	LL3D_RAST_DITHER = 1,
	LL3D_RAST_DITHER2 = 2,
	LL3D_RAST_SOLID = 3,
	LL3D_RAST_2PPS = 4,
	LL3D_RAST_4PPS = 5,
	LL3D_RAST_6PPS = 6,
	LL3D_RAST_8PPS = 7,
}LL3D_RasterType;

#define LL3D_FLAG_RENDER_CW     0x01
#define LL3D_FLAG_RENDER_CCW    0x02
#define LL3D_FLAG_RENDER_LIGHTS 0x04


typedef struct PixelBuffer_f {
	ll3d_uint32 width;
	ll3d_uint32 height;
	ll3d_uint32 half_width;
	ll3d_uint32 half_height;
	ll3d_uint32 fastsize;
	ll3d_float32 aspect;
	ll3d_float32 *pixelbuffer;

	inline void OnCreateStruct(){
		width = height = half_width = half_height = fastsize = 0;
		pixelbuffer = NULL;
	}

	inline bool Create(int w,int h){
		if(pixelbuffer){
			free(pixelbuffer);
			pixelbuffer = NULL;
		}
		pixelbuffer = (ll3d_float32 *)malloc(w*h*sizeof(ll3d_float32));
		if(!pixelbuffer){
			LL3D_ThrowError("Could not allocate new depth buffer!\n");
			return false;
		}
		width = w; 
		height = h; 

		fastsize = w*h*sizeof(ll3d_float32);
		
		half_width = w / 2; 
		half_height = h / 2;
		
		aspect = (float)h / (float)w;

		memset(pixelbuffer,0,w*h*sizeof(ll3d_float32)); // Clear the buffer for fun
		return true;
	};

	inline void Free(){
		if(pixelbuffer){
			free(pixelbuffer);
			pixelbuffer = NULL;
		}
	};
}PixelBuffer_f;

bool LL3D_SetRenderer(MK2D_PixelBuffer &buffer,PixelBuffer_f &depth);

void LL3D_FillCircle(int cx,int cy,int radius,unsigned char color);
void LL3D_DrawLine(int x1, int y1, int x2, int y2, unsigned char col);

void LL3D_DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, unsigned char c);
void LL3D_FillTriangle(int x1,int y1,int x2,int y2,int x3, int y3,int col);
void LL3D_TexturedTriangle(	int x1, int y1, float u1, float v1, float w1,
							int x2, int y2, float u2, float v2, float w2,
							int x3, int y3, float u3, float v3, float w3,
							MK2D_Texture2D *tex, MK2D_RGB color);

void LL3D_TexturedTriangleChunky(	int x1, int y1, float u1, float v1, float w1,
							int x2, int y2, float u2, float v2, float w2,
							int x3, int y3, float u3, float v3, float w3,
							MK2D_Texture2D *tex, MK2D_RGB color);
void LL3D_FillDepthTriangle(	int x1, int y1, float w1,
								int x2, int y2, float w2,
								int x3, int y3, float w3,
								unsigned char col);


// Draw 2D Objects in a 3D World
void LL3D_RenderLine(LL3D_Vector3D &p1, LL3D_Vector3D &p2, unsigned char col);
void LL3D_RenderCircle(LL3D_Vector3D &p1, float r, unsigned char col);

#endif
