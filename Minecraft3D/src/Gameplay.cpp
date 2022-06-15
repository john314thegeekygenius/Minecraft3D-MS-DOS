

#include "Minecraft.h"

bool MC_GeneratedChunksYet = false;

extern int MC_MouseInverted;
extern int MC_ScreenScale;
extern bool MC_OverlayOpen;

extern LL3D_Mesh MC_WorldMesh;

MK2D_PixelBuffer MC_3DBuffer;
PixelBuffer_f MC_3DDepthBuffer;
int BufferOffsetX,BufferOffsetY;

LL3D_Camera MC_PlayerCamera;

void MC_Setup3DStuff(void){
	LL3D_SetRenderer(MC_3DBuffer,MC_3DDepthBuffer);

	MC_PlayerCamera.Reset();

	MC_3DBuffer.OnCreateStruct();
	MC_3DDepthBuffer.OnCreateStruct();
	MC_3DBuffer.Create(320,200);
	MC_3DDepthBuffer.Create(320,200);

	BufferOffsetX = (320 - MC_3DDepthBuffer.width) >> 1;
	BufferOffsetY = (200 - MC_3DDepthBuffer.height) >> 1;


	MC_PlayerCamera.SetMat(60.0f,MC_3DBuffer.aspect);

	MC_PlayerCamera.rotation.x = LL3D_PI * 2;
	LL3D_SetWorldCamera(MC_PlayerCamera);
	LL3D_ProjectCamera();

	// Setup the texture stuff
	MC_UpdateTextureScale();
};

void MC_Shutdown3DStuff(void){
	MC_3DDepthBuffer.Free();
	MC_3DBuffer.Free();
};


void MC_UpdateChunks(void){
	// Regenerate the entire mesh

	// Remove the old scene mesh
	LL3D_ClearScene();
	MC_ClearWorldMesh();
	// Generate the mesh
	for(int x = 0; x < 4; x++){
		for(int z = 0; z < 4; z++){
			MC_LightChunk(x,z); // Update the light
			MC_GenerateChunkMesh(x,z);
		}
	}
};


void MC_CreateGameChunks(){
	MC_ClearWorldChunks();

	// Generate the world
	for(int x = 0; x < 4; x++){
		for(int z = 0; z < 4; z++){
			MC_GenerateWorldChunk(x,z);
		}
	}

	MC_UpdateChunks();
	// Rotate the world
//	LL3D_Vector3D rotationV;
//	rotationV.Set(0,LL3D_PI,0);
//	LL3D_RotateMesh(MC_WorldMesh,rotationV);

};

extern bool MC_InGame;
extern bool MC_GamePaused;

const float fElapsedTime = 0.05;

unsigned short MC_GamePlaceBlockId = 1;

