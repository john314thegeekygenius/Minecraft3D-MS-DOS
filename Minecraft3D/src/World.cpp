
#include "Minecraft.h"

extern MK2D_Texture2D MC_BlocksAtlas;

float MC_TextureAtlasScale = 0.0f;

LL3D_Mesh MC_WorldMesh;

extern int MC_BrightnessValue;

long MC_WorldSeed = 0l;

void MC_UpdateTextureScale(void){
	MC_TextureAtlasScale = 16.0f / 256.0f;//(float)MC_BlocksAtlas.width;
	std::cout << "Set texture scale to " << MC_TextureAtlasScale << std::endl;
};

void MC_SetWorldSeed(long seed){
	MC_WorldSeed = seed;
};

void MC_GenerateChunk(Chunk &chunk){
	BlockState block;

	// Bedrock floor
	block.setup(7,false,0,'n',0,0); // Bedrock
	for(int x = 0; x < MC_SUBCHUNK_W; x++){
		for(int z = 0; z < MC_SUBCHUNK_D; z++){
			chunk.sub_chunk[0].blocks[x][0][z] = block;
		}
	}

	int Yrng = (rand() % 2) - 1;
	for(int y = 1; y < MC_SUBCHUNK_H * MC_CHUNK_H; y++){
		if(y < 40+Yrng) {
			block.setup(1,false,0,'n',0,0); // stone
		}else{
			if(y >= 45 && y < 48+Yrng){
				block.setup(3,false,0,'n',0,0); // dirt
			}else
			if(y == 48+Yrng){
				block.setup(2,false,0,'n',0,0); // grass
			}else{
				block.setup(0,false,0,'n',0,0); // air				
			}
		}
		for(int x = 0; x < MC_SUBCHUNK_W; x++){
			for(int z = 0; z < MC_SUBCHUNK_D; z++){
				if(y < 4){
					float rng = (float)(rand() % 100) / 100.0f;
					if(rng > 0.5){
						block.setup(7,false,0,'n',0,0); // Bedrock
					}else{
						block.setup(1,false,0,'n',0,0); // stone				
					}
				}
				if(y >= 40-Yrng && y < 45){
					float rng = (float)(rand() % 100) / 100.0f;
					if(rng > 0.25){
						block.setup(3,false,0,'n',0,0); // dirt
					}else{
						block.setup(1,false,0,'n',0,0); // stone	
					}
				}
				chunk.sub_chunk[(y / MC_SUBCHUNK_H)].blocks[x][y%MC_SUBCHUNK_H][z] = block;
			}
		}
	}
};

void MC_GenerateFlatChunk(Chunk &chunk,std::string chunkblocks){
	BlockState block;
	int stroff = 0;
	int blockcount = 0;
	int blocksdone = 0;
	bool done = false;
	std::string blockname;

	for(int y = 0; y < MC_SUBCHUNK_H * MC_CHUNK_H; y++){
		if(blocksdone==blockcount){
			blockname = "";
			if(stroff>=chunkblocks.length()){
				if(!done){
					block.setup(0,false,0,'n',0,0); // Fill with air
					done = true;
				}
			}
			if(!done){
				while(chunkblocks.c_str()[stroff]!=':')
					blockname += chunkblocks.c_str()[stroff++];
				stroff += 1;
				blockcount = 0;
				while(chunkblocks.c_str()[stroff]!=';'&&stroff<chunkblocks.length()){
					blockcount *= 10;
					blockcount += chunkblocks.c_str()[stroff++] - '0';
				}
				stroff += 1;
				blocksdone = 0;
				int blockid = MC_FindBlockID(blockname);
				block.setup(blockid,false,0,'n',0,0);
//				printf("Found Block %s -> %i\n",blockname.c_str(),blockid);
//				printf("T:%i\n",blockcount);
			}
		}
		for(int x = 0; x < MC_SUBCHUNK_W; x++){
			for(int z = 0; z < MC_SUBCHUNK_D; z++){
				chunk.sub_chunk[(y / MC_SUBCHUNK_H)].blocks[x][y%MC_SUBCHUNK_H][z] = block;
			}
		}
		blocksdone += 1;
	}
};


