#include "stdafx.h"
#include "UICellCustomItems.h"
#include "UIInventoryUtilities.h"
#include "../Weapon.h"
#include "UIDragDropListEx.h"

#include "../pch_script.h"
#include "../game_object_space.h"
#include "../script_callback_ex.h"
#include "../script_game_object.h"
#include "../Actor.h"


#define INV_GRID_WIDTHF			50.0f
#define INV_GRID_HEIGHTF		50.0f

CUIInventoryCellItem::CUIInventoryCellItem(CInventoryItem* itm)
{
	m_pData											= (void*)itm;
	itm->m_cell_item								= this;

	inherited::SetShader							(InventoryUtilities::GetEquipmentIconsShader());

	m_grid_size.set									(itm->GetGridWidth(),itm->GetGridHeight());	
	Frect rect; 
	rect.lt.set										(	INV_GRID_WIDTHF*itm->GetXPos(), 
														INV_GRID_HEIGHTF*itm->GetYPos() );

	rect.rb.set										(	rect.lt.x+INV_GRID_WIDTHF*m_grid_size.x, 
														rect.lt.y+INV_GRID_HEIGHTF*m_grid_size.y);

	inherited::SetOriginalRect						(rect);
	inherited::SetStretchTexture					(true);
	b_auto_drag_childs = true;
}

bool CUIInventoryCellItem::EqualTo(CUICellItem* itm)
{
	CUIInventoryCellItem* ci = smart_cast<CUIInventoryCellItem*>(itm);
	if(!itm)				return false;
	// Real Wolf: Колбек на группировку и само регулирование группировкой предметов. 12.08.2014.
	auto item1 = (CInventoryItem*)m_pData;
	auto item2 = (CInventoryItem*)itm->m_pData;

	g_actor->callback(GameObject::eUIGroupItems)(item1->object().lua_game_object(), item2->object().lua_game_object() );

	auto fl1 = item1->m_flags;
	auto fl2 = item2->m_flags;

	item1->m_flags.set(CInventoryItem::FIUngroupable, false);
	item2->m_flags.set(CInventoryItem::FIUngroupable, false);

	if (fl1.test(CInventoryItem::FIUngroupable) || fl2.test(CInventoryItem::FIUngroupable) )
		return false;

	return					(
								fsimilar(object()->GetCondition(), ci->object()->GetCondition(), 0.01f) &&
								(object()->object().cNameSect() == ci->object()->object().cNameSect())
							);
}

CUIInventoryCellItem::~CUIInventoryCellItem()
{
	if (auto item = object() )
		item->m_cell_item = NULL;
}

void CUIInventoryCellItem::OnFocusReceive()
{
	inherited::OnFocusReceive();
	auto script_obj = object()->object().lua_game_object();
	g_actor->callback(GameObject::eCellItemFocus)(script_obj);
}

void CUIInventoryCellItem::OnFocusLost()
{
	inherited::OnFocusLost();	
	auto script_obj = object()->object().lua_game_object();
	g_actor->callback(GameObject::eCellItemFocusLost)(script_obj);
}

bool CUIInventoryCellItem::OnMouse(float x, float y, EUIMessages action)
{
	inherited::OnMouse(x, y, action);

	//if (m_bCursorOverWindow)
	g_actor->callback(GameObject::eOnCellItemMouse)(object()->object().lua_game_object(), x, y, action);

	return false;
}


