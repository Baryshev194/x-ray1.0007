// HUDCrosshair.h:  ������� �������, ������������ ������� ���������
// 
//////////////////////////////////////////////////////////////////////

#pragma once

#define HUD_CURSOR_SECTION "hud_cursor"


class CHUDCrosshair	
{
private:
	float			cross_length_perc;
	float			min_radius_perc;
	float			max_radius_perc;
	bool			m_is_enabled;

	//������� ������ �������
	float			radius;
	float			target_radius;
	float			radius_speed_perc; 

	ref_geom 		hGeomLine;
	ref_shader		hShader;
public:
	u32				cross_color;

					CHUDCrosshair	();
					~CHUDCrosshair	();

			void	OnRender		();
			void	SetDispersion	(float disp);

			void	Load			();
			bool	IsEnabled		() {return m_is_enabled;};
};