void MC_GenerateSillyChunk(Chunk &chunk){
	BlockState 	block,
				block2,
				block3,
				airblock;
	block.setup(MC_FindBlockID("grass_block"),false,0,'n',0,0);
	block2.setup(MC_FindBlockID("glass"),false,0,'n',0,0);
	block3.setup(MC_FindBlockID("lava"),false,0,'n',0,0);
	airblock.setup(MC_FindBlockID("air"),false,0,'n',0,0);

	// Reset the chunk with air
	for(int y = 0; y < MC_SUBCHUNK_H * MC_CHUNK_H; y++){
		for(int x = 0; x < MC_SUBCHUNK_W; x++){
			for(int z = 0; z < MC_SUBCHUNK_D; z++){
				chunk.sub_chunk[(y / MC_SUBCHUNK_H)].blocks[x][y%MC_SUBCHUNK_H][z] = airblock;
			}
		}
	}
//	memset(chunk.sub_chunk[0].blocks,0,sizeof(chunk.sub_chunk[0].blocks));
	for(int y = 0; y < MC_SUBCHUNK_H; y++){
		for(int x = 0; x <= y; x++){
			if(y < 4){
				for(int z = 0; z <= y; z++){
				chunk.sub_chunk[0].blocks[x][y][z] = block3;
				}
			}else{
				for(int z = 0; z <= y; z++){
				chunk.sub_chunk[0].blocks[x][y][z] = block2;
				}
			}
			chunk.sub_chunk[0].blocks[x][y][y] = block;
		}
	}
};

///////////////////////////////
// Minecraft World Stuff

std::vector<Chunk> MC_MinecraftWorld;
Chunk nullchunk;

std::string MC_WorldGenerationString = "bedrock:1;dirt:3;grass_block:1;";

// Removes all the chunks
void MC_ClearWorldChunks(void){
	MC_MinecraftWorld.clear();
};

// Generates a world chunk at cx, cy
void MC_GenerateWorldChunk(int cx,int cz){
	MC_MinecraftWorld.push_back(nullchunk);
	MC_MinecraftWorld.back().sub_chunk.resize(MC_CHUNK_H);
	MC_MinecraftWorld.back().x = cx;
	MC_MinecraftWorld.back().z = cz;
//	MC_GenerateChunk(MC_MinecraftWorld.back());
//	MC_GenerateSillyChunk(MC_MinecraftWorld.back());
	MC_GenerateFlatChunk(MC_MinecraftWorld.back(),MC_WorldGenerationString);
};

int MC_GetMinecraftChunk(int cx,int cz){
	for(int i = 0; i < MC_MinecraftWorld.size(); i++){
		if(MC_MinecraftWorld[i].x == cx &&
			MC_MinecraftWorld[i].z == cz){
			return i;
		}
	}
	return -1;
};

