#include "stdafx.h"
#include "UIInventoryWnd.h"
#include "../actor.h"
#include "../silencer.h"
#include "../scope.h"
#include "../grenadelauncher.h"
#include "../Artifact.h"
#include "../eatable_item.h"
#include "../BottleItem.h"
#include "../WeaponMagazined.h"
#include "../inventory.h"
#include "../game_base.h"
#include "../game_cl_base.h"
#include "../xr_level_controller.h"
#include "UICellItem.h"
#include "UIListBoxItem.h"
#include "../CustomOutfit.h"


void CUIInventoryWnd::EatItem(PIItem itm)
{
	SetCurrentItem							(NULL);
	if(!itm->Useful())						return;

	SendEvent_Item_Eat						(itm);

	PlaySnd									(eInvItemUse);
}

#if defined(INV_NEW_SLOTS_SYSTEM)
// вернет true если слот назначения быстрый, и не занят аналогичным предметом
bool is_quick_slot(u32 slot, PIItem item, CInventory *inv)
{
	if (slot >= SLOT_QUICK_ACCESS_0 && slot <= SLOT_QUICK_ACCESS_3)
	{
		const shared_str &sect = item->object().cNameSect();
		for (u32 s = SLOT_QUICK_ACCESS_0; s <= SLOT_QUICK_ACCESS_3; s ++)
		if  ( inv->m_slots[s].m_pIItem 
			  && inv->m_slots[s].m_pIItem->object().cNameSect() == sect )
				return false;
		return true;
	}
	return false;
}
#endif

