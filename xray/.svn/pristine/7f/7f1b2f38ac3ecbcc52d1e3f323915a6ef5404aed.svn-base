#pragma once

namespace GameObject {
	enum ECallbackType {
		eTradeStart = u32(0),
		eTradeStop,
		eTradeSellBuyItem,
		eTradePerformTradeOperation,

		eZoneEnter,
		eZoneExit,
		eExitLevelBorder,
		eEnterLevelBorder,
		eDeath,

		ePatrolPathInPoint,

		eInventoryPda,
		eInventoryInfo,
		eArticleInfo,
		eTaskStateChange,
		eMapLocationAdded,

		eUseObject,

		eHit,

		eSound,

		eActionTypeMovement,
		eActionTypeWatch,
		eActionTypeAnimation,
		eActionTypeSound,
		eActionTypeParticle,
		eActionTypeObject,

		eActorSleep,

		eHelicopterOnPoint,
		eHelicopterOnHit,

		eOnItemTake,
		eOnItemDrop,

		eScriptAnimation,

		eTraderGlobalAnimationRequest,
		eTraderHeadAnimationRequest,
		eTraderSoundEnd,

		eInvBoxItemTake,
		/************************************************** added by Ray Twitty (aka Shadows) START **************************************************/
		// клавиатура/мышь
		eOnKeyPress,
		eOnKeyRelease,
		eOnKeyHold,
		eOnMouseMove,
		eOnMouseWheel,
		// инвентарь
		eOnItemBelt,
		eOnItemSlot,
		eOnItemRuck,
		// фонарь
		eSwitchTorch,
		/*************************************************** added by Ray Twitty (aka Shadows) END ***************************************************/

		// Added by Cribbledirge Start.
		/* Added in some mouse press detection for the player character. */
		eOnMousePress,
		eOnMouseRelease,
		eOnMouseHold,

		// These specifically say actor as I intend to add callbacks for NPCs firing their weapons.
		eOnActorWeaponFire,
		eOnActorWeaponJammed,
		eOnActorWeaponEmpty,
		eOnActorWeaponReload,

		// NPC Weapon Callbacks.
		eOnNPCWeaponFire,
		eOnNPCWeaponJammed,
		eOnNPCWeaponEmpty,
		eOnNPCWeaponReload,

		// Called when the player zooms their weapon in or out.
		eOnActorWeaponZoomIn,
		eOnActorWeaponZoomOut,

		eCellItemFocus,
		eCellItemFocusLost,

		eAttachVehicle,
		eDetachVehicle,
		eUseVehicle,
		eBeforeSave,
		ePostSave,
		eOnInvBoxItemTake,
		eOnInvBoxItemDrop,
		eOnInvBoxOpen,
		eUIMapClick,
		eUIMapSpotClick,
		eUIPickUpItemShowing,
		eUIGroupItems,
		eOnCellItemMouse,
		eOnActorStartShoot,
		eOnWpnShellDrop,
		eOnThrowGrenade,

		// Cribbledirge End
		eDummy = u32(-1),
	};
};

enum EBoneCallbackType{
	bctDummy = u32(0),	// 0 - required!!!
	bctPhysics,
	bctCustom,
	bctForceU32 = u32(-1),
};