void MC_LightChunk(int cx,int cz){
	int chunkid = MC_GetMinecraftChunk(cx,cz);
	if(chunkid==-1) return; // Bad chunk to light
	Chunk &chunk = MC_MinecraftWorld[chunkid];
	int lightBrightness = 15;
	int blockBrightness = 0;

	#define MCBLOCK(X,Y,Z) chunk.sub_chunk[((Y) / MC_SUBCHUNK_H)].blocks[X][(Y)%MC_SUBCHUNK_H][Z]

	for(int y = (MC_SUBCHUNK_H * MC_CHUNK_H)-1; y >= 0; y--){
		for(int z = 0; z < MC_SUBCHUNK_D; z++){
            for(int x = 0; x < MC_SUBCHUNK_W; x++){
                MCBLOCK(x,y,z).block_light = (y==(MC_SUBCHUNK_H * MC_CHUNK_H)-1)?(15<<4):0;
				if(y+1 < (MC_SUBCHUNK_H * MC_CHUNK_H)){
					int Lum1 = MCBLOCK(x,y+1,z).block_light >> 4;
					if(MC_BLOCKS[MCBLOCK(x,y+1,z).block_id].solid){
						Lum1 = 0;
					}
					MCBLOCK(x,y,z).block_light = Lum1 << 4;
				}
            }
        }
		for(int z = 0; z < MC_SUBCHUNK_D; z++){
            for(int x = 0; x < MC_SUBCHUNK_W; x++){
				int Lum1 = 15;
				if(y+1 < (MC_SUBCHUNK_H * MC_CHUNK_H)){
					Lum1 = MCBLOCK(x,y+1,z).block_light >> 4;
					if(MC_BLOCKS[MCBLOCK(x,y+1,z).block_id].solid){
						Lum1 = 0;
						if(x-1 >= 0 )
							if(!MC_BLOCKS[MCBLOCK(x-1,y,z).block_id].solid){
								Lum1 += std::max(MCBLOCK(x-1,y,z).block_light >> 4,0);
							}
						if(x+1 < MC_SUBCHUNK_W )
							if(!MC_BLOCKS[MCBLOCK(x+1,y,z).block_id].solid){
								Lum1 += std::max(MCBLOCK(x+1,y,z).block_light >> 4,0);
							}
						if(z-1 >= 0 )
							if(!MC_BLOCKS[MCBLOCK(x,y,z-1).block_id].solid){
								Lum1 += std::max(MCBLOCK(x,y,z-1).block_light >> 4,0);
							}
						if(z+1 < MC_SUBCHUNK_D )
							if(!MC_BLOCKS[MCBLOCK(x,y,z+1).block_id].solid){
								Lum1 += std::max(MCBLOCK(x,y,z+1).block_light >> 4,0);
							}
						Lum1 >>= 2; // Divide by 4
						--Lum1; // Subtract 1
					}
				}
				if(Lum1 > 15) Lum1 = 15;
				if(Lum1<0) Lum1 = 0;
            	MCBLOCK(x,y,z).block_light = Lum1 << 4;
            }
        }
    }

/*

//	block.lightemit
	for(int x = 0; x < MC_SUBCHUNK_W; x++){
		for(int z = 0; z < MC_SUBCHUNK_D; z++){
			lightBrightness = 15;
			for(int y = (MC_SUBCHUNK_H * MC_CHUNK_H)-1; y >= 0; y--){
				#define MCBLOCK(X,Y,Z) chunk.sub_chunk[(Y / MC_SUBCHUNK_H)].blocks[X][Y%MC_SUBCHUNK_H][Z]
				if(MC_BLOCKS[MCBLOCK(x,y,z).block_id].solid){
					if(lightBrightness){
						int lightN = MCBLOCK(x,y,z-1).block_light&0xF;
						int lightS = MCBLOCK(x,y,z+1).block_light&0xF;
						int lightE = MCBLOCK(x-1,y,z).block_light&0xF;
						int lightW = MCBLOCK(x+1,y,z).block_light&0xF;
						int lightU = MCBLOCK(x,(y-1),z).block_light&0xF;
						int lightV = lightN + lightS + lightE + lightW + lightU;
						lightV /= 5; // Avrage the value
						blockBrightness = 15;
						blockBrightness += lightV - lightBrightness;
						chunk.sub_chunk[(y / MC_SUBCHUNK_H)].blocks[x][y%MC_SUBCHUNK_H][z].block_light = (lightBrightness << 4);
						chunk.sub_chunk[(y / MC_SUBCHUNK_H)].blocks[x][y%MC_SUBCHUNK_H][z].block_light += blockBrightness&0xF;
						lightBrightness -= 1;
					}
				}
			}
		}
	}*/
};


// Mesh Stuff