CUIDragItem* CUIInventoryCellItem::CreateDragItem()
{
	CUIDragItem* i		= inherited::CreateDragItem();
	CUIStatic* s		= NULL;
	
	// в отдельном случае не требуется автоматика(!)
	//if (b_auto_drag_childs)
	//{
		// Real Wolf: Вместо частного решения, сделаем общее для всех детей-статиков. Необходимые параметры добавляйте сами. 25.07.2014.
		for (auto it = this->m_ChildWndList.begin(); it != this->m_ChildWndList.end(); ++it)
		{
			if (auto s_child = smart_cast<CUIStatic*>(*it))
			{
				// Real Wolf: Скопируем все кроме аддонов, для них по-другому.
				if (s_child->WindowName() != "upgrade")
				{
					s = xr_new<CUIStatic>();
					s->SetAutoDelete(true);
					s->SetShader(InventoryUtilities::GetEquipmentIconsShader());

					s->SetWndRect(s_child->GetWndRect());
					s->SetOriginalRect(s_child->GetOriginalRect());

					s->SetStretchTexture(s_child->GetStretchTexture());
					s->SetText(s_child->GetText());

					if (auto text = s_child->GetTextureName())
						s->InitTextureEx(text, s_child->GetShaderName());

					//s->SetColor				(i->wnd()->GetColor());
					s->SetHeading(i->wnd()->Heading());
					i->wnd()->AttachChild(s);
				}
			}
		}
	//}

	// Real Wolf: цвет как у родителя + стандартная прозрачность. 27.12.14
	auto	color = GetColor();
	auto	R = color_get_R(color);
	auto	G = color_get_G(color);
	auto	B = color_get_B(color);
	i->wnd()->SetColorAll(color_argb(170, R, G, B));
	i->wnd()->SetText(GetText());
	i->wnd()->SetTextPos(GetTextX(), GetTextY());
	i->wnd()->SetTextAlignment(GetTextAlignment());
	i->wnd()->SetTextColor(GetTextColor());

	return	i;
}

CUIAmmoCellItem::CUIAmmoCellItem(CWeaponAmmo* itm)
:inherited(itm)
{}

bool CUIAmmoCellItem::EqualTo(CUICellItem* itm)
{
	if(!inherited::EqualTo(itm))	return false;

	CUIAmmoCellItem* ci				= smart_cast<CUIAmmoCellItem*>(itm);
	if(!ci)							return false;

	return					( (object()->cNameSect() == ci->object()->cNameSect()) );
}

void CUIAmmoCellItem::Update()
{
	inherited::Update	();
	UpdateItemText		();
}

void CUIAmmoCellItem::UpdateItemText()
{
	if(NULL==m_custom_draw)
	{
		xr_vector<CUICellItem*>::iterator it = m_childs.begin();
		xr_vector<CUICellItem*>::iterator it_e = m_childs.end();
		
		u16 total				= object()->m_boxCurr;
		for(;it!=it_e;++it)
			total				= total + ((CUIAmmoCellItem*)(*it))->object()->m_boxCurr;

		string32				str;
		sprintf_s					(str,"%d",total);

		SetText					(str);
	}else
	{
		SetText					("");
	}
}


CUIWeaponCellItem::CUIWeaponCellItem(CWeapon* itm)
:inherited(itm)
{
	m_addons[eSilencer]		= NULL;
	m_addons[eScope]		= NULL;
	m_addons[eLauncher]		= NULL;

	m_cell_size.set (INV_GRID_WIDTHF, INV_GRID_HEIGHTF); // alpet: это просто значения по умолчанию

	if(itm->SilencerAttachable())
		m_addon_offset[eSilencer].set(object()->GetSilencerX(), object()->GetSilencerY());

	if(itm->ScopeAttachable())
		m_addon_offset[eScope].set(object()->GetScopeX(), object()->GetScopeY());

	if(itm->GrenadeLauncherAttachable())
		m_addon_offset[eLauncher].set(object()->GetGrenadeLauncherX(), object()->GetGrenadeLauncherY());
}

#include "../object_broker.h"
CUIWeaponCellItem::~CUIWeaponCellItem()
{
}

bool CUIWeaponCellItem::is_scope()
{
	return object()->ScopeAttachable()&&object()->IsScopeAttached();
}

bool CUIWeaponCellItem::is_silencer()
{
	return object()->SilencerAttachable()&&object()->IsSilencerAttached();
}

bool CUIWeaponCellItem::is_launcher()
{
	return object()->GrenadeLauncherAttachable()&&object()->IsGrenadeLauncherAttached();
}

