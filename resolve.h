// WallpaperChanger v1.3 by estuibal.
// mod by plum.
// Thanks to estuibal!

#ifndef __RESOLVE_H__
#define __RESOLVE_H__

// header
#include "wallpaper_changer.h"

// struct
typedef struct
{
	u32 setting_bk_funcaddr;
	u32 save_wp_funcaddr;
	u32 open_img_funcaddr;
	u32 disable_preview_addr;
	u32 save_wp_addr;
	u32 open_wp_addr;
	u32 vsh_pwcb_addr;
	u32 jump_vshctrl_addr;
} OffsetList;

typedef struct
{
	u32 version;
	OffsetList *list;
} Patches;

// global
OffsetList list_620 =
{
	0x31EB0,	// vshSettingBackground_func_addr
	0x37158,	// vshSaveWallpaper_func_addr
	0x3E7D0,	// scePafOpenImage_func_addr
	0x320F0,	// vshSettingBackground_addr
	0x32164,	// vshSaveWallpaperPatched_addr
	0x31E68,	// scePafOpenImagePatched_addr
	0xF96C,		// sceKernelCreateCallbackPatched_addr
	0x14948,	// vshCtrlReadBufferPositive_jump_addr
};

OffsetList list_63X =
{
	0x32760,	// vshSettingBackground_func_addr
	0x37A04,	// vshSaveWallpaper_func_addr
	0x3F044,	// scePafOpenImage_func_addr
	0x329A0,	// vshSettingBackground_addr
	0x32A14,	// vshSaveWallpaperPatched_addr
	0x32704,	// scePafOpenImagePatched_addr
	0xFE60,		// sceKernelCreateCallbackPatched_addr
	0x14F64,	// vshCtrlReadBufferPositive_jump_addr
};

OffsetList list_660 =
{
	0x327EC,	// vshSettingBackground_func_addr
	0x37A90,	// vshSaveWallpaper_func_addr
	0x3F348,	// scePafOpenImage_func_addr
	0x32A2C,	// vshSettingBackground_addr
	0x32AA0,	// vshSaveWallpaperPatched_addr
	0x32790,	// scePafOpenImagePatched_addr
	0xFEE0,		// sceKernelCreateCallbackPatched_addr
	0x1502C,	// vshCtrlReadBufferPositive_jump_addr
};

Patches patch_list[] =
{
	{ 0x620, &list_620 },
	{ 0x635, &list_63X },
	{ 0x637, &list_63X },
	{ 0x638, &list_63X },
	{ 0x639, &list_63X },
	{ 0x660, &list_660 },
};

#define PATCHES_N (sizeof(patch_list) / sizeof(Patches))

#endif

