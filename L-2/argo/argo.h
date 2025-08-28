#ifndef ARGO_H
#define ARGO_H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

/* AST types */
typedef struct json {
	enum { MAP, INTEGER, STRING } type;
	union {
		struct {
			struct pair *data;
			size_t      size;
		} map;
		int   integer;
		char *string;
	};
} json;

typedef struct pair {
	char *key;
	json  value;
} pair;

/* Provided helpers (implemented in argo_utils.c) */
int   peek(FILE *stream);
int   accept(FILE *stream, char c);
int   expect(FILE *stream, char c);
void  unexpected(FILE *stream);

/* Free the AST (used by main/tests) */
void  free_json(json j);

/* Your parser entry point */
int   argo(json *dst, FILE *stream);

#endif
