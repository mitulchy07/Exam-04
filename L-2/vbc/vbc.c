#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

/* --------------------------- GIVEN (unchanged) --------------------------- */

typedef struct node {
	enum {
		ADD,
		MULTI,
		VAL
	}	type;
	int             val;
	struct node    *l;
	struct node    *r;
}	node;

node	*new_node(node n)
{
	node	*ret = calloc(1, sizeof(n));
	if (!ret)
		return (NULL);
	*ret = n;
	return (ret);
}

void	destroy_tree(node *n)
{
	if (!n)
		return ;
	if (n->type != VAL)
	{
		destroy_tree(n->l);
		destroy_tree(n->r);
	}
	free(n);
}

void	unexpected(char c)
{
	if (c)
		printf("Unexpected token '%c'\n", c);
	else
		printf("Unexpected end of file\n");
}

/* NOTE: Given accept/expect are buggy in subject (accept advances unconditionally).
 * We keep them to match the skeleton but DO NOT use them in our parser. */
int	accept(char **s, char c)
{
	if (**s)
	{
		(*s)++;
		return (1);
	}
	return (0);
}

int	expect(char **s, char c)
{
	if (accept(s, c))
		return (1);
	unexpected(**s);
	return (0);
}

/* --------------------------- YOUR IMPLEMENTATION ------------------------- */
/* Safe helpers that handle whitespace and only consume on match. */

static void	skip_ws(char **s)
{
	while (**s == ' ' || **s == '\t' || **s == '\n'
		|| **s == '\r' || **s == '\v' || **s == '\f')
		(*s)++;
}

static int	accept_tok(char **s, char c)
{
	skip_ws(s);
	if (**s == c)
	{
		(*s)++;
		return (1);
	}
	return (0);
}

static int	expect_tok(char **s, char c)
{
	if (accept_tok(s, c))
		return (1);
	skip_ws(s);
	unexpected(**s);
	return (0);
}

static node	*new_val(int v)
{
	node tmp;

	tmp.type = VAL;
	tmp.val = v;
	tmp.l = NULL;
	tmp.r = NULL;
	return (new_node(tmp));
}

static node	*new_binop(int type, node *l, node *r)
{
	node tmp;

	tmp.type = type;
	tmp.val = 0;
	tmp.l = l;
	tmp.r = r;
	return (new_node(tmp));
}

/* Grammar (precedence: * > +)
   expr   := term { '+' term }*
   term   := factor { '*' factor }*
   factor := DIGIT | '(' expr ')'
   Single-digit values only (0..9). */

static node	*parse_expr_r(char **s, int *ok);

static node	*parse_factor(char **s, int *ok)
{
	node	*ret;
	int		v;
	char	*look;

	skip_ws(s);
	if (accept_tok(s, '('))
	{
		ret = parse_expr_r(s, ok);
		if (!*ok)
			return (NULL);
		if (!expect_tok(s, ')'))
		{
			*ok = 0;
			destroy_tree(ret);
			return (NULL);
		}
		return (ret);
	}
	skip_ws(s);
	if (isdigit((unsigned char)**s))
	{
		v = **s - '0';
		(*s)++;
		/* Enforce single-digit only */
		look = *s;
		skip_ws(&look);
		if (isdigit((unsigned char)*look))
		{
			unexpected(*look);
			*ok = 0;
			return (NULL);
		}
		return (new_val(v));
	}
	unexpected(**s);
	*ok = 0;
	return (NULL);
}

static node	*parse_term(char **s, int *ok)
{
	node	*lhs;
	node	*rhs;
	node	*tmp;

	lhs = parse_factor(s, ok);
	if (!*ok)
		return (NULL);
	while (accept_tok(s, '*'))
	{
		rhs = parse_factor(s, ok);
		if (!*ok)
		{
			destroy_tree(lhs);
			return (NULL);
		}
		tmp = new_binop(MULTI, lhs, rhs);
		if (!tmp)
		{
			destroy_tree(lhs);
			destroy_tree(rhs);
			*ok = 0;
			return (NULL);
		}
		lhs = tmp;
	}
	return (lhs);
}

static node	*parse_expr_r(char **s, int *ok)
{
	node	*lhs;
	node	*rhs;
	node	*tmp;

	lhs = parse_term(s, ok);
	if (!*ok)
		return (NULL);
	while (accept_tok(s, '+'))
	{
		rhs = parse_term(s, ok);
		if (!*ok)
		{
			destroy_tree(lhs);
			return (NULL);
		}
		tmp = new_binop(ADD, lhs, rhs);
		if (!tmp)
		{
			destroy_tree(lhs);
			destroy_tree(rhs);
			*ok = 0;
			return (NULL);
		}
		lhs = tmp;
	}
	return (lhs);
}

/* --------------------------- parse_expr (public) -------------------------- */
/* This replaces the //... placeholder in the given code. */

node	*parse_expr(char *s)
{
	node	*ret;
	int		ok;
	char	*p;

	if (!s)
		return (NULL);
	p = s;
	ok = 1;
	ret = parse_expr_r(&p, &ok);
	if (!ok || !ret)
		return (NULL);
	skip_ws(&p);
	if (*p)
	{
		unexpected(*p);
		destroy_tree(ret);
		return (NULL);
	}
	return (ret);
}

/* ---------------------------- GIVEN (unchanged) --------------------------- */

int	eval_tree(node *tree)
{
	switch (tree->type)
	{
		case ADD:
			return (eval_tree(tree->l) + eval_tree(tree->r));
		case MULTI:
			return (eval_tree(tree->l) * eval_tree(tree->r));
		case VAL:
			return (tree->val);
	}
	/* Unreachable, but keep compiler happy if -Wswitch is strict */
	return (0);
}

int	main(int argc, char **argv)
{
	if (argc != 2)
		return (1);
	node *tree = parse_expr(argv[1]);
	if (!tree)
		return (1);
	printf("%d\n", eval_tree(tree));
	destroy_tree(tree);
	return (0);
}