LL3D_Triangle MC_CubeMesh[12] = {
// BOTTOM -Y
{ 1.0f, 0.0f, 1.0f, 1.0f,    0.0f, 0.0f, 1.0f, 1.0f,    0.0f, 0.0f, 0.0f, 1.0f,		0.0f, 1.0f, 1.0f,		0.0f, 0.0f, 1.0f,		1.0f, 0.0f, 1.0f,},
{ 1.0f, 0.0f, 1.0f, 1.0f,    0.0f, 0.0f, 0.0f, 1.0f,    1.0f, 0.0f, 0.0f, 1.0f,		0.0f, 1.0f, 1.0f,		1.0f, 0.0f, 1.0f,		1.0f, 1.0f, 1.0f,},

// TOP +Y
{ 0.0f, 1.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 1.0f, 1.0f,		0.0f, 1.0f, 1.0f,		0.0f, 0.0f, 1.0f,		1.0f, 0.0f, 1.0f,},
{ 0.0f, 1.0f, 0.0f, 1.0f,    1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 0.0f, 1.0f,		0.0f, 1.0f, 1.0f,		1.0f, 0.0f, 1.0f,		1.0f, 1.0f, 1.0f,},

// SOUTH -Z
{ 0.0f, 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f, 1.0f,    1.0f, 1.0f, 0.0f, 1.0f,		0.0f, 1.0f, 1.0f,		0.0f, 0.0f, 1.0f,		1.0f, 0.0f, 1.0f,}, 
{ 0.0f, 0.0f, 0.0f, 1.0f,    1.0f, 1.0f, 0.0f, 1.0f,    1.0f, 0.0f, 0.0f, 1.0f,		0.0f, 1.0f, 1.0f,		1.0f, 0.0f, 1.0f,		1.0f, 1.0f, 1.0f,},

// NORTH +Z        																			   
{ 1.0f, 0.0f, 1.0f, 1.0f,    1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 1.0f, 1.0f, 1.0f,		0.0f, 1.0f, 1.0f,		0.0f, 0.0f, 1.0f,		1.0f, 0.0f, 1.0f,},
{ 1.0f, 0.0f, 1.0f, 1.0f,    0.0f, 1.0f, 1.0f, 1.0f,    0.0f, 0.0f, 1.0f, 1.0f,		0.0f, 1.0f, 1.0f,		1.0f, 0.0f, 1.0f,		1.0f, 1.0f, 1.0f,},

// WEST -X           																			   
{ 0.0f, 0.0f, 1.0f, 1.0f,    0.0f, 1.0f, 1.0f, 1.0f,    0.0f, 1.0f, 0.0f, 1.0f,		0.0f, 1.0f, 1.0f,		0.0f, 0.0f, 1.0f,		1.0f, 0.0f, 1.0f,},
{ 0.0f, 0.0f, 1.0f, 1.0f,    0.0f, 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f, 1.0f,		0.0f, 1.0f, 1.0f,		1.0f, 0.0f, 1.0f,		1.0f, 1.0f, 1.0f,},

// EAST  +X         																			   
{ 1.0f, 0.0f, 0.0f, 1.0f,    1.0f, 1.0f, 0.0f, 1.0f,    1.0f, 1.0f, 1.0f, 1.0f,		0.0f, 1.0f, 1.0f,		0.0f, 0.0f, 1.0f,		1.0f, 0.0f, 1.0f,},
{ 1.0f, 0.0f, 0.0f, 1.0f,    1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 0.0f, 1.0f, 1.0f,		0.0f, 1.0f, 1.0f,		1.0f, 0.0f, 1.0f,		1.0f, 1.0f, 1.0f,},

};

BlockState MC_AIRBLOCK_STATE;

unsigned short MC_GetChunkBlock(Chunk &chunk,int chunkY, int x,int y,int z) {
	if(y > MC_SUBCHUNK_H) y -= MC_SUBCHUNK_H;
	if(x>=0&&y>=0&&z>=0&&x<MC_SUBCHUNK_W&&y<MC_SUBCHUNK_H&&z<MC_SUBCHUNK_D)
		return chunk.sub_chunk[chunkY].blocks[x][y][z].block_id;
	return 0; // Just air??
};