void CUIWeaponCellItem::CreateIcon(eAddonType t)
{
	if (GetIcon(t)) return;  // накапливаются дочерние статитики из-за добавления вертикального отображения. Не работает автоудаление?
	m_addons[t]					= xr_new<CUIStatic>();	
	m_addons[t]->SetAutoDelete	(true);
	AttachChild					(m_addons[t]);
	m_addons[t]->SetShader		(InventoryUtilities::GetEquipmentIconsShader());
}

void CUIWeaponCellItem::DestroyIcon(eAddonType t)
{
	DetachChild		(m_addons[t]);
	m_addons[t]		= NULL;
}

CUIStatic* CUIWeaponCellItem::GetIcon(eAddonType t)
{
	return m_addons[t];
}

void CUIWeaponCellItem::Update()
{
	bool b						= Heading();
	inherited::Update			();
	
	bool bForceReInitAddons		= (b!=Heading()); 

	if (object()->SilencerAttachable()){
		if (object()->IsSilencerAttached())
		{
			if (!GetIcon(eSilencer) || bForceReInitAddons)
			{
				CreateIcon	(eSilencer);
				InitAddon	(GetIcon(eSilencer), *object()->GetSilencerName(), m_addon_offset[eSilencer], Heading());
			}
		}
		else
		{
			if (m_addons[eSilencer])
				DestroyIcon(eSilencer);
		}
	}

	if (object()->ScopeAttachable()){
		if (object()->IsScopeAttached())
		{
			if (!GetIcon(eScope) || bForceReInitAddons)
			{
				CreateIcon	(eScope);
				InitAddon	(GetIcon(eScope), *object()->GetScopeName(), m_addon_offset[eScope], Heading());
			}
		}
		else
		{
			if (m_addons[eScope])
				DestroyIcon(eScope);
		}
	}

	if (object()->GrenadeLauncherAttachable()){
		if (object()->IsGrenadeLauncherAttached())
		{
			if (!GetIcon(eLauncher) || bForceReInitAddons)
			{
				CreateIcon	(eLauncher);
				InitAddon	(GetIcon(eLauncher), *object()->GetGrenadeLauncherName(), m_addon_offset[eLauncher], Heading());
			}
		}
		else
		{
			if (m_addons[eLauncher])
				DestroyIcon(eLauncher);
		}
	}
}

void CUIWeaponCellItem::OnAfterChild(CUIDragDropListEx* parent_list)
{
	const Ivector2 &cs = parent_list->CellSize();
	m_cell_size.set( (float)cs.x, (float)cs.y );

	CUIStatic* s_silencer	= is_silencer() ?	GetIcon(eSilencer):		NULL;
	CUIStatic* s_scope		= is_scope() ?		GetIcon(eScope):		NULL;
	CUIStatic* s_launcher	= is_launcher() ?	GetIcon(eLauncher):		NULL;

	InitAllAddons(s_silencer, s_scope, s_launcher, parent_list->GetVerticalPlacement());
}

void CUIWeaponCellItem::InitAllAddons(CUIStatic* s_silencer, CUIStatic* s_scope, CUIStatic* s_launcher, bool b_vertical)
{
	
	if (s_silencer) 
		InitAddon	(s_silencer, *object()->GetSilencerName(),	      m_addon_offset[eSilencer], b_vertical);
	if (s_scope) 
		InitAddon	(s_scope,	 *object()->GetScopeName(),		      m_addon_offset[eScope], b_vertical);	
	if (s_launcher)
		InitAddon	(s_launcher, *object()->GetGrenadeLauncherName(), m_addon_offset[eLauncher], b_vertical);

}

