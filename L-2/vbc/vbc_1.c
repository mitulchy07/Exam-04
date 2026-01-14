#include <stdio.h>
#include <ctype.h>

int err(char c)
{
	if (c) 
        printf("Unexpected token '%c'\n", c);
	else   
        printf("Unexpected end of input\n");
	return -1;
}

int plus(const char *s, int *i);

int factor(const char *s, int *i)
{
	int r;

	if (!s[*i]) 
        return err(0);
	if (s[*i] == '(')
	{
		(*i)++;
		r = plus(s, i);
		if (r < 0) 
            return -1;
		if (!s[*i]) 
            return err(0);
		if (s[*i] != ')') 
            return err(s[*i]);
		(*i)++;
		return r;
	}
	if (isdigit((unsigned char)s[*i]))
		return s[(*i)++] - '0';
	return err(s[*i]);
}

int mul(const char *s, int *i)
{
	int r = factor(s, i), t;

	if (r < 0) 
        return -1;
	while (s[*i] == '*')
	{
		(*i)++;
		if (!s[*i]) 
            return err(0);
		t = factor(s, i);
		if (t < 0) 
            return -1;
		r *= t;
	}
	return r;
}

int plus(const char *s, int *i)
{
	int r = mul(s, i), t;

	if (r < 0) 
        return -1;
	while (s[*i] == '+')
	{
		(*i)++;
		if (!s[*i]) 
            return err(0);
		t = mul(s, i);
		if (t < 0) 
            return -1;
		r += t;
	}
	return r;
}

int main(int ac, char **av)
{
	int i = 0, r;

	if (ac != 2) 
        return 1;
	r = plus(av[1], &i);
	if (r < 0) 
        return 1;
	if (av[1][i]) 
    { 
        err(av[1][i]); 
        return 1; 
    }
	if (printf("%d\n", r) < 0) 
        return 1;
	return 0;
}
