#include "argo.h"

static int parse_value(json *dst, FILE *s);
static int parse_integer(json *dst, FILE *s);
static int parse_string(json *dst, FILE *s);
static int parse_map(json *dst, FILE *s);

int argo(json *dst, FILE *s)
{
	int r;

	if (dst == NULL || s == NULL)
		return (-1);
	dst->type = INTEGER;
	dst->integer = 0;
	r = parse_value(dst, s);
	if (r == -1)
		return (-1);
	return (1);
}

static int parse_integer(json *dst, FILE *s)
{
	int  c;
	int  sign;
	long v;

	sign = 1;
	v = 0;
	c = peek(s);
	if (c == '-')
	{
		(void)getc(s);
		sign = -1;
		c = peek(s);
	}
	if (!isdigit(c))
	{
		unexpected(s);
		return (-1);
	}
	while (1)
	{
		c = peek(s);
		if (!isdigit(c))
			break;
		(void)getc(s);
		v = v * 10 + (c - '0');
	}
	dst->type = INTEGER;
	dst->integer = (int)(sign * v);
	return (1);
}

static int parse_string(json *dst, FILE *s)
{
	size_t cap;
	size_t len;
	char  *buf;
	int    c;
	int    e;
	char  *nb;

	if (!expect(s, '"'))
		return (-1);
	cap = 16;
	len = 0;
	buf = (char *)malloc(cap);
	if (buf == NULL)
		return (-1);
	while (1)
	{
		c = getc(s);
		if (c == EOF)
		{
			printf("Unexpected end of input\n");
			free(buf);
			return (-1);
		}
		if (c == '"')
			break;
		if (c == '\\')
		{
			e = getc(s);
			if (e == EOF)
			{
				printf("Unexpected end of input\n");
				free(buf);
				return (-1);
			}
			if (e == '"' || e == '\\')
				c = e;
			else
			{
				if (len + 2 >= cap)
				{
					cap = cap * 2;
					nb = (char *)realloc(buf, cap);
					if (nb == NULL)
					{
						free(buf);
						return (-1);
					}
					buf = nb;
				}
				buf[len] = '\\';
				len = len + 1;
				buf[len] = (char)e;
				len = len + 1;
				continue;
			}
		}
		if (len + 1 >= cap)
		{
			cap = cap * 2;
			nb = (char *)realloc(buf, cap);
			if (nb == NULL)
			{
				free(buf);
				return (-1);
			}
			buf = nb;
		}
		buf[len] = (char)c;
		len = len + 1;
	}
	if (len + 1 > cap)
	{
		nb = (char *)realloc(buf, len + 1);
		if (nb == NULL)
		{
			free(buf);
			return (-1);
		}
		buf = nb;
	}
	buf[len] = '\0';
	dst->type = STRING;
	dst->string = buf;
	return (1);
}

static int parse_map(json *dst, FILE *s)
{
	pair *np;
	pair *cur;
	json  keyj;

	if (!expect(s, '{'))
		return (-1);
	dst->type = MAP;
	dst->map.size = 0;
	dst->map.data = NULL;
	if (accept(s, '}'))
		return (1);
	while (1)
	{
		if (peek(s) != '"')
		{
			unexpected(s);
			return (-1);
		}
		if (parse_string(&keyj, s) == -1)
			return (-1);
		if (!expect(s, ':'))
		{
			free(keyj.string);
			return (-1);
		}
		np = (pair *)realloc(dst->map.data, (dst->map.size + 1) * sizeof(pair));
		if (np == NULL)
		{
			free(keyj.string);
			return (-1);
		}
		dst->map.data = np;
		cur = &dst->map.data[dst->map.size];
		cur->key = keyj.string;
		if (parse_value(&cur->value, s) == -1)
		{
			free(cur->key);
			return (-1);
		}
		dst->map.size = dst->map.size + 1;
		if (accept(s, '}'))
			break;
		if (accept(s, ','))
			continue;
		unexpected(s);
		return (-1);
	}
	return (1);
}

static int parse_value(json *dst, FILE *s)
{
	int c;

	c = peek(s);
	if (c == EOF)
	{
		printf("Unexpected end of input\n");
		return (-1);
	}
	if (c == '"')
		return (parse_string(dst, s));
	if (c == '{')
		return (parse_map(dst, s));
	if (c == '-' || isdigit(c))
		return (parse_integer(dst, s));
	unexpected(s);
	return (-1);
}
