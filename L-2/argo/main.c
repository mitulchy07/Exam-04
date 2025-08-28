#include "argo.h"

/* Re-emit JSON exactly as parsed; no for/++ */
static void serialize(json j)
{
	if (j.type == INTEGER)
	{
		printf("%d", j.integer);
	}
	else if (j.type == STRING)
	{
		size_t i;

		putchar('"');
		i = 0;
		while (j.string[i] != '\0')
		{
			if (j.string[i] == '\\' || j.string[i] == '"')
			{
				putchar('\\');
			}
			putchar(j.string[i]);
			i = i + 1;
		}
		putchar('"');
	}
	else if (j.type == MAP)
	{
		size_t i;
		json   k;

		putchar('{');
		i = 0;
		while (i < j.map.size)
		{
			if (i != 0)
			{
				putchar(',');
			}
			/* print key as a string value */
			k.type = STRING;
			k.string = j.map.data[i].key;
			serialize(k);
			putchar(':');
			serialize(j.map.data[i].value);
			i = i + 1;
		}
		putchar('}');
	}
}

int main(int ac, char **av)
{
	FILE *stream;
	json  file;

	if (ac != 2)
	{
		return (1);
	}
	stream = fopen(av[1], "r");
	if (stream == NULL)
	{
		return (1);
	}
	/* initialize to a safe default */
	file.type = INTEGER;
	file.integer = 0;

	if (argo(&file, stream) != 1)
	{
		free_json(file);
		fclose(stream);
		return (1);
	}
	serialize(file);
	printf("\n");
	free_json(file);
	fclose(stream);
	return (0);
}
