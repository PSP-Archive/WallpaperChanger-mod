// WallpaperChanger v1.3 by estuibal.
// mod by plum.
// Thanks to estuibal!

// header
#include "wallpaper_changer.h"
#include "resolve.h"

// info
PSP_MODULE_INFO("WallpaperChanger", 0x0007, 1, 3);

// function pointer
void (*vshSettingBackground)(int arg0, int arg1, int arg2);
void (*vshSaveWallpaper)(const char *filepath, void *arg1);
int (*vshPowerCallback)(int arg0, int power_flags, void *arg2);
void *(*scePafOpenImage)(const char *filename, void *arg1, void *arg2, u32 image_offset, u32 image_size);

// global
int prev_number;
int wallpaper_count;
int cur_version;
int invalid_ctrl;
int delay_time;
u32 jump_vshctrl;
clock_t g_time;
STMOD_HANDLER previous;
SceKernelUtilsMt19937Context ctx;

// function
void vshSettingWallpaper(void)
{
	/*
		XMB画面の背景を変更する関数

		3つの引数に特定の値を入れると標準、クラシック、クッキー、壁紙に変更できるみたいです(カスタムテーマは未テスト)
		[テーマ設定]->[背景]から選択している時に表示されるプレビュー画面もこの関数が使われています
	*/

	// PSPの背景を壁紙に変更する
	vshSettingBackground(1, 1, -1);
	return;
}

void vshSaveWallpaperPatched(const char *savepath, void *arg1)
{
	/*
		壁紙をflash1に保存する関数
	*/

	char path[256];

	// flash1に壁紙が保存されてない場合は念の為に壁紙を保存しておく
	if(CheckFile(savepath) < 0)
		vshSaveWallpaper(savepath, arg1);

	// パスを生成
	GetSaveWallpaperPath(path);

	// 壁紙を保存する
	vshSaveWallpaper(path, arg1);

	// ファイル名を小文字に修正
	sceIoRename(path, path);

	// 壁紙リストの更新
	wallpaper_count = GetCountWallpaper();
	return;
}

int vshPowerCallbackPatched(int arg0, int power_flags, void *arg2)
{
	int res = vshPowerCallback(arg0, power_flags, arg2);

	// タイマーを更新する
	if(power_flags & PSP_POWER_CB_SUSPENDING)
		g_time = sceKernelLibcClock() + delay_time;

	// スリープ復帰時に壁紙を設定する
	else if(power_flags & PSP_POWER_CB_RESUME_COMPLETE)
		vshSettingWallpaper();

	return res;
}

int sceKernelCreateCallbackPatched(const char *name, SceKernelCallbackFunction func, void *arg)
{
	vshPowerCallback = (void *)func;
	return sceKernelCreateCallback(name, vshPowerCallbackPatched, arg);
}

void vshCtrlReadBufferPositivePatched(SceCtrlData *pad_data)
{
	if(invalid_ctrl)
		pad_data->Buttons &= ~KEY_MASK;

	return;
}

void *scePafOpenImagePatched(const char *filename, void *arg1, void *arg2, u32 image_offset, u32 image_size)
{
	/*
		画像ファイルを開く関数

		引数のimage_sizeは0を指定しても自動で取得してくれるみたいです。
		引数は構造体を返してるようですが放置しておくことに…
	*/

	int number;
	char buf[256];

	if(wallpaper_count <= 0)
		return scePafOpenImage(filename, arg1, arg2, image_offset, image_size);

	switch(wallpaper_count)
	{
		case 1:
			GetWallpaperPath(buf, 0);
			break;

		case 2:
			if(prev_number == -1)
			{
				prev_number = sceKernelUtilsMt19937UInt(&ctx) % 2;
			}

			GetWallpaperPath(buf, prev_number);
			prev_number ^= 1;
			break;

		default:
			do
			{
				number = sceKernelUtilsMt19937UInt(&ctx) % wallpaper_count;
			}
			while(number == prev_number);

			GetWallpaperPath(buf, number);
			prev_number = number;
	}

	// 壁紙が変更されたのでタイマーを更新する
	g_time = sceKernelLibcClock() + delay_time;

	return scePafOpenImage(buf, NULL, NULL, 0, 0);
}

int WallpaperTimerThread(SceSize args, void *argp)
{
	u32 value;

	while(1)
	{
		// 指定された時間だけ待機する
		g_time = sceKernelLibcClock() + delay_time;

		while(sceKernelLibcClock() < g_time)
			sceKernelDelayThread(1000);

		// レジストリをチェック
		GetRegistryValue("/CONFIG/SYSTEM/XMB/THEME", "wallpaper_mode", &value);

		if(value == 1)
		{
			// 念の為にボタン操作を無効にする
			invalid_ctrl = 1;
			sceKernelDelayThread(100000);

			vshSettingWallpaper();

			invalid_ctrl = 0;
		}
	}

	return 0;
}

void PatchVshModule(u32 text_addr)
{
	vshSettingBackground = (void *)text_addr + patch_list[cur_version].list->setting_bk_funcaddr;
	vshSaveWallpaper = (void *)text_addr + patch_list[cur_version].list->save_wp_funcaddr;
	scePafOpenImage = (void *)text_addr + patch_list[cur_version].list->open_img_funcaddr;

	/*
		[テーマ設定]->[背景]から背景を変更する際に表示されるプレビューにも
		scePafOpenImagePatchedが反応するのでプレビューを非表示にする
	*/
	_sw(MIPS_NOP, text_addr + patch_list[cur_version].list->disable_preview_addr);

	MAKE_CALL(text_addr + patch_list[cur_version].list->save_wp_addr, vshSaveWallpaperPatched);
	MAKE_CALL(text_addr + patch_list[cur_version].list->open_wp_addr, scePafOpenImagePatched);
	MAKE_CALL(text_addr + patch_list[cur_version].list->vsh_pwcb_addr, sceKernelCreateCallbackPatched);

	jump_vshctrl = text_addr + patch_list[cur_version].list->jump_vshctrl_addr;
	MAKE_STUB(jump_vshctrl - 8, HijackvshCtrlReadBufferPositive);

	ClearCaches();
	return;
}

int OnModuleStart(SceModule2 *module)
{
	if(_strcmp(module->modname, "vsh_module") == 0)
	{
		PatchVshModule(module->text_addr);
	}

	return previous ? previous(module) : 0;
}

int module_start(SceSize args, void *argp)
{
	SceUID thid;
	int i, devkit, thid_flag;

	cur_version = -1;
	devkit = sceKernelDevkitVersion();

	for(i = 0; i < PATCHES_N; i++)
	{
		if(patch_list[i].version == FIRMWARE_TO_FW(devkit))
		{
			cur_version = i;
			break;
		}
	}

	if(cur_version == -1)
		return -1;

	MakeWallpaperPath((char *)argp);
	wallpaper_count = GetCountWallpaper();

	ReadConfig((char *)argp, &thid_flag, &delay_time);

	if(thid_flag)
	{
		thid = sceKernelCreateThread("WallpaperTimerThread", WallpaperTimerThread, 16, 0x1000, 0, NULL);

		if(thid > 0)
			sceKernelStartThread(thid, 0, NULL);
	}

	prev_number = -1;
	sceKernelUtilsMt19937Init(&ctx, sceKernelLibcTime(NULL));

	previous = sctrlHENSetStartModuleHandler(OnModuleStart);
	return 0;
}

int module_stop(SceSize args, void *argp)
{
	return 0;
}

