#ifndef __MC_BLOCKS__
#define __MC_BLOCKS__

// List of all the minecraft blocks and their ids

typedef struct MC_BLOCK_TYPE {
	char block_name[32];
	unsigned short id;
	bool block_culls;
	bool solid;
	unsigned char lightemit;
}MC_BLOCK_TYPE;

#define MC_NUM_OF_BLOCKS 64

const MC_BLOCK_TYPE MC_BLOCKS[MC_NUM_OF_BLOCKS] = {
	{"air",0,false,false,0},
	{"stone",1,true,true,0},
	{"grass_block",2,true,true,0}, 
	{"dirt",3,true,true,0},
	{"cobblestone",4,true,true,0},
	{"oak_planks",5,true,true,0},
	{"oak_sapling",6,false,false,0},
	{"bedrock",7,true,true,0},
	{"water",8,false,true,0},
	{"water_flowing",9,false,true,0},
	{"lava",10,false,true,15},
	{"lava_flowing",11,false,true,15},
	{"sand",12,true,true,0},
	{"gravel",13,true,true,0},
	{"gold_ore",14,true,true,0},
	{"iron_ore",15,true,true,0},
	{"coal_ore",16,true,true,0},
	{"oak_log",17,true,true,0},
	{"oak_leaves",18,false,false,0},
	{"sponge",19,true,true,0},
	{"glass",20,false,false,0},
	{"wool_#1",21,true,true,0},
	{"wool_#2",22,true,true,0},
	{"wool_#3",23,true,true,0},
	{"wool_#4",24,true,true,0},
	{"wool_#5",25,true,true,0},
	{"wool_#6",26,true,true,0},
	{"wool_#7",27,true,true,0},
	{"wool_#8",28,true,true,0},
	{"wool_#9",29,true,true,0},
	{"wool_#10",30,true,true,0},
	{"wool_#11",31,true,true,0},
	{"wool_#12",32,true,true,0},
	{"wool_#13",33,true,true,0},
	{"wool_#14",34,true,true,0},
	{"wool_#15",35,true,true,0},
	{"wool_#16",36,true,true,0},
	{"dandilion",37,false,false,0},
	{"red_rose",38,false,false,0},
	{"brown_mushroom",39,false,false,0},
	{"red_mushroom",40,false,false,0},
	{"gold_block",41,true,true,0},
	{"iron_block",42,true,true,0},
	{"double_stone_slabs",43,true,true,0},
	{"stone_slab",44,false,false,0},
	{"bricks",45,true,true,0},
	{"tnt",46,true,true,0},
	{"bookshelf",47,true,true,0},
	{"mossy_cobblestone",48,true,true,0},
	{"obsidian",49,true,true,0},
	{"torch",50,false,false,14},
	{"fire",51,false,false,14},
	{"water_2",52,false,false,0},
	{"lava_2",53,false,false,15},
	{"chest",54,false,false,0},
	{"gear",55,false,false,0},
	{"diamond_ore",56,true,true,0},
	{"diamond_block",57,true,true,0},
	{"crafting_table",58,true,true,0},
	{"wheat",59,false,false,0},
	{"soil",60,false,true,0},
	{"furnace_off",61,true,true,0},
	{"furnace_on",62,true,true,14},

};

inline unsigned short MC_FindBlockID(std::string name){
	for(int i = 0; i < MC_NUM_OF_BLOCKS; i++){
		if(strcmp(name.c_str(),MC_BLOCKS[i].block_name)==0)
			return MC_BLOCKS[i].id;
	}
	return 0; // air! :D
};


