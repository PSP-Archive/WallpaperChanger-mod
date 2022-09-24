// WallpaperChanger v1.3 by estuibal.
// mod by plum.
// Thanks to estuibal!

// header
#include "wallpaper_changer.h"

// function
void *_memset(void *dest, int src, int n)
{
	char *p = (char *)dest;

	for(; n > 0; p++, n--)
	{
		*p = (char)src;
	}

	return dest;
}

void *_memcpy(void *dest, const void *src, int n)
{
	char *p1 = (char *)dest;
	char *p2 = (char *)src;

	for(; n > 0; p1++, p2++, n--)
	{
		*p1 = *p2;
	}

	return dest;
}

int _strlen(const char *s)
{
	char *p = (char *)s;

	for(; *p != '\0'; p++);

	return p - s;
}

int _strcmp(const char *s1, const char *s2)
{
	u8 *p1 = (u8 *)s1;
	u8 *p2 = (u8 *)s2;

	for(; *p1 != '\0'; p1++, p2++)
	{
		if(*p1 != *p2)
			break;
	}

	return *p1 - *p2;
}

int _strncmp(const char *s1, const char *s2, int n)
{
	u8 *p1 = (u8 *)s1;
	u8 *p2 = (u8 *)s2;

	for(; *p1 != '\0' && *p1 == *p2; p1++, p2++)
	{
		n--;

		if(n <= 0)
			break;
	}

	return *p1 - *p2;
}

char *_strcpy(char *s1, const char *s2)
{
	char *p1 = (char *)s1;
	char *p2 = (char *)s2;

	for(; (*p1 = *p2) != '\0'; p1++, p2++);

	return s1;
}

char *_strncpy(char *s1, const char *s2, int n)
{
	char *p1 = (char *)s1;
	char *p2 = (char *)s2;

	for(; (*p1 = *p2) != '\0' && n > 0; p1++, p2++, n--);

	return s1;
}

char *_strcat(char *s1, const char *s2)
{
	char *p1 = (char *)s1;

	while(*p1 != '\0')
		p1++;

	_strcpy(p1, s2);
	return s1;
}

char *_strncat(char *s1, const char *s2, int n)
{
	char *p1 = (char *)s1;

	while(*p1 != '\0')
		p1++;

	_strncpy(p1, s2, n);
	return s1;
}

char *_strchr(const char *s, int c)
{
	char *p = (char *)s;

	for(; *p != '\0'; p++)
	{
		if(*p == c)
			return p;
	}

	return NULL;
}

char *_strrchr(const char *s, int c)
{
	char *p, *b;

	p = (char *)s;
	b = NULL;

	for(; *p != '\0'; p++)
	{
		if(*p == c)
			b = p;
	}

	return b;
}

char *_strstr(const char *s1, const char *s2)
{
	char *p;

	do
	{
		p = _strchr(s1, s2[0]);

		if(p != NULL)
		{
			if(!_strncmp(p, s2, _strlen(s2)))
				return p;
		}
	}
	while(p != NULL);

	return NULL;
}

int _atoi(const char *s)
{
	char *p;
	int m, res;

	m = 0;
	res = 0;

	if(*s == '-')
	{
		s++;
		m = 1;
	}

	for(p = (char *)s; *p != '\0'; p++)
	{
		if(*p >= '0' && *p <= '9')
		{
			res *= 10;
			res += *p - '0';
		}
	}

	return (m == 1) ? (~res) + 1 : res;
}

void insert_char(char *s, int c)
{
	int i = _strlen(s) + 1;

	for(; i > 0; i--)
	{
		s[i] = s[i - 1];
	}

	s[0] = (char)c;
	return;
}

void reverse_str(char *s)
{
	char c;
	int i, e, cnt;

	e = _strlen(s);
	cnt = e / 2;

	for(i = 0; i < cnt; i++)
	{
		c = s[i];
		s[i] = s[e - i - 1];
		s[e - i - 1] = c;
	}

	return;
}

void padding_zero(char *s, int *len, int padding_count)
{
	int i, n;

	for(i = 0; i < padding_count; i++)
	{
		for(n = 0; n < *len + 1; n++)
		{
			s[*len - n + 1] = s[*len - n];
		}

		s[0] = '0';
		*len += 1;
	}

	return;
}

void convert_wide(char *s, int len)
{
	int i, n, v;

	for(i = 0; i < len; i++)
	{
		v = len - i;
		n = v << 1;

		s[n] = s[v];
		s[n + 1] = '\0';
	}

	s[1] = '\0';
	return;
}

int _itoa(int value, char *buf, int base, int padding, int code, int wide)
{
	char *ptr;
	int res, len, m;

	ptr = buf;
	m = 0;

	if(base == 10 && value < 0)
	{
		value = (~value) + 1;
		m = 1;
	}

	do
	{
		res = value % base;
		value = value / base;

		if(res < 10)
		{
			res += '0';
		}
		else
		{
			res += 'a' - 10;

			if(code)
				res -= 0x20;
		}

		*ptr++ = (char)res;
	}
	while(value > 0);

	*ptr = '\0';

	len = ptr - buf;
	res = len;

	reverse_str(buf);

	if(padding > len)
		padding_zero(buf, &res, padding - len);

	if(wide)
		convert_wide(buf, res);

	if(m)
	{
		insert_char(buf, '-');
		res++;
	}

	return res;
}

int _uitoa(u32 value, char *buf, int base, int padding, int code, int wide)
{
	char *ptr;
	int res, len;

	ptr = buf;

	do
	{
		res = value % base;
		value = value / base;

		if(res < 10)
		{
			res += '0';
		}
		else
		{
			res += 'a' - 10;

			if(code)
				res -= 0x20;
		}

		*ptr++ = (char)res;
	}
	while(value > 0);

	*ptr = '\0';

	len = ptr - buf;
	res = len;

	reverse_str(buf);

	if(padding > len)
		padding_zero(buf, &res, padding - len);

	if(wide)
		convert_wide(buf, res);

	return res;
}

