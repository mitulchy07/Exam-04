#include "argo.h"

/* Safe peek: never ungetc EOF */
int peek(FILE *stream)
{
	int c;

	c = getc(stream);
	if (c != EOF)
	{
		ungetc(c, stream);
	}
	return (c);
}

int accept(FILE *stream, char c)
{
	if (peek(stream) == c)
	{
		(void)getc(stream);
		return (1);
	}
	return (0);
}

/* Exact error strings required by the subject */
void unexpected(FILE *stream)
{
	int c;

	c = peek(stream);
	if (c == EOF)
	{
		printf("Unexpected end of input\n");
	}
	else
	{
		printf("Unexpected token '%c'\n", c);
	}
}

int expect(FILE *stream, char c)
{
	if (accept(stream, c))
	{
		return (1);
	}
	unexpected(stream);
	return (0);
}

/* Free recursively; no for/++ */
void free_json(json j)
{
	size_t i;

	if (j.type == MAP)
	{
		i = 0;
		while (i < j.map.size)
		{
			free(j.map.data[i].key);
			free_json(j.map.data[i].value);
			i = i + 1;
		}
		free(j.map.data);
	}
	else if (j.type == STRING)
	{
		free(j.string);
	}
}
