#pragma once
// ==================================== Правки от alpet ======================================= 
#define HLAMP_AFFECT_IMMUNITIES			// включает обработку иммунитетов для висячих ламп (CHangingLamp)
// #define LUAICP_COMPAT				// включает совместимость с перехватчиком Lua (luaicp.dll). Задавать надо в свойствах 
#define SCRIPT_ICONS_CONTROL		    // включает экспорт функции setup_game_icon в Lua
#define SCRIPT_EZI_CONTROL				// включает экспорт класса CEffectorZoomInertion для управления движением прицела 
#define SPAWN_ANTIFREEZE				// распределяет массовый спавн объектов по кадрам, чтобы исключить продолжительные фризы 
#define ECO_RENDER						// добавляет небольшую задержку между рендерингом кадров, чтобы зря не насиловать видеокарту при больших FPS
// #define MT_OPT						// задает выделенные ядра ЦП для второго потока
#define AF_SHOW_DYNAMIC_PARAMS			// для артефактов будут выводиться действующие свойства, а не из файла конфигурации
#define SCRIPT_VARS_STORAGE				// включает хранилище скриптовых переменных в сейвы


// ==================================== Правки от K.D. =======================================
#define KD_DETAIL_RADIUS				// alpet: включает правку дистанции отрисовки травы и плотности. Может влиять на производительность для слабых систем.

// ==================================== Правки от Real Wolf ======================================= 
//#define INV_RUCK_UNLIMITED_FIX		// Real Wolf: позволяет безболезненно использовать атрибут unlimited в теге dragdrop_bag для создания лимитного инвентаря
#define INV_NEW_SLOTS_SYSTEM			// Real Wolf, Red Virus: включает слоты.
#define SUN_DIR_NOT_DEBUG			    // Real Wolf: отключение вывода в лог информации вида CurrentEnv.sun_dir...
//#define ARTEFACTS_FROM_RUCK			// Real Wolf: артефакты будут работать из всего инвентаря (слотов, пояса, рюкзака).
#define KNIFE_SPRINT_FIX				// Real Wolf: остановка спринта при ударе2.
#define KNIFE_SPRINT_MOTION				// Real Wolf: вопроизведение анимации бега для ножа.
#define MISSILE_THREAT_FIX				// Real Wolf: остановка спринта при броске гранаты, болта и т.д.
//#define GRENADE_FROM_BELT				// Real Wolf: гранаты с пояса.
//#define LOCK_RELOAD_IN_SPRINT			// Real Wolf: блокировка перезарядки по время спринта.
#define HIDE_WEAPON_IN_CAR				// Real Wolf: прятать все оружие в машине.
//#define BM16_ANIMS_FIX					// Real Wolf: расширение анимаций для BM16.
//#define INV_OUTFIT_FULL_ICON_HIDE		// Real Wolf: иконка игрока в костюме заменена на иконку костюма.
#define UI_LOCK_PDA_WITHOUT_PDA_IN_SLOT // Real Wolf: блокировать работу ПДА, если самого предмета нету в слоте.
#define CAR_SAVE_FUEL					// Real Wolf: сохранение текущего объема топлива, максимального объема и потребления машины.
//#define R1_EXCLUDE						// Real Wolf: отключает первый рендер, оставляя только полное динамическое освещение.

// ==================================== Правки от Red Virus ======================================= 
#define WPN_BOBBING						// Red Virus: bobbing effect from lost alpha

#ifdef INV_NEW_SLOTS_SYSTEM
	#define INV_NO_ACTIVATE_APPARATUS_SLOT  // Red Virus: убирает невозможность сменить оружие клавишами при активных слотах:  фонарика, детектора и тд
	#define INV_MOVE_ITM_INTO_QUICK_SLOTS	// Red Virus: позволяет менять местами предметы в быстрых слотах
//	#define INV_QUICK_SLOT_PANEL		// Red Virus: панель быстрых слотов на главном экране
//	#define INV_COLORIZE_AMMO				// Red Virus: colorize ammo from lost alpha
//	#define INV_COLORIZE					// Red Virus: colorize from lost alpha
//	#define INV_FLOAT_ITEM_INFO				// Red Virus: окно информации о предмете рядом с предметом
#endif

// ==================================== Правки от Karlan ======================================= 
//#define AMMO_FROM_BELT					// Karlan: Патроны используются только с пояса

