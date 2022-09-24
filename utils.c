// WallpaperChanger v1.3 by estuibal.
// mod by plum.
// Thanks to estuibal!

// header
#include "wallpaper_changer.h"

// global
int first_log;
int save_count;
char load_path[256];
char wallpaper_list[100];

// extern
extern int wallpaper_count;

// function
void ClearCaches(void)
{
	sceKernelIcacheInvalidateAll();
	sceKernelDcacheWritebackInvalidateAll();
}

void GetRegistryValue(const char *dir, const char *name, u32 *value)
{
	SceSize size;
	unsigned int type;
	REGHANDLE h, hd, hk;
	struct RegParam reg;

	_memset(&reg, 0, sizeof(struct RegParam));

	reg.regtype = 1;
	reg.unk2 = 1;
	reg.unk3 = 1;

	reg.namelen = _strlen("/system");
	_strcpy(reg.name, "/system");

	if(!sceRegOpenRegistry(&reg, 2, &h))
	{
		if(!sceRegOpenCategory(h, dir, 2, &hd))
		{
			if(!sceRegGetKeyInfo(hd, name, &hk, &type, &size))
			{
				if(!sceRegGetKeyValue(hd, hk, value, 4))
				{
					sceRegFlushCategory(hd);
				}
			}

			sceRegCloseCategory(hd);
		}

		sceRegFlushRegistry(h);
		sceRegCloseRegistry(h);
	}

	return;
}

int CheckFile(const char *filename)
{
	SceIoStat stat;
	return sceIoGetstat(filename, &stat);
}

int MakeWallpaperPath(const char *path)
{
	char *ptr;

	_strncpy(load_path, (char *)path, 255);
	ptr = _strchr(load_path, '/');

	if(!ptr)
		return -1;

	// xx0:
	*ptr = '\0';

	// xx0:/PICTURE
	_strncat(load_path, "/PICTURE", 255);
	sceIoMkdir(load_path, 0777);

	// xx0:/PICTURE/wallpaper
	_strncat(load_path, "/wallpaper", 255);
	sceIoMkdir(load_path, 0777);

	return 0;
}

int GetCountWallpaper(void)
{
	char *ptr;
	int cnt, res;
	SceUID dfd;
	SceIoDirent dir;

	dfd = sceIoDopen(load_path);

	if(dfd < 0)
		return 0;

	cnt = 0;

	do
	{
		_memset(&dir, 0, sizeof(SceIoDirent));
		res = sceIoDread(dfd, &dir);

		if(res > 0)
		{
			if(!_strcmp(dir.d_name, ".") || !_strcmp(dir.d_name, ".."))
				continue;

			if(_strstr(dir.d_name, ".bmp") != NULL || _strstr(dir.d_name, ".BMP") != NULL)
			{
				ptr = _strrchr(dir.d_name, '.');

				wallpaper_list[cnt] = (char)_atoi(ptr - 3);

				cnt++;
			}
		}
	}
	while(res > 0 && cnt < 100);

	sceIoDclose(dfd);
	return cnt;
}

void GetSaveWallpaperPath(char *buffer)
{
	int i, match;

	for(i = 0, match = 0; i < 100; i++)
	{
		_snprintf(buffer, 255, "%s/wallpaper%03d.bmp", load_path, i);

		if(CheckFile(buffer) < 0)
		{
			match = 1;
			break;
		}
	}

	if(!match)
		_snprintf(buffer, 255, "%s/wallpaper%03d.bmp", load_path, save_count++);

	return;
}

void GetWallpaperPath(char *buf, int number)
{
	_snprintf(buf, 255, "%s/wallpaper%03d.bmp", load_path, (int)wallpaper_list[number]);
	return;
}

int LogPrintf(const char *fmt, ...)
{
	SceUID fd;
	va_list ap;
	int len, size;
	char str[256];

	if(!first_log)
	{
		fd = sceIoOpen("ms0:/log.txt", PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);
		first_log = 1;
	}
	else
	{
		fd = sceIoOpen("ms0:/log.txt", PSP_O_WRONLY | PSP_O_CREAT | PSP_O_APPEND, 0777);
	}

	if(fd < 0)
		return 0;

	va_start(ap, fmt);
	len = _vsnprintf(str, 255, fmt, ap);
	va_end(ap);

	size = sceIoWrite(fd, str, len);
	sceIoClose(fd);

	return size;
}