BlockState MC_GetChunkBlockState(Chunk &chunk,int chunkY, int x,int y,int z) {
	if(y > MC_SUBCHUNK_H) y -= MC_SUBCHUNK_H;
	if(x>=0&&y>=0&&z>=0&&x<MC_SUBCHUNK_W&&y<MC_SUBCHUNK_H&&z<MC_SUBCHUNK_D)
		return chunk.sub_chunk[chunkY].blocks[x][y][z];
	return MC_AIRBLOCK_STATE; // Just air??
};

void MC_SetChunkBlock(Chunk &chunk,unsigned short block, int chunkY, int x,int y,int z) {
	if(y > MC_SUBCHUNK_H) y -= MC_SUBCHUNK_H;
	if(x>=0&&y>=0&&z>=0&&x<MC_SUBCHUNK_W&&y<MC_SUBCHUNK_H&&z<MC_SUBCHUNK_D)
		chunk.sub_chunk[chunkY].blocks[x][y][z].block_id = block;
};

unsigned short MC_GetWorldBlock(int x,int y,int z) {
	// Get the chunk location in the world
	int cx = (x / MC_SUBCHUNK_W);
	int cy = (y / MC_SUBCHUNK_H);
	int cz = (z / MC_SUBCHUNK_D);
	int chunkid = MC_GetMinecraftChunk(cx, cz);
	if(chunkid==-1) return 0;
	return MC_GetChunkBlock(MC_MinecraftWorld[chunkid],cy,x - (cx*MC_SUBCHUNK_W),y - (cy*MC_SUBCHUNK_H),z - (cz*MC_SUBCHUNK_D));
};

void MC_SetWorldBlock(int x,int y,int z,std::string block_name){
	// Get the chunk location in the world
	int cx = (x / MC_SUBCHUNK_W);
	int cy = (y / MC_SUBCHUNK_H);
	int cz = (z / MC_SUBCHUNK_D);
	int chunkid = MC_GetMinecraftChunk(cx, cz);
	if(chunkid==-1) return;
	MC_SetChunkBlock(MC_MinecraftWorld[chunkid],MC_FindBlockID(block_name),cy,x - (cx*MC_SUBCHUNK_W),y - (cy*MC_SUBCHUNK_H),z - (cz*MC_SUBCHUNK_D));
};

void MC_SetWorldBlock(int x,int y,int z,unsigned short block_id){
	// Get the chunk location in the world
	int cx = (x / MC_SUBCHUNK_W);
	int cy = (y / MC_SUBCHUNK_H);
	int cz = (z / MC_SUBCHUNK_D);
	int chunkid = MC_GetMinecraftChunk(cx, cz);
	if(chunkid==-1) return;
	MC_SetChunkBlock(MC_MinecraftWorld[chunkid],block_id,cy,x - (cx*MC_SUBCHUNK_W),y - (cy*MC_SUBCHUNK_H),z - (cz*MC_SUBCHUNK_D));
};


