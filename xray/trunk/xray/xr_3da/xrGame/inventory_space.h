#pragma once
#include "../../build_config_defines.h"

#define CMD_START	(1<<0)
#define CMD_STOP	(1<<1)

#define NO_ACTIVE_SLOT		0xffffffff
#define KNIFE_SLOT			0
#define PISTOL_SLOT			1
#define RIFLE_SLOT			2
#define GRENADE_SLOT		3
#define APPARATUS_SLOT		4
#define BOLT_SLOT			5
#define OUTFIT_SLOT			6
#define PDA_SLOT			7
#define DETECTOR_SLOT		8
#define TORCH_SLOT			9
#define ARTEFACT_SLOT		10

#ifdef INV_NEW_SLOTS_SYSTEM
	#define HELMET_SLOT			11
	#define SLOT_QUICK_ACCESS_0 12
	#define SLOT_QUICK_ACCESS_1 13
	#define SLOT_QUICK_ACCESS_2 14
	#define SLOT_QUICK_ACCESS_3 15
	#define SLOTS_TOTAL			16
    // alpet: ограничение по вхождению предмета
	#define SLOT_QUICK_CELLS_X	1
	#define SLOT_QUICK_CELLS_Y	1
#else
	#define SLOTS_TOTAL			11	
#endif

#define RUCK_HEIGHT			280
#define RUCK_WIDTH			7

class CInventoryItem;
class CInventory;

typedef CInventoryItem*				PIItem;
typedef xr_vector<PIItem>			TIItemContainer;


enum EItemPlace
{			
	eItemPlaceUndefined,
	eItemPlaceSlot,
	eItemPlaceBelt,
	eItemPlaceRuck
};

extern u32	INV_STATE_LADDER;
extern u32	INV_STATE_CAR;
extern u32	INV_STATE_BLOCK_ALL;
extern u32	INV_STATE_INV_WND;
extern u32	INV_STATE_BUY_MENU;
