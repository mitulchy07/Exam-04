#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>

static int plus(const char *s, int *i);

static void unexpected_token(char c)
{
    printf("Unexpected token '%c'\n", c);
    exit(1);
}

static void unexpected_end(void)
{
    printf("Unexpected end of input\n");
    exit(1);
}

static int factor(const char *s, int *i)
{
    int res;

    if (s[*i] == '\0')
        unexpected_end();

    if (s[*i] == '(')
    {
        (*i)++;
        res = plus(s, i);

        if (s[*i] == '\0')
            unexpected_end();
        if (s[*i] != ')')
            unexpected_token(s[*i]);

        (*i)++;
        return res;
    }

    if (isdigit((unsigned char)s[*i]))
        return s[(*i)++] - '0';

    unexpected_token(s[*i]);
    return 0;
}

static int mul(const char *s, int *i)
{
    int res = factor(s, i);

    while (s[*i] == '*')
    {
        (*i)++;
        if (s[*i] == '\0')
            unexpected_end();
        res *= factor(s, i);
    }
    return res;
}

static int plus(const char *s, int *i)
{
    int res = mul(s, i);

    while (s[*i] == '+')
    {
        (*i)++;
        if (s[*i] == '\0')
            unexpected_end();
        res += mul(s, i);
    }
    return res;
}

int main(int ac, char **av)
{
    int i = 0;
    int res;

    if (ac != 2)
        return 1;

    res = plus(av[1], &i);

    if (av[1][i] != '\0')
        unexpected_token(av[1][i]);

    if (printf("%d\n", res) < 0)
        return 1;
    return 0;
}