#include "../Medkit.h"
#include "../Antirad.h"
void CUIInventoryWnd::ActivatePropertiesBox()
{
	// Флаг-признак для невлючения пункта контекстного меню: Dreess Outfit, если костюм уже надет
	bool bAlreadyDressed = false; 

		
	UIPropertiesBox.RemoveAll();

	CMedkit*			pMedkit				= smart_cast<CMedkit*>			(CurrentIItem());
	CAntirad*			pAntirad			= smart_cast<CAntirad*>			(CurrentIItem());
	CEatableItem*		pEatableItem		= smart_cast<CEatableItem*>		(CurrentIItem());
	CCustomOutfit*		pOutfit				= smart_cast<CCustomOutfit*>	(CurrentIItem());
	CWeapon*			pWeapon				= smart_cast<CWeapon*>			(CurrentIItem());
	CScope*				pScope				= smart_cast<CScope*>			(CurrentIItem());
	CSilencer*			pSilencer			= smart_cast<CSilencer*>		(CurrentIItem());
	CGrenadeLauncher*	pGrenadeLauncher	= smart_cast<CGrenadeLauncher*>	(CurrentIItem());
	CBottleItem*		pBottleItem			= smart_cast<CBottleItem*>		(CurrentIItem());
    
	bool	b_show			= false;


#if defined(INV_NEW_SLOTS_SYSTEM)
	// Добавим в контекстное меню выбор слота. Real Wolf.
	auto slots = CurrentIItem()->GetSlots();
	char temp[64];
	for(u8 i = 0; i < (u8)slots.size(); ++i) 
	{
		if (slots[i] != NO_ACTIVE_SLOT && slots[i] != GRENADE_SLOT)
			if (!m_pInv->m_slots[slots[i]].m_pIItem || m_pInv->m_slots[slots[i]].m_pIItem != CurrentIItem() )
			{
				CEatableItem *eat = smart_cast<CEatableItem*>(CurrentIItem() );
				// Для еды разрешены только быстрые слоты.

				if (!eat || is_quick_slot(u32(slots[i]), CurrentIItem(), m_pInv) )
				{
					sprintf_s(temp, "st_move_to_slot%d", slots[i]);
					UIPropertiesBox.AddItem(temp,  NULL, INVENTORY_TO_SLOT0_ACTION + slots[i]);
					b_show = true;
				}
			}
	};
#else
	if(!pOutfit && CurrentIItem()->GetSlot()!=NO_ACTIVE_SLOT && !m_pInv->m_slots[CurrentIItem()->GetSlot()].m_bPersistent && m_pInv->CanPutInSlot(CurrentIItem()))
	{
		UIPropertiesBox.AddItem("st_move_to_slot",  NULL, INVENTORY_TO_SLOT_ACTION);
		b_show			= true;
	}
#endif

#if defined(GRENADE_FROM_BELT)
	if (CurrentIItem() != m_pInv->m_slots[GRENADE_SLOT].m_pIItem)
#endif
	if(CurrentIItem()->Belt() && m_pInv->CanPutInBelt(CurrentIItem()))
	{
		UIPropertiesBox.AddItem("st_move_on_belt",  NULL, INVENTORY_TO_BELT_ACTION);
		b_show			= true;
	}

#if defined(GRENADE_FROM_BELT)
	if (CurrentIItem()->GetSlot() == GRENADE_SLOT && CurrentIItem()->m_eItemPlace != eItemPlaceRuck)
	{
		UIPropertiesBox.AddItem("st_move_to_bag",  NULL, INVENTORY_TO_BAG_ACTION);
		bAlreadyDressed = true;
		b_show			= true;
	}
	else
#endif
	if(CurrentIItem()->Ruck() && m_pInv->CanPutInRuck(CurrentIItem()) && (CurrentIItem()->GetSlot()==u32(-1) || !m_pInv->m_slots[CurrentIItem()->GetSlot()].m_bPersistent) )
	{
		if(!pOutfit)
			UIPropertiesBox.AddItem("st_move_to_bag",  NULL, INVENTORY_TO_BAG_ACTION);
		else
			UIPropertiesBox.AddItem("st_undress_outfit",  NULL, INVENTORY_TO_BAG_ACTION);
		bAlreadyDressed = true;
		b_show			= true;
	}
#if !defined(INV_NEW_SLOTS_SYSTEM)
	if(pOutfit  && !bAlreadyDressed )
	{
		UIPropertiesBox.AddItem("st_dress_outfit",  NULL, INVENTORY_TO_SLOT_ACTION);
		b_show			= true;
	}
#endif
	
	//отсоединение аддонов от вещи
	if(pWeapon)
	{
		if(pWeapon->GrenadeLauncherAttachable() && pWeapon->IsGrenadeLauncherAttached())
		{
			UIPropertiesBox.AddItem("st_detach_gl",  NULL, INVENTORY_DETACH_GRENADE_LAUNCHER_ADDON);
		b_show			= true;
		}
		if(pWeapon->ScopeAttachable() && pWeapon->IsScopeAttached())
		{
			UIPropertiesBox.AddItem("st_detach_scope",  NULL, INVENTORY_DETACH_SCOPE_ADDON);
		b_show			= true;
		}
		if(pWeapon->SilencerAttachable() && pWeapon->IsSilencerAttached())
		{
			UIPropertiesBox.AddItem("st_detach_silencer",  NULL, INVENTORY_DETACH_SILENCER_ADDON);
		b_show			= true;
		}
		if(smart_cast<CWeaponMagazined*>(pWeapon) && IsGameTypeSingle())
		{
			bool b = (0!=pWeapon->GetAmmoElapsed());

			if(!b)
			{
				CUICellItem * itm = CurrentItem();
				for(u32 i=0; i<itm->ChildsCount(); ++i)
				{
					pWeapon		= smart_cast<CWeaponMagazined*>((CWeapon*)itm->Child(i)->m_pData);
					if(pWeapon->GetAmmoElapsed())
					{
						b = true;
						break;
					}
				}
			}

			if(b){
				UIPropertiesBox.AddItem("st_unload_magazine",  NULL, INVENTORY_UNLOAD_MAGAZINE);
				b_show			= true;
			}
		}
	}
	
	//присоединение аддонов к активному слоту (2 или 3)
	if(pScope)
	{
	#ifndef INV_NEW_SLOTS_SYSTEM
		if(m_pInv->m_slots[PISTOL_SLOT].m_pIItem != NULL &&
		   m_pInv->m_slots[PISTOL_SLOT].m_pIItem->CanAttach(pScope))
		 {
			PIItem tgt = m_pInv->m_slots[PISTOL_SLOT].m_pIItem;
			UIPropertiesBox.AddItem("st_attach_scope_to_pistol",  (void*)tgt, INVENTORY_ATTACH_ADDON);
			b_show			= true;
		 }
		 if(m_pInv->m_slots[RIFLE_SLOT].m_pIItem != NULL &&
			m_pInv->m_slots[RIFLE_SLOT].m_pIItem->CanAttach(pScope))
		 {
			PIItem tgt = m_pInv->m_slots[RIFLE_SLOT].m_pIItem;
			UIPropertiesBox.AddItem("st_attach_scope_to_rifle",  (void*)tgt, INVENTORY_ATTACH_ADDON);
			b_show			= true;
		 }
	#else
		for(u8 i = 0; i < OUTFIT_SLOT; ++i) 
		{
			 if(m_pInv->m_slots[i].m_pIItem && m_pInv->m_slots[i].m_pIItem->CanAttach(pScope) )
			 {
				PIItem tgt = m_pInv->m_slots[i].m_pIItem;
				sprintf_s(temp, "st_attach_scope_to_%d", i);
				UIPropertiesBox.AddItem(temp,  (void*)tgt, INVENTORY_ATTACH_ADDON);
				b_show			= true;
			 }
		};	
	#endif
	}
	else if(pSilencer)
	{
	#ifndef INV_NEW_SLOTS_SYSTEM
		 if(m_pInv->m_slots[PISTOL_SLOT].m_pIItem != NULL &&
		   m_pInv->m_slots[PISTOL_SLOT].m_pIItem->CanAttach(pSilencer))
		 {
			PIItem tgt = m_pInv->m_slots[PISTOL_SLOT].m_pIItem;
			UIPropertiesBox.AddItem("st_attach_silencer_to_pistol",  (void*)tgt, INVENTORY_ATTACH_ADDON);
			b_show			= true;
		 }
		 if(m_pInv->m_slots[RIFLE_SLOT].m_pIItem != NULL &&
			m_pInv->m_slots[RIFLE_SLOT].m_pIItem->CanAttach(pSilencer))
		 {
			PIItem tgt = m_pInv->m_slots[RIFLE_SLOT].m_pIItem;
			UIPropertiesBox.AddItem("st_attach_silencer_to_rifle",  (void*)tgt, INVENTORY_ATTACH_ADDON);
			b_show			= true;
		 }
	#else
		for(u8 i = 0; i < OUTFIT_SLOT; ++i) 
		{
			 if(m_pInv->m_slots[i].m_pIItem != NULL && m_pInv->m_slots[i].m_pIItem->CanAttach(pSilencer))
			 {
				PIItem tgt = m_pInv->m_slots[i].m_pIItem;
				sprintf_s(temp, "st_attach_silencer_to_%d", i);
				UIPropertiesBox.AddItem(temp,  (void*)tgt, INVENTORY_ATTACH_ADDON);
				b_show			= true;
			 }
		}	
	#endif
	}
	else if(pGrenadeLauncher)
	{
	#ifndef INV_NEW_SLOTS_SYSTEM
		 if(m_pInv->m_slots[RIFLE_SLOT].m_pIItem != NULL &&
			m_pInv->m_slots[RIFLE_SLOT].m_pIItem->CanAttach(pGrenadeLauncher))
		 {
			PIItem tgt = m_pInv->m_slots[RIFLE_SLOT].m_pIItem;
			UIPropertiesBox.AddItem("st_attach_gl_to_rifle",  (void*)tgt, INVENTORY_ATTACH_ADDON);
			b_show			= true;
		 }
	#else
	for(u8 i = 0; i < OUTFIT_SLOT; ++i) 
	{
		 if(m_pInv->m_slots[i].m_pIItem && m_pInv->m_slots[i].m_pIItem->CanAttach(pGrenadeLauncher))
		 {
			PIItem tgt = m_pInv->m_slots[i].m_pIItem;
			sprintf_s(temp, "st_attach_gl_to_%d", i);
			UIPropertiesBox.AddItem(temp,  (void*)tgt, INVENTORY_ATTACH_ADDON);
			b_show			= true;
		 }
	};		
	#endif
	}
	LPCSTR _action = NULL;

	if(pMedkit || pAntirad)
	{
		_action					= "st_use";
	}
	else if(pEatableItem)
	{
		if(pBottleItem)
			_action					= "st_drink";
		else
			_action					= "st_eat";
	}

	if(_action){
		UIPropertiesBox.AddItem(_action,  NULL, INVENTORY_EAT_ACTION);
		b_show			= true;
	}

	bool disallow_drop	= (pOutfit&&bAlreadyDressed);
	disallow_drop		|= !!CurrentIItem()->IsQuestItem();

	if(!disallow_drop)
	{

		UIPropertiesBox.AddItem("st_drop", NULL, INVENTORY_DROP_ACTION);
		b_show			= true;

		if(CurrentItem()->ChildsCount())
			UIPropertiesBox.AddItem("st_drop_all", (void*)33, INVENTORY_DROP_ACTION);
	}

	if(b_show)
	{
		UIPropertiesBox.AutoUpdateSize	();
		UIPropertiesBox.BringAllToTop	();

		Fvector2						cursor_pos;
		Frect							vis_rect;
		GetAbsoluteRect					(vis_rect);
		cursor_pos						= GetUICursor()->GetCursorPosition();
		cursor_pos.sub					(vis_rect.lt);
		UIPropertiesBox.Show			(vis_rect, cursor_pos);
		PlaySnd							(eInvProperties);
	}
}

