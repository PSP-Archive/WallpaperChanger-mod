// WallpaperChanger v1.3 by estuibal.
// mod by plum.
// Thanks to estuibal!

// header
#include "wallpaper_changer.h"

// function
int ReadLine(SceUID fd, char *buf, int len)
{
	char ch;
	int res, cnt, read_flag;

	cnt = 0;
	read_flag = 0;
	len -= 1;

	do
	{
		res = sceIoRead(fd, &ch, 1);

		if(res == 1)
		{
			if(ch == '\r' || ch == '\n')
			{
				read_flag = 1;
				break;
			}

			else if(ch == ' ')
				continue;

			else
			{
				buf[cnt++] = ch;
			}
		}
	}
	while(res == 1 && cnt < len);

	buf[cnt] = '\0';
	return (read_flag != 0) ? 1 : cnt;
}

int GetConfigPath(const char *path, char *buf)
{
	char *ptr;

	_strncpy(buf, (char *)path, 255);
	ptr = _strrchr(buf, '/');

	if(!ptr)
		return -1;

	_strncpy(ptr + 1, "wpc.ini", 255);
	return 0;
}

int ReadConfig(const char *path, int *flag, int *time)
{
	int res;
	SceUID fd;
	char *ptr, buf[256];

	// デフォルトの設定
	*flag = 1;
	*time = 10000000;

	// 設定ファイルのパスを生成
	GetConfigPath(path, buf);

	fd = sceIoOpen(buf, PSP_O_RDONLY, 0);

	if(fd < 0)
		return -1;

	while(1)
	{
		res = ReadLine(fd, buf, 255);

		if(res == 0)
			break;

		else if(buf[0] == '\0' || buf[0] == '#')
			continue;

		else
		{
			ptr = _strrchr(buf, '=');

			if(ptr != NULL)
			{
				if(_strncmp(buf, "[EnableRandomChanger]", 21) == 0)
				{
					*flag = _atoi(ptr + 1);
				}
				else if(_strncmp(buf, "[DelayTime]", 11) == 0)
				{
					*time = _atoi(ptr + 1);
					*time *= 1000000;

					if(*time == 0)
						*time = 10000000;
				}
			}
		}
	}

	sceIoClose(fd);
	return 0;
}

