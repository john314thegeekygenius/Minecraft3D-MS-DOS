
#ifndef __MINECRAFT_WORLD__
#define __MINECRAFT_WORLD__

const int MC_SUBCHUNK_H = 8;
const int MC_SUBCHUNK_W = 8;
const int MC_SUBCHUNK_D = 8;
const int MC_CHUNK_H = 8;

typedef struct BlockState {
	// No way to make this smaller unless we limit the number of blocks to 255
	unsigned short block_id;

	/*	
	+--------------- Waterloged bit or allows age to be up to 31
	| +------------- Block Age
	| |       +----- Block faceing direction (n, s, e, w, u, d)
	w a a a a f f f 
	*/
	unsigned char block_info;

	// First 4 bits are sky light
	// Last 4 bits are block light
	unsigned char block_light; 

	inline void setup(unsigned short blkid,bool wdl, unsigned char a, char f, unsigned char sk_l, unsigned char bk_l){
		if(blkid==-1)
			blkid = 0;
		block_id = blkid;

		// Clear the block info
		block_info = 0x00;

		// Add the faceing direction
		block_info = f&0x7;

		// Add the age
		if(a > 31) a = 31; // Max
		block_info += (a<<3); // Shift it 3 bits left

		// Flip the bit if waterloged
		if(wdl)
			block_info |= 0x80;

		// Set the light levels
		block_light = ((sk_l&0xF)<<4) + (bk_l&0xF);
	};
}BlockState;

typedef struct SubChunk {
	BlockState blocks[MC_SUBCHUNK_W][MC_SUBCHUNK_H][MC_SUBCHUNK_D];
}SubChunk;

typedef struct Chunk {
	std::vector<SubChunk> sub_chunk; // sub_chunks verticaly stacked
	short x;
	short z;
}Chunk;

void MC_UpdateTextureScale(void);

void MC_SetWorldSeed(long seed);
void MC_ClearWorldChunks(void);
void MC_GenerateChunk(Chunk &chunk);
void MC_GenerateFlatChunk(Chunk &chunk,std::string chunkblocks);
void MC_LightChunk(int cx,int cz);
void MC_GenerateWorldChunk(int cx,int cz);

// Mesh stuff

int MC_GetMinecraftChunk(int cx,int cz);
void MC_GenerateChunkMesh(int cx, int cy);
unsigned short MC_GetWorldBlock(int x,int y,int z);
void MC_SetWorldBlock(int x,int y,int z,std::string block_name);
void MC_SetWorldBlock(int x,int y,int z,unsigned short block_id);
void MC_ClearWorldMesh(void);

#endif