void CUIInventoryWnd::ProcessPropertiesBoxClicked	()
{
	if(UIPropertiesBox.GetClickedItem())
	{
		auto num = UIPropertiesBox.GetClickedItem()->GetTAG();
#ifdef INV_NEW_SLOTS_SYSTEM
		if (num >= INVENTORY_TO_SLOT0_ACTION && num <= INVENTORY_TO_SLOT15_ACTION)
		{
			switch(num)
			{
			case INVENTORY_TO_SLOT0_ACTION:
				CurrentIItem()->SetSlot(KNIFE_SLOT);
				break;
			case INVENTORY_TO_SLOT1_ACTION:
				CurrentIItem()->SetSlot(PISTOL_SLOT);
				break;
			case INVENTORY_TO_SLOT2_ACTION:
				CurrentIItem()->SetSlot(RIFLE_SLOT);
				break;
			case INVENTORY_TO_SLOT3_ACTION:
				CurrentIItem()->SetSlot(GRENADE_SLOT);
				break;
			case INVENTORY_TO_SLOT4_ACTION:
				CurrentIItem()->SetSlot(APPARATUS_SLOT);
				break;
			case INVENTORY_TO_SLOT5_ACTION:
				CurrentIItem()->SetSlot(BOLT_SLOT);
				break;
			case INVENTORY_TO_SLOT6_ACTION:
				CurrentIItem()->SetSlot(OUTFIT_SLOT);
				break;
			case INVENTORY_TO_SLOT7_ACTION:
				CurrentIItem()->SetSlot(PDA_SLOT);
				break;
			case INVENTORY_TO_SLOT8_ACTION:
				CurrentIItem()->SetSlot(DETECTOR_SLOT);
				break;
			case INVENTORY_TO_SLOT9_ACTION:
				CurrentIItem()->SetSlot(TORCH_SLOT);
				break;
			case INVENTORY_TO_SLOT10_ACTION:
				CurrentIItem()->SetSlot(ARTEFACT_SLOT);
				break;
			case INVENTORY_TO_SLOT11_ACTION:
				CurrentIItem()->SetSlot(HELMET_SLOT);
				break;
			case INVENTORY_TO_SLOT12_ACTION:
				CurrentIItem()->SetSlot(SLOT_QUICK_ACCESS_0);
				break;
			case INVENTORY_TO_SLOT13_ACTION:
				CurrentIItem()->SetSlot(SLOT_QUICK_ACCESS_1);
				break;
			case INVENTORY_TO_SLOT14_ACTION:
				CurrentIItem()->SetSlot(SLOT_QUICK_ACCESS_2);
				break;
			case INVENTORY_TO_SLOT15_ACTION:
				CurrentIItem()->SetSlot(SLOT_QUICK_ACCESS_3);
				break;
			}
			ToSlot(CurrentItem(), true);
			return;
		}
#endif
		switch(num)
		{
		case INVENTORY_TO_SLOT_ACTION:	
			ToSlot(CurrentItem(), true);
			break;
		case INVENTORY_TO_BELT_ACTION:	
			ToBelt(CurrentItem(),false);
			break;
		case INVENTORY_TO_BAG_ACTION:	
			ToBag(CurrentItem(),false);
			break;
		case INVENTORY_DROP_ACTION:
			{
				void* d = UIPropertiesBox.GetClickedItem()->GetData();
				bool b_all = (d==(void*)33);

				DropCurrentItem(b_all);
			}break;
		case INVENTORY_EAT_ACTION:
			EatItem(CurrentIItem());
			break;
		case INVENTORY_ATTACH_ADDON:
			AttachAddon((PIItem)(UIPropertiesBox.GetClickedItem()->GetData()));
			break;
		case INVENTORY_DETACH_SCOPE_ADDON:
			DetachAddon(*(smart_cast<CWeapon*>(CurrentIItem()))->GetScopeName());
			break;
		case INVENTORY_DETACH_SILENCER_ADDON:
			DetachAddon(*(smart_cast<CWeapon*>(CurrentIItem()))->GetSilencerName());
			break;
		case INVENTORY_DETACH_GRENADE_LAUNCHER_ADDON:
			DetachAddon(*(smart_cast<CWeapon*>(CurrentIItem()))->GetGrenadeLauncherName());
			break;
		case INVENTORY_RELOAD_MAGAZINE:
			(smart_cast<CWeapon*>(CurrentIItem()))->Action(kWPN_RELOAD, CMD_START);
			break;
		case INVENTORY_UNLOAD_MAGAZINE:
			{
				CUICellItem * itm = CurrentItem();
				(smart_cast<CWeaponMagazined*>((CWeapon*)itm->m_pData))->UnloadMagazine();
				for(u32 i=0; i<itm->ChildsCount(); ++i)
				{
					CUICellItem * child_itm			= itm->Child(i);
					(smart_cast<CWeaponMagazined*>((CWeapon*)child_itm->m_pData))->UnloadMagazine();
				}
			}break;
		}
	}
}