void MC_UpdateInput(void){

	int key_hit = MK2D_KeyReleased();

	bool UpdateCamera = false;
	bool UpdateScreen = false;

	if(key_hit==mkey_escape || key_hit == mkey_tab){
		MC_SetMenuPause();
		MC_GamePaused = true;
		MC_OverlayOpen = true;
	}

	if(key_hit == mkey_g_pageup){
		MC_GamePlaceBlockId += 1;
	}
	if(key_hit == mkey_g_pagedown){
		MC_GamePlaceBlockId -= 1;
	}
	if(MC_GamePlaceBlockId < 1) MC_GamePlaceBlockId = 1;
	if(MC_GamePlaceBlockId>=MC_NUM_OF_BLOCKS) MC_GamePlaceBlockId = MC_NUM_OF_BLOCKS-1;

	// Change the screen size
	if(key_hit==( mkey_minus)){
		MC_ScreenScale += 1;
		if(MC_ScreenScale > 10){
			MC_ScreenScale = 10;
		}else{
			UpdateScreen = true;
		}
	}

	if(key_hit==(mkey_equals)){
		MC_ScreenScale -= 1;
		if(MC_ScreenScale<2){
			MC_ScreenScale = 2;
		}else{
			UpdateScreen = true;
		}
	}

	if(UpdateScreen){
		MC_3DDepthBuffer.Free();
		MC_3DBuffer.Free();
		MC_3DBuffer.Create(
			(int)(320.0f/((float)MC_ScreenScale/2.0f)),
			(int)(200.0f/((float)MC_ScreenScale/2.0f)));
		MC_3DDepthBuffer.Create(
			(int)(320.0f/((float)MC_ScreenScale/2.0f)),
			(int)(200.0f/((float)MC_ScreenScale/2.0f)));
		BufferOffsetX = (320 - MC_3DDepthBuffer.width) >> 1;
		BufferOffsetY = (200 - MC_3DDepthBuffer.height) >> 1;
		MC_PlayerCamera.SetMat(60.0f,MC_3DBuffer.aspect);

		UpdateCamera = true;
	}

	if(!MC_OverlayOpen){
		LL3D_Vector3D vForward = Vector_Mul(MC_PlayerCamera.vLookDir, 8.0f * fElapsedTime);

		// Standard FPS Control scheme, but turn instead of strafe
		if(MK2D_IsKey( mkey_w)){
			MC_PlayerCamera.position = Vector_Add(MC_PlayerCamera.position, vForward);
			UpdateCamera = true;
		}

		if(MK2D_IsKey(mkey_s)){
			MC_PlayerCamera.position = Vector_Sub(MC_PlayerCamera.position, vForward);
			UpdateCamera = true;
		}

		if(Monkey_Mouse.delta_x!=0){
			MC_PlayerCamera.rotation.y += Monkey_Mouse.delta_x * fElapsedTime;
			UpdateCamera = true;
		}
		if(Monkey_Mouse.delta_y!=0){
			if(MC_MouseInverted)
				MC_PlayerCamera.rotation.x -= Monkey_Mouse.delta_y * fElapsedTime;
			else
				MC_PlayerCamera.rotation.x += Monkey_Mouse.delta_y * fElapsedTime;
			UpdateCamera = true;
		}

		if(MK2D_IsKey(mkey_r)){
			MC_PlayerCamera.rotation.x -= 2.0f * fElapsedTime;
			UpdateCamera = true;
		}

		if(MK2D_IsKey(mkey_f)){
			MC_PlayerCamera.rotation.x += 2.0f * fElapsedTime;
			UpdateCamera = true;
		}

		if(MK2D_IsKey(mkey_a)){
			MC_PlayerCamera.rotation.y -= 2.0f * fElapsedTime;
			UpdateCamera = true;
		}

		if(MK2D_IsKey(mkey_d)){
			MC_PlayerCamera.rotation.y += 2.0f * fElapsedTime;
			UpdateCamera = true;
		}

		if(MK2D_IsKey(mkey_z)){
			MC_PlayerCamera.rotation.z -= 2.0f * fElapsedTime;
			UpdateCamera = true;
		}

		if(MK2D_IsKey(mkey_c)){
			MC_PlayerCamera.rotation.z += 2.0f * fElapsedTime;
			UpdateCamera = true;
		}

		if(MK2D_IsKey(mkey_e)){
			MC_PlayerCamera.position.y += 8.0f * fElapsedTime;	// Travel Upwards
			UpdateCamera = true;
		}
		if(MK2D_IsKey(mkey_q)){
			MC_PlayerCamera.position.y -= 8.0f * fElapsedTime;	// Travel Downwards
			UpdateCamera = true;
		}
	}
	if(UpdateCamera){
		//MC_PlayerCamera.clamp();
		LL3D_ProjectCamera();
	}

	if(key_hit == mkey_0){
		LL3D_SetRenderType(LL3D_REND_NONE);
	}
	if(key_hit == mkey_1){
		LL3D_SetRenderType(LL3D_REND_WIRE);
	}
	if(key_hit == mkey_2){
		LL3D_SetRenderType(LL3D_REND_SOLID_NDEPTH);
	}
	if(key_hit == mkey_3){
		LL3D_SetRenderType(LL3D_REND_SOLID);
	}
	if(key_hit == mkey_4){
		LL3D_SetRenderType(LL3D_REND_TEX);
	}

	if(key_hit == mkey_0){
		LL3D_SetRasterType(LL3D_RAST_NONE);
	}
	if(key_hit == mkey_5){
		LL3D_SetRasterType(LL3D_RAST_DITHER);
	}
	if(key_hit == mkey_6){
		LL3D_SetRasterType(LL3D_RAST_SOLID);
	}
	if(key_hit == mkey_7){
		LL3D_SetRasterType(LL3D_RAST_DITHER2);
	}
	if(key_hit == mkey_8){
		LL3D_SetRasterType(LL3D_RAST_2PPS);
	}
	if(key_hit == mkey_9){
		LL3D_SetRasterType(LL3D_RAST_4PPS);
	}

};