void CUIWeaponCellItem::InitAddon(CUIStatic* s, LPCSTR section, Fvector2 addon_offset, bool b_rotate)
{
	
		Frect					tex_rect;
		Fvector2				base_scale;
		Fvector2				inventory_size;
		Fvector2				expected_size;
		static int method = 1;


		inventory_size.x = INV_GRID_WIDTHF  * m_grid_size.x;
		inventory_size.y = INV_GRID_HEIGHTF * m_grid_size.y;
		// m_grid_size содержит информацию об размерах иконки оружия Columns x Rows
		expected_size.x = m_cell_size.x * m_grid_size.x; // vert: cell_size = 40x50, grid_size = 5x2 
		expected_size.y = m_cell_size.y * m_grid_size.y; 			
		if (Heading())
		{   // h = 250, w = 80, i.x = 300, i.y = 100	
			if (1 == method)
			{   // надо поменять местами height & width.
				expected_size.x = m_cell_size.y * m_grid_size.x;
				expected_size.y = m_cell_size.x * m_grid_size.y;
			}			
			if (2 == method)
			{   // надо поменять местами height & width + пропорции
				expected_size.x = m_cell_size.y * m_grid_size.y;
				expected_size.y = m_cell_size.x * m_grid_size.x;
			}

			base_scale.x = GetHeight() / inventory_size.x;  // допущение, что происходит стретч сетки, вместо появления полосы прокрутки
			base_scale.y = GetWidth()  / inventory_size.y;  // или, что указанные размеры контрола в конфиге полностью пропорциональны его сетке
			
		}else
		{
			base_scale.x = GetWidth()   /  inventory_size.x;
			base_scale.y = GetHeight()  /  inventory_size.y;			
		} 

		if (method > 0)
		{   // тестовый метод, если размеры ячеек не подстраиваются под размеры окна
			base_scale.x = expected_size.x / inventory_size.x;
			base_scale.y = expected_size.y / inventory_size.y;
		}		

		Fvector2				cell_size;

		cell_size.x				= pSettings->r_u32(section, "inv_grid_width")*INV_GRID_WIDTHF; // это допущение, что ячейки инвентаря 50х50 всегда? )
		cell_size.y				= pSettings->r_u32(section, "inv_grid_height")*INV_GRID_HEIGHTF;

		tex_rect.x1				= pSettings->r_u32(section, "inv_grid_x")*INV_GRID_WIDTHF;
		tex_rect.y1				= pSettings->r_u32(section, "inv_grid_y")*INV_GRID_HEIGHTF;

		tex_rect.rb.add			(tex_rect.lt,cell_size);

		cell_size.mul			(base_scale);
		if(b_rotate)
		{
			s->SetWndSize		(Fvector2().set(cell_size.y, cell_size.x) );
			Fvector2 new_offset;
#ifdef DEBUG_SLOTS
			Msg(" original icon offset = %.3f x %.3f, base_scale = %.3f x %.3f", addon_offset.x, addon_offset.y, base_scale.x, base_scale.y);
#endif
			float correct		= addon_offset.y > 0.f ? 1.f : ( addon_offset.y < 0.f ? -1.f : 0.f );
			new_offset.x		= addon_offset.y * base_scale.y - correct;
			if (method > 0)
			    new_offset.y		= expected_size.x - addon_offset.x*base_scale.x - cell_size.x;
			else
				new_offset.y		= GetHeight()	   - addon_offset.x*base_scale.x - cell_size.y;
			addon_offset		= new_offset;
#ifdef DEBUG_SLOTS
			Msg(" rotated icon offset = %.3f x %.3f", addon_offset.x, addon_offset.y);
#endif
		}else
		{
			s->SetWndSize		(cell_size);
			addon_offset.mul	(base_scale);
		} 
		s->SetWndPos			(addon_offset);
		s->SetOriginalRect		(tex_rect);
		s->SetStretchTexture	(true);

		s->EnableHeading		(b_rotate);
		
		if(b_rotate)
		{
			s->SetHeading			(GetHeading());
			Fvector2 offs;
			offs.set				(0.0f, s->GetWndSize().y);
			s->SetHeadingPivot		(Fvector2().set(0.0f,0.0f), /*Fvector2().set(0.0f,0.0f)*/offs, true);
		}

		s->SetWindowName("upgrade");
}

CUIStatic *MakeAddonStatic(CUIDragItem* i)
{
	CUIStatic* s		= xr_new<CUIStatic>();
	s->SetShader	(InventoryUtilities::GetEquipmentIconsShader());
	s->SetAutoDelete(true);
	s->SetColor		(i->wnd()->GetColor());
	i->wnd			()->AttachChild	(s);
	return s;
}

