// WallpaperChanger v1.3 by estuibal.
// mod by plum.
// Thanks to estuibal!

#ifndef __WALLPAPER_CHANGER_H__
#define __WALLPAPER_CHANGER_H__

// header
#include <pspkernel.h>
#include <systemctrl.h>
#include <psploadcore.h>
#include <psputilsforkernel.h>
#include <psppower.h>
#include <pspctrl.h>
#include <pspreg.h>
#include <stdarg.h>

// MASK
#define KEY_MASK	(PSP_CTRL_UP | PSP_CTRL_DOWN | PSP_CTRL_LEFT | PSP_CTRL_RIGHT |\
					PSP_CTRL_CIRCLE | PSP_CTRL_CROSS | PSP_CTRL_SQUARE | PSP_CTRL_TRIANGLE |\
					PSP_CTRL_LTRIGGER | PSP_CTRL_RTRIGGER | PSP_CTRL_START | PSP_CTRL_SELECT)

// define
#define MIPS_NOP 0x00000000
#define MAKE_CALL(a, f) _sw(0x0C000000 | (((u32)(f) >> 2) & 0x03FFFFFF), a);

// by Total_Noob
#define FIRMWARE_TO_FW(f) ((((f >> 24) & 0xF) << 8) | (((f >> 16) & 0xF) << 4) | ((f >> 8) & 0xF))
#define MAKE_STUB(a, f) _sw(0x08000000 | (((u32)(f) & 0x0FFFFFFC) >> 2), a); _sw(MIPS_NOP, a + 4);

// extras.S
void HijackvshCtrlReadBufferPositive(void);

// main.c
void vshSettingWallpaper(void);
void vshSaveWallpaperPatched(const char *savepath, void *arg1);
int vshPowerCallbackPatched(int arg0, int power_flags, void *arg2);
int sceKernelCreateCallbackPatched(const char *name, SceKernelCallbackFunction func, void *arg);
void vshCtrlReadBufferPositivePatched(SceCtrlData *pad_data);
void *scePafOpenImagePatched(const char *filename, void *arg1, void *arg2, u32 image_offset, u32 image_size);
int WallpaperTimerThread(SceSize args, void *argp);
void PatchVshModule(u32 text_addr);
int OnModuleStart(SceModule2 *module);
int module_start(SceSize args, void *argp);
int module_stop(SceSize args, void *argp);

// utils.c
void ClearCaches(void);
void GetRegistryValue(const char *dir, const char *name, u32 *value);
int CheckFile(const char *filename);
int MakeWallpaperPath(const char *path);
int GetCountWallpaper(void);
void GetSaveWallpaperPath(char *buffer);
void GetWallpaperPath(char *buf, int number);
int LogPrintf(const char *fmt, ...);

// config.c
int ReadLine(SceUID fd, char *buf, int len);
int GetConfigPath(const char *path, char *buf);
int ReadConfig(const char *path, int *flag, int *time);

// sysclib.c
void *_memset(void *dest, int src, int n);
void *_memcpy(void *dest, const void *src, int n);
int _strlen(const char *s);
int _strcmp(const char *s1, const char *s2);
int _strncmp(const char *s1, const char *s2, int n);
char *_strcpy(char *s1, const char *s2);
char *_strncpy(char *s1, const char *s2, int n);
char *_strcat(char *s1, const char *s2);
char *_strncat(char *s1, const char *s2, int n);
char *_strchr(const char *s, int c);
char *_strrchr(const char *s, int c);
char *_strstr(const char *s1, const char *s2);
int _atoi(const char *s);
int _itoa(int value, char *buf, int base, int padding, int code, int wide);
int _uitoa(u32 value, char *buf, int base, int padding, int code, int wide);
int _vsnprintf(char *buf, int n, const char *format, va_list ap);
int _snprintf(char *buf, int n, const char *format, ...);
//int _vswprintf(wchar_t *buf, int n, const wchar_t *format, va_list ap);
//int _swprintf(wchar_t *buf, int n, const wchar_t *format, ...);

#endif