// Texture Coords
// 0----1
// |   /|
// | /  |
// 2----3
const int MC_BlockTexturesOffsets[MC_NUM_OF_BLOCKS][6] = {
	// Air (No texture)
	{0,0,0,0,0,0}, // b,t,s,n,w,e
	// Stone
	{1,1,1,1,1,1}, 
	// Grass Block
	{2,0,3,3,3,3}, 
	// Dirt
	{2,2,2,2,2,2}, 
	// Cobblestone
	{16,16,16,16,16,16}, 
	// Oak Planks
	{4,4,4,4,4,4}, 
	// Sapling
	{15,15,15,15,15,15}, 
	// Bedrock
	{17,17,17,17,17,17}, 
	// Water
	{205,205,205,205,205,205}, 
	// Water Flowing
	{222,222,222,222,222,222}, 
	// Lava
	{237,237,237,237,237,237}, 
	// Lava Flowing
	{254,254,254,254,254,254}, 
	// Sand
	{18,18,18,18,18,18}, 
	// Gravel
	{19,19,19,19,19,19}, 
	// Gold Ore
	{32,32,32,32,32,32}, 
	// Iron Ore
	{33,33,33,33,33,33}, 
	// Coal Ore
	{34,34,34,34,34,34}, 
	// Oak Log
	{21,21,20,20,20,20}, 
	// Oak Leaves
	{52,52,52,52,52,52}, 
	// Fast Oak Leaves
//	{53,53,53,53,53,53}, 
	// Sponge
	{48,48,48,48,48,48}, 
	// Glass
	{49,49,49,49,49,49}, 
	// Wool 1
	{64,64,64,64,64,64},
	// Wool 2
	{65,65,65,65,65,65},
	// Wool 3
	{66,66,66,66,66,66},
	// Wool 4
	{67,67,67,67,67,67},
	// Wool 5
	{68,68,68,68,68,68},
	// Wool 6
	{69,69,69,69,69,69},
	// Wool 7
	{70,70,70,70,70,70},
	// Wool 8
	{71,71,71,71,71,71},
	// Wool 9
	{72,72,72,72,72,72},
	// Wool 10
	{73,73,73,73,73,73},
	// Wool 11
	{74,74,74,74,74,74},
	// Wool 12
	{75,75,75,75,75,75},
	// Wool 13
	{76,76,76,76,76,76},
	// Wool 14
	{77,77,77,77,77,77},
	// Wool 15
	{78,78,78,78,78,78},
	// Wool 16
	{79,79,79,79,79,79},
	// Dandilion
	{13,13,13,13,13,13},
	// Red Rose
	{12,12,12,12,12,12},
	// Brown Mushroom
	{29,29,29,29,29,29},
	// Red Mushroom
	{28,28,28,28,28,28},
	// Gold Block
	{55,23,39,39,39,39},
	// Iron Block
	{54,22,38,38,38,38},
	// Double Stone Slabs
	{6,6,5,5,5,5},
	// Stone Slab
	{6,6,5,5,5,5},
	// Bricks
	{7,7,7,7,7,7},
	// TNT
	{10,9,8,8,8,8},
	// Bookshelf
	{4,4,35,35,35,35},
	// Mossy Cobblestone
	{36,36,36,36,36,36},
	// Obsidian
	{37,37,37,37,37,37},
	// Torch
	{80,80,80,80,80,80},
	// Fire
};

const MK2D_RGB MC_BlockColors[MC_NUM_OF_BLOCKS] = {
	{0,0,0}, // air
	{128,128,128}, // stone
	{0,128,0}, // grass block
	{160,80,0}, // dirt
	{180,180,180}, // cobblestone
	{160,120,0}, // oak planks
	{0,128,0}, // oak sapling
	{40,40,40}, // bedrock
	{0,0,128}, // water
	{0,0,128}, // water flowing
	{255,40,0}, // lava
	{255,40,0}, // lava flowing
	{200,200,0}, // sand
	{200,200,200}, // gravel
	{200,200,100}, // gold ore
	{200,100,100}, // iron ore
	{100,100,100}, // coal ore
	{200,100,10}, // oak log
	{20,200,20}, // oak leaves
	{230,230,0}, // sponge
	{0,0,0}, // glass
	
};

// MC_BlockTexturesOffsets[block][xy] *= 0.0625f;

#endif