CUIDragItem* CUIWeaponCellItem::CreateDragItem()
{
	b_auto_drag_childs = false;
 	CUIDragItem* i		= inherited::CreateDragItem();

	CUIStatic* s_silencer	= GetIcon(eSilencer) ? MakeAddonStatic(i) : NULL;
	CUIStatic* s_scope		= GetIcon(eScope)    ? MakeAddonStatic(i) : NULL;
	CUIStatic* s_launcher	= GetIcon(eLauncher) ? MakeAddonStatic(i) : NULL;		
	if (Heading()) m_cell_size.set(m_cell_size.y, m_cell_size.x);   // swap before	
	InitAllAddons(s_silencer, s_scope, s_launcher, false);	
	if (Heading()) m_cell_size.set( m_cell_size.y, m_cell_size.x );  // swap after

	return				i;
} 

bool CUIWeaponCellItem::EqualTo(CUICellItem* itm)
{
	if(!inherited::EqualTo(itm))	return false;

	CUIWeaponCellItem* ci			= smart_cast<CUIWeaponCellItem*>(itm);
	if(!ci)							return false;

	bool b_addons					= ( (object()->GetAddonsState() == ci->object()->GetAddonsState()) );
	bool b_place					= ( (object()->m_eItemPlace == ci->object()->m_eItemPlace) );
	
	return							b_addons && b_place;
}

Ivector2 CUIWeaponCellItem::GetGridSize(bool with_childs)
{
	auto child_grid_size = m_grid_size;

	// Real Wolf: Для того, чтобы размер ячейки учитывал и размеры улучшений.01.02.15.
	if (with_childs && !Heading())
	{
		Fvector2	inventory_size;
			inventory_size.x = INV_GRID_WIDTHF  * m_grid_size.x;
			inventory_size.y = INV_GRID_HEIGHTF * m_grid_size.y;

		auto grid_size	= inventory_size;
			grid_size.x /= m_grid_size.x;
			grid_size.y /= m_grid_size.y;

		auto obj		= object();

		str_c addons[eMaxAddon]; std::memset(addons, NULL, sizeof(addons));

		if (obj->IsSilencerAttached())
			addons[eSilencer]	= obj->GetSilencerName().c_str();

		if (obj->IsScopeAttached())
			addons[eScope]		= obj->GetScopeName().c_str();

		if (obj->IsGrenadeLauncherAttached())
			addons[eLauncher]	= obj->GetGrenadeLauncherName().c_str();

		for (int it = eSilencer; it < eMaxAddon; ++it)
		{
			if (addons[it] == NULL)
				continue;

			auto delta	= m_addon_offset[it];

			Fvector2	cell_size;
				cell_size.x	= pSettings->r_u32(addons[it], "inv_grid_width")	*INV_GRID_WIDTHF;
				cell_size.y = pSettings->r_u32(addons[it], "inv_grid_height")	*INV_GRID_HEIGHTF;

			delta.add(cell_size);
			
			auto delta_x = delta.x - inventory_size.x;
			if (delta_x > 0)
			{
				auto num = (int)std::ceil(delta_x / grid_size.x);

				child_grid_size.x	+= num;
				inventory_size.x	+= num * grid_size.x;
			}

			auto delta_y = delta.y - inventory_size.y;
			if (delta_y > 0)
			{
				auto num = (int)std::ceil(delta_y / grid_size.y);

				child_grid_size.y	+= num;
				inventory_size.y	+= num * grid_size.y;
			}
		}
	}

	return child_grid_size;
}

CBuyItemCustomDrawCell::CBuyItemCustomDrawCell	(LPCSTR str, CGameFont* pFont)
{
	m_pFont		= pFont;
	VERIFY		(xr_strlen(str)<16);
	strcpy		(m_string,str);
}

void CBuyItemCustomDrawCell::OnDraw(CUICellItem* cell)
{
	Fvector2							pos;
	cell->GetAbsolutePos				(pos);
	UI()->ClientToScreenScaled			(pos, pos.x, pos.y);
	m_pFont->Out						(pos.x, pos.y, m_string);
	m_pFont->OnRender					();
}
	