int _vsnprintf(char *buf, int n, const char *format, va_list ap)
{
	char *str;
	u32 data, len, i, cnt, code, zero;
	char degit[32];

	if(!format || n <= 0)
	{
		buf[0] = '\0';
		return 0;
	}

	n -= 1;
	cnt = 0;

	while(*format != '\0')
	{
		code = 0;
		zero = 0;

		if(cnt >= n)
			goto VS_EXIT;

		if(*format == '%')
		{
			if(format[1] >= '0' && format[1] <= '9')
			{
				zero += format[1] - '0';
				format++;

				if(format[1] >= '0' && format[1] <= '9')
				{
					zero *= 10;
					zero += format[1] - '0';
					format++;
				}
			}

			switch(*++format)
			{
				case 'C':
					code = 1;
				case 'c':
					data = va_arg(ap, int);

					if(code && data >= 'a' && data <= 'z')
					{
						data -= 0x20;
					}

					buf[cnt++] = data;
					format++;
					break;

				case 'D':
					code = 1;
				case 'd':
					data = va_arg(ap, int);
					len = _itoa(data, degit, 10, zero, code, 0);

					for(i = 0; i < len; i++)
					{
						if(cnt >= n)
							goto VS_EXIT;

						buf[cnt++] = degit[i];
					}

					format++;
					break;

				case 'P':
				case 'X':
					code = 1;
				case 'p':
				case 'x':
					data = va_arg(ap, u32);
					len = _uitoa(data, degit, 16, zero, code, 0);

					for(i = 0; i < len; i++)
					{
						if(cnt >= n)
							goto VS_EXIT;

						buf[cnt++] = degit[i];
					}

					format++;
					break;

				case 'S':
					code = 1;
				case 's':
					str = va_arg(ap, char *);

					if(!str)
						str = "(null)";

					for(; *str != '\0'; str++)
					{
						if(cnt >= n)
							goto VS_EXIT;

						if(code && *str >= 'a' && *str <= 'z')
						{
							buf[cnt++] = *str - 0x20;
						}
						else
						{
							buf[cnt++] = *str;
						}
					}

					format++;
					break;

				case '%':
					buf[cnt++] = '%';
					format++;
					break;

				default:
					buf[cnt++] = *format++;
					break;
			}
		}
		else
		{
			buf[cnt++] = *format++;
		}
	}

VS_EXIT:
	buf[cnt] = '\0';
	return cnt;
}

int _snprintf(char *buf, int n, const char *format, ...)
{
	int len;
	va_list ap;

	va_start(ap, format);
	len = _vsnprintf(buf, n, format, ap);
	va_end(ap);

	return len;
}

/*
int _vswprintf(wchar_t *buf, int n, const wchar_t *format, va_list ap)
{
	wchar_t *str;
	u32 data, len, i, cnt, code, zero;
	wchar_t degit[32];

	if(!format || n <= 0)
	{
		buf[0] = L'\0';
		return 0;
	}

	n -= 1;
	cnt = 0;

	while(*format != L'\0')
	{
		code = 0;
		zero = 0;

		if(cnt >= n)
			goto VW_EXIT;

		if(*format == L'%')
		{
			if(format[1] >= L'0' && format[1] <= L'9')
			{
				zero += format[1] - L'0';
				format++;

				if(format[1] >= L'0' && format[1] <= L'9')
				{
					zero *= 10;
					zero += format[1] - L'0';
					format++;
				}
			}

			switch(*++format)
			{
				case L'C':
					code = 1;
				case L'c':
					data = va_arg(ap, int);

					if(code && data >= L'a' && data <= L'z')
					{
						data -= 0x20;
					}

					buf[cnt++] = data;
					format++;
					break;

				case L'D':
					code = 1;
				case L'd':
					data = va_arg(ap, u32);
					len = _itoa(data, (char *)degit, 10, zero, code, 1);

					for(i = 0; i < len; i++)
					{
						if(cnt >= n)
							goto VW_EXIT;

						buf[cnt++] = degit[i];
					}

					format++;
					break;

				case L'P':
				case L'X':
					code = 1;
				case L'p':
				case L'x':
					data = va_arg(ap, u32);
					len = _uitoa(data, (char *)degit, 16, zero, code, 1);

					for(i = 0; i < len; i++)
					{
						if(cnt >= n)
							goto VW_EXIT;

						buf[cnt++] = degit[i];
					}

					format++;
					break;

				case L'S':
					code = 1;
				case L's':
					str = va_arg(ap, wchar_t *);

					if(!str)
						str = L"(null)";

					for(; *str != L'\0'; str++)
					{
						if(cnt >= n)
							goto VW_EXIT;

						if(code && *str >= L'a' && *str <= L'z')
						{
							buf[cnt++] = *str - 0x20;
						}
						else
						{
							buf[cnt++] = *str;
						}
					}

					format++;
					break;

				case L'%':
					buf[cnt++] = L'%';
					format++;
					break;

				default:
					buf[cnt++] = *format++;
					break;
			}
		}
		else
		{
			buf[cnt++] = *format++;
		}
	}

VW_EXIT:
	buf[cnt] = L'\0';
	return cnt;
}

int _swprintf(wchar_t *buf, int n, const wchar_t *format, ...)
{
	int len;
	va_list ap;

	va_start(ap, format);
	len = _vswprintf(buf, n, format, ap);
	va_end(ap);

	return len;
}
*/

