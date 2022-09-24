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
		XMB��ʂ̔w�i��ύX����֐�

		3�̈����ɓ���̒l������ƕW���A�N���V�b�N�A�N�b�L�[�A�ǎ��ɕύX�ł���݂����ł�(�J�X�^���e�[�}�͖��e�X�g)
		[�e�[�}�ݒ�]->[�w�i]����I�����Ă��鎞�ɕ\�������v���r���[��ʂ����̊֐����g���Ă��܂�
	*/

	// PSP�̔w�i��ǎ��ɕύX����
	vshSettingBackground(1, 1, -1);
	return;
}

void vshSaveWallpaperPatched(const char *savepath, void *arg1)
{
	/*
		�ǎ���flash1�ɕۑ�����֐�
	*/

	char path[256];

	// flash1�ɕǎ����ۑ�����ĂȂ��ꍇ�͔O�ׂ̈ɕǎ���ۑ����Ă���
	if(CheckFile(savepath) < 0)
		vshSaveWallpaper(savepath, arg1);

	// �p�X�𐶐�
	GetSaveWallpaperPath(path);

	// �ǎ���ۑ�����
	vshSaveWallpaper(path, arg1);

	// �t�@�C�������������ɏC��
	sceIoRename(path, path);

	// �ǎ����X�g�̍X�V
	wallpaper_count = GetCountWallpaper();
	return;
}

int vshPowerCallbackPatched(int arg0, int power_flags, void *arg2)
{
	int res = vshPowerCallback(arg0, power_flags, arg2);

	// �^�C�}�[���X�V����
	if(power_flags & PSP_POWER_CB_SUSPENDING)
		g_time = sceKernelLibcClock() + delay_time;

	// �X���[�v���A���ɕǎ���ݒ肷��
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
		�摜�t�@�C�����J���֐�

		������image_size��0���w�肵�Ă������Ŏ擾���Ă����݂����ł��B
		�����͍\���̂�Ԃ��Ă�悤�ł������u���Ă������ƂɁc
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

	// �ǎ����ύX���ꂽ�̂Ń^�C�}�[���X�V����
	g_time = sceKernelLibcClock() + delay_time;

	return scePafOpenImage(buf, NULL, NULL, 0, 0);
}

int WallpaperTimerThread(SceSize args, void *argp)
{
	u32 value;

	while(1)
	{
		// �w�肳�ꂽ���Ԃ����ҋ@����
		g_time = sceKernelLibcClock() + delay_time;

		while(sceKernelLibcClock() < g_time)
			sceKernelDelayThread(1000);

		// ���W�X�g�����`�F�b�N
		GetRegistryValue("/CONFIG/SYSTEM/XMB/THEME", "wallpaper_mode", &value);

		if(value == 1)
		{
			// �O�ׂ̈Ƀ{�^������𖳌��ɂ���
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
		[�e�[�}�ݒ�]->[�w�i]����w�i��ύX����ۂɕ\�������v���r���[�ɂ�
		scePafOpenImagePatched����������̂Ńv���r���[���\���ɂ���
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