bool CUIInventoryWnd::TryUseItem(PIItem itm)
{
	CBottleItem*		pBottleItem			= smart_cast<CBottleItem*>		(itm);
	CMedkit*			pMedkit				= smart_cast<CMedkit*>			(itm);
	CAntirad*			pAntirad			= smart_cast<CAntirad*>			(itm);
	CEatableItem*		pEatableItem		= smart_cast<CEatableItem*>		(itm);

	if(pMedkit || pAntirad || pEatableItem || pBottleItem)
	{
		EatItem(itm);
		return true;
	}
	return false;
}

bool CUIInventoryWnd::DropItem(PIItem itm, CUIDragDropListEx* lst)
{
	if(lst==m_pUIOutfitList)
	{
		return TryUseItem			(itm);
/*
		CCustomOutfit*		pOutfit		= smart_cast<CCustomOutfit*>	(CurrentIItem());
		if(pOutfit)
			ToSlot			(CurrentItem(), true);
		else
			EatItem				(CurrentIItem());

		return				true;
*/
	}
	CUICellItem*	_citem	= lst->ItemsCount() ? lst->GetItemIdx(0) : NULL;
	PIItem _iitem	= _citem ? (PIItem)_citem->m_pData : NULL;

	if(!_iitem)						return	false;
	if(!_iitem->CanAttach(itm))		return	false;
	AttachAddon						(_iitem);

	return							true;
}