bool MC_SpawnBlockMesh(int cx, int cy, int cz, int x, int y, int z){
	// Get the chunk location in the vector
	int chunkid = MC_GetMinecraftChunk(cx,cz);
	if(x<0||y<0||z<0||x>=MC_SUBCHUNK_W||y>=MC_SUBCHUNK_H||z>=MC_SUBCHUNK_D)
		return false;
	if(chunkid==-1) return false;

	int checkChunk = -1;
	unsigned short checkBlocks[4] = {0,0,0,0};
	// We have to check the chunk next to it
	if(z==0) checkChunk = MC_GetMinecraftChunk(cx,cz-1);
	if(z==(MC_SUBCHUNK_D-1)) checkChunk = MC_GetMinecraftChunk(cx,cz+1);

	if(x==0){
		checkChunk = MC_GetMinecraftChunk(cx-1,cz);
		if(checkChunk!=-1)
			checkBlocks[0] = MC_GetChunkBlock(MC_MinecraftWorld[checkChunk],cy,(MC_SUBCHUNK_W-1),y,z);
	}

	if(x==(MC_SUBCHUNK_W-1)){
		checkChunk = MC_GetMinecraftChunk(cx+1,cz);
		if(checkChunk!=-1)
			checkBlocks[1] = MC_GetChunkBlock(MC_MinecraftWorld[checkChunk],cy,0,y,z);
	}

	if(z==0){
		checkChunk = MC_GetMinecraftChunk(cx,cz-1);
		if(checkChunk!=-1)
			checkBlocks[2] = MC_GetChunkBlock(MC_MinecraftWorld[checkChunk],cy,x,y,(MC_SUBCHUNK_D-1));
	}

	if(z==(MC_SUBCHUNK_D-1)){
		checkChunk = MC_GetMinecraftChunk(cx,cz+1);
		if(checkChunk!=-1)
			checkBlocks[3] = MC_GetChunkBlock(MC_MinecraftWorld[checkChunk],cy,x,y,0);
	}

	// get the block
	BlockState blockS = MC_GetChunkBlockState(MC_MinecraftWorld[chunkid],cy,x,y,z);
	unsigned short blockid = blockS.block_id;

	if(blockid==0) return true; // It was an air block

	// Generate the cube
	LL3D_Triangle triangle;

	int blockTex,TexY,TexX;

	#define genTex(BV) \
	blockTex = MC_BlockTexturesOffsets[blockid][(BV/2)];\
	TexY = (blockTex / 16);\
	TexX = (blockTex - (TexY*16));\
	if(BV&0x1){\
		triangle.t[0].Set((float)TexX * MC_TextureAtlasScale, ((float)TexY * MC_TextureAtlasScale)+MC_TextureAtlasScale);\
		triangle.t[1].Set(((float)TexX * MC_TextureAtlasScale)+MC_TextureAtlasScale, (float)TexY * MC_TextureAtlasScale);\
		triangle.t[2].Set(((float)TexX * MC_TextureAtlasScale)+MC_TextureAtlasScale, ((float)TexY * MC_TextureAtlasScale)+MC_TextureAtlasScale);\
	}else{\
		triangle.t[0].Set((float)TexX * MC_TextureAtlasScale, ((float)TexY * MC_TextureAtlasScale)+MC_TextureAtlasScale);\
		triangle.t[1].Set((float)TexX * MC_TextureAtlasScale, (float)TexY * MC_TextureAtlasScale);\
		triangle.t[2].Set(((float)TexX * MC_TextureAtlasScale)+MC_TextureAtlasScale, (float)TexY * MC_TextureAtlasScale);\
	}

	float CX = cx*MC_SUBCHUNK_W, CY = cy * MC_SUBCHUNK_H, CZ = cz*MC_SUBCHUNK_D;
	float lightDiff = (float)(blockS.block_light&0x0F) / 15.0f;
	float blightDiff = (float)((blockS.block_light>>4)&0x0F) / 15.0f;
	lightDiff = std::max(lightDiff,blightDiff);
	lightDiff = std::max(lightDiff,0.2f + ((float)(MC_BrightnessValue*3)/10.f));
	unsigned char lum100 = std::min((float)255 * (float)lightDiff,255.0f);
	unsigned char lum80 = std::min((float)191 * (float)lightDiff,255.0f);
	unsigned char lum50 = std::min((float)127 * (float)lightDiff,255.0f);

	{
		unsigned short testblock = MC_GetChunkBlock(MC_MinecraftWorld[chunkid],cy,x,y-1,z);
		if(testblock != blockid && (testblock==0 || MC_BLOCKS[testblock].block_culls == false)){
			triangle = MC_CubeMesh[0]; // Bottom
			triangle.translate(x+CX,y+CY,z+CZ);

			genTex(0)

			triangle.attr.culls = true;
			triangle.attr.affectedByLight = true;
			triangle.attr.usesZBuffer = true;
			triangle.attr.tri_color = MC_BlockColors[blockid];
			triangle.attr.light_lum = lum50; // 50% lum
			triangle.tex = &MC_BlocksAtlas;

			MC_WorldMesh.tris.push_back(triangle);
			
			triangle = MC_CubeMesh[1]; // Bottom
			triangle.translate(x+CX,y+CY,z+CZ);

			genTex(1)

			triangle.attr.culls = true;
			triangle.attr.affectedByLight = true;
			triangle.attr.usesZBuffer = true;
			triangle.attr.tri_color = MC_BlockColors[blockid];
			triangle.attr.light_lum = lum50; // 50% lum
			triangle.tex = &MC_BlocksAtlas;
			MC_WorldMesh.tris.push_back(triangle);
		}
	}
	{
		unsigned short testblock = MC_GetChunkBlock(MC_MinecraftWorld[chunkid],cy,x,y+1,z);
		if(testblock != blockid && (testblock==0 || MC_BLOCKS[testblock].block_culls == false)){

			triangle = MC_CubeMesh[2]; // Top
			triangle.translate(x+CX,y+CY,z+CZ);

			genTex(2)

			triangle.attr.culls = true;
			triangle.attr.affectedByLight = true;
			triangle.attr.usesZBuffer = true;
			triangle.attr.tri_color = MC_BlockColors[blockid];
			triangle.attr.light_lum = lum100; // 100% lum
			triangle.tex = &MC_BlocksAtlas;
			MC_WorldMesh.tris.push_back(triangle);

			triangle = MC_CubeMesh[3]; // Top
			triangle.translate(x+CX,y+CY,z+CZ);

			genTex(3)

			triangle.attr.culls = true;
			triangle.attr.affectedByLight = true;
			triangle.attr.usesZBuffer = true;
			triangle.attr.tri_color = MC_BlockColors[blockid];
			triangle.attr.light_lum = lum100; // 100% lum
			triangle.tex = &MC_BlocksAtlas;
			MC_WorldMesh.tris.push_back(triangle);
		}
	}
	if(!(checkBlocks[2]&&MC_BLOCKS[blockid].block_culls)){
		unsigned short testblock = MC_GetChunkBlock(MC_MinecraftWorld[chunkid],cy,x,y,z-1);
		if(testblock != blockid && (testblock==0 || MC_BLOCKS[testblock].block_culls == false)){
			triangle = MC_CubeMesh[4]; // South
			triangle.translate(x+CX,y+CY,z+CZ);

			genTex(4)

			triangle.attr.culls = true;
			triangle.attr.affectedByLight = true;
			triangle.attr.usesZBuffer = true;
			triangle.attr.tri_color = MC_BlockColors[blockid];
			triangle.attr.light_lum = lum80; 
			triangle.tex = &MC_BlocksAtlas;
			MC_WorldMesh.tris.push_back(triangle);

			triangle = MC_CubeMesh[5]; // South
			triangle.translate(x+CX,y+CY,z+CZ);

			genTex(5)

			triangle.attr.culls = true;
			triangle.attr.affectedByLight = true;
			triangle.attr.usesZBuffer = true;
			triangle.attr.tri_color = MC_BlockColors[blockid];
			triangle.attr.light_lum = lum80; 
			triangle.tex = &MC_BlocksAtlas;
			MC_WorldMesh.tris.push_back(triangle);
		}
	}
	if(!(checkBlocks[3]&&MC_BLOCKS[blockid].block_culls)){
		// Add the side
		unsigned short testblock = MC_GetChunkBlock(MC_MinecraftWorld[chunkid],cy,x,y,z+1);
		if(testblock != blockid && (testblock==0 || MC_BLOCKS[testblock].block_culls == false)){
			triangle = MC_CubeMesh[6]; // North
			triangle.translate(x+CX,y+CY,z+CZ);

			genTex(6)

			triangle.attr.culls = true;
			triangle.attr.affectedByLight = true;
			triangle.attr.usesZBuffer = true;
			triangle.attr.tri_color = MC_BlockColors[blockid];
			triangle.attr.light_lum = lum80; 
			triangle.tex = &MC_BlocksAtlas;
			MC_WorldMesh.tris.push_back(triangle);

			triangle = MC_CubeMesh[7]; // North
			triangle.translate(x+CX,y+CY,z+CZ);

			genTex(7)

			triangle.attr.culls = true;
			triangle.attr.affectedByLight = true;
			triangle.attr.usesZBuffer = true;
			triangle.attr.tri_color = MC_BlockColors[blockid];
			triangle.attr.light_lum = lum80; 
			triangle.tex = &MC_BlocksAtlas;
			MC_WorldMesh.tris.push_back(triangle);
		}
	}
	if(!(checkBlocks[0]&&MC_BLOCKS[blockid].block_culls)){
		// Add the side
		unsigned short testblock = MC_GetChunkBlock(MC_MinecraftWorld[chunkid],cy,x-1,y,z);
		if(testblock != blockid && (testblock==0 || MC_BLOCKS[testblock].block_culls == false)){
			triangle = MC_CubeMesh[8]; // West
			triangle.translate(x+CX,y+CY,z+CZ);

			genTex(8)

			triangle.attr.culls = true;
			triangle.attr.affectedByLight = true;
			triangle.attr.usesZBuffer = true;
			triangle.attr.tri_color = MC_BlockColors[blockid];
			triangle.attr.light_lum = lum80;
			triangle.tex = &MC_BlocksAtlas;
			MC_WorldMesh.tris.push_back(triangle);

			triangle = MC_CubeMesh[9]; // West
			triangle.translate(x+CX,y+CY,z+CZ);

			genTex(9)

			triangle.attr.culls = true;
			triangle.attr.affectedByLight = true;
			triangle.attr.usesZBuffer = true;
			triangle.attr.tri_color = MC_BlockColors[blockid];
			triangle.attr.light_lum = lum80;
			triangle.tex = &MC_BlocksAtlas;
			MC_WorldMesh.tris.push_back(triangle);
		}
	}
	if(!(checkBlocks[1]&&MC_BLOCKS[blockid].block_culls)){
		// Add the side
		unsigned short testblock = MC_GetChunkBlock(MC_MinecraftWorld[chunkid],cy,x+1,y,z);
		if(testblock != blockid && (testblock==0 || MC_BLOCKS[testblock].block_culls == false)){
			triangle = MC_CubeMesh[10]; // East
			triangle.translate(x+CX,y+CY,z+CZ);

			genTex(10)

			triangle.attr.culls = true;
			triangle.attr.affectedByLight = true;
			triangle.attr.usesZBuffer = true;
			triangle.attr.tri_color = MC_BlockColors[blockid];
			triangle.attr.light_lum = lum80;
			triangle.tex = &MC_BlocksAtlas;
			MC_WorldMesh.tris.push_back(triangle);

			triangle = MC_CubeMesh[11]; // East
			triangle.translate(x+CX,y+CY,z+CZ);

			genTex(11)

			triangle.attr.culls = true;
			triangle.attr.affectedByLight = true;
			triangle.attr.usesZBuffer = true;
			triangle.attr.tri_color = MC_BlockColors[blockid];
			triangle.attr.light_lum = lum80; 
			triangle.tex = &MC_BlocksAtlas;
			MC_WorldMesh.tris.push_back(triangle);
		}
	}
	return true;
};

void MC_GenerateChunkMesh(int cx, int cz){
	for(int subcY = 0; subcY < MC_CHUNK_H; subcY++){
		for(int y = 0; y < MC_SUBCHUNK_H; y++){
			for(int x = 0; x < MC_SUBCHUNK_W; x++){
				for(int z = 0; z < MC_SUBCHUNK_D; z++){
					MC_SpawnBlockMesh(cx, subcY, cz, x, y,z);
				}
			}
		}
	}
	// Add the new mesh to the scene
	//LL3D_AddMesh(MC_WorldMesh);
};

void MC_ClearWorldMesh(void){
	MC_WorldMesh.tris.clear();
};