void MC_GameLoop(void){
	// Make sure the chunks are there!
	if(MC_GeneratedChunksYet == false){
		MC_CreateGameChunks();
		MC_GeneratedChunksYet = true;
	}

	// Read Input

	MC_UpdateInput();

	// Update the tick

	// Update the chunks

	// Update the entitys

	// Play the sounds

	// Render the chunks

	// Project the triangles
	//LL3D_ProjectObjects();
	// Render the triangles
	//LL3D_RenderObjects();

	// Render the scene
	LL3D_ClearRender(15);// Blue sky
	LL3D_RenderScene(LL3D_FLAG_RENDER_CW | LL3D_FLAG_RENDER_LIGHTS,
						MC_WorldMesh);
	
	LL3D_Vector3D RayC;
	RayC = MC_PlayerCamera.position;
	LL3D_Vector3D raycF = MC_PlayerCamera.vLookDir;//Vector_Mul(MC_PlayerCamera.vLookDir, 1.0f);
	//std::cout << raycF.x << ", " << raycF.y << ", " << raycF.z << std::endl;
	for(int c = 0; c < 4; c++){
		RayC = Vector_Add(RayC, raycF);
		// Test if there is a block there
		if(MC_GetWorldBlock((int)RayC.x,(int)RayC.y,(int)RayC.z)){
			c = 5;
			if(Monkey_Mouse.left_button){
			//	std::cout << RayC.x << ", " << RayC.y << ", " << RayC.z << std::endl;
				MC_SetWorldBlock((int)RayC.x,(int)RayC.y,(int)RayC.z, "air");
				MC_UpdateChunks();
			}
			if(Monkey_Mouse.right_button){
				// Previous location (Not the correct way to do this)
				RayC = Vector_Sub(RayC, raycF);
			//	std::cout << RayC.x << ", " << RayC.y << ", " << RayC.z << std::endl;
				MC_SetWorldBlock((int)RayC.x,(int)RayC.y,(int)RayC.z, MC_GamePlaceBlockId);
				MC_UpdateChunks();
			}
		}
	}
	LL3D_RenderLine(MC_PlayerCamera.position,RayC,6);

	// Display the rendered chunks
	MK2D_BlitMem2Mem(MK_ScreenBuffer.pixelbuffer, MK_ScreenBuffer.width, MK_ScreenBuffer.height, 
					MC_3DBuffer.pixelbuffer,BufferOffsetX,BufferOffsetY, 
					MC_3DBuffer.width, MC_3DBuffer.height);


	extern MK2D_Texture2D MC_BlocksAtlas;
	int AtID = MC_BlockTexturesOffsets[MC_GamePlaceBlockId][2];
	int AtSize = MC_BlocksAtlas.width / 16;
	MK2D_RenderAATexture2D(MC_BlocksAtlas,
							MK_ScreenBuffer.width-AtSize,0,
							(AtID - ((AtID/16)*16))*AtSize,(AtID / 16)*AtSize,
							AtSize,AtSize);

	// Draw the UI
	if(MC_OverlayOpen)
		MC_DrawDarkenOverlay(); // Darken the background

	MK2D_SetRenderBlend(MK2D_BLEND_ALPHA);
	MC_Write(LL3D_GetNumOfTris(),0,10,MK2D_WHITE);
	MC_Write(LL3D_GetNumOfRasterTris(),0,20,MK2D_WHITE);
	MC_Write((float)MC_PlayerCamera.position.x,0,30,MK2D_WHITE);
	MC_Write((float)MC_PlayerCamera.position.y,32,30,MK2D_WHITE);
	MC_Write((float)MC_PlayerCamera.position.z,64,30,MK2D_WHITE);
	MC_Write((float)MC_PlayerCamera.rotation.x*LL3D_Rad2DEG,0,40,MK2D_WHITE);
	MC_Write((float)MC_PlayerCamera.rotation.y*LL3D_Rad2DEG,32,40,MK2D_WHITE);
	MC_Write((float)MC_PlayerCamera.rotation.z*LL3D_Rad2DEG,64,40,MK2D_WHITE);
	MK2D_SetRenderBlend(MK2D_BLEND_NONE);

};

