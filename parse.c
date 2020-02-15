#include <u.h>
#include <libc.h>
#include <ctype.h>
#include <bio.h>
#include "a.h"


Rule* mkrule(char pred, char *succ);
void skipws(Biobuf *bp);
char* next(Biobuf *bp);
char* readstring(Biobuf *bp);
char* readnumber(Biobuf *bp, int real);

enum 
{
	Bufsize = 1024
};

Lsystem*
parse(char *filename)
{
	Lsystem *ls;
	Rule *r;
	Biobuf *bp;
	char *s, c;

	bp = Bopen(filename, OREAD);
	if(bp==nil)
		sysfatal("bopen: %r");
	ls = emalloc(sizeof(Lsystem));
	for(;;){
		skipws(bp);
		s = next(bp);
		if(strlen(s) == 0)
			break;
		if(strcmp(s, "name")==0){
			free(s);
			skipws(bp);
			ls->name = readstring(bp);
		}else if(strcmp(s, "axiom")==0){
			free(s);
			skipws(bp);
			ls->axiom = next(bp);
		}else if(strcmp(s, "rule")==0){
			free(s);
			skipws(bp);
			s = next(bp);
			c = s[0];
			free(s);
			skipws(bp);
			s = next(bp);
			if(strcmp(s, "→")!=0)
				sysfatal("epxected '->' but got '%s'", s);
			free(s);
			skipws(bp);
			s = next(bp);
			r = mkrule(c, s);
			r->next = ls->rules;
			ls->rules = r;
		}else if(strcmp(s, "line-length")==0){
			free(s);
			skipws(bp);
			s = readnumber(bp, 0);
			ls->linelen = atoi(s);
			free(s);
		}else if(strcmp(s, "initial-angle")==0){
			free(s);
			skipws(bp);
			s = readnumber(bp, 1);
			ls->initangle = atof(s);
			free(s);
		}else if(strcmp(s, "left-angle")==0){
			free(s);
			skipws(bp);
			s = readnumber(bp, 1);
			ls->leftangle = atof(s);
			free(s);
		}else if(strcmp(s, "right-angle")==0){
			free(s);
			skipws(bp);
			s = readnumber(bp, 1);
			ls->rightangle = atof(s);
			free(s);
		}else{
			sysfatal("unexpected token '%s'", s);
		}
	}
	if(ls->name==nil)
		sysfatal("missing lsystem name");
	if(ls->axiom==nil)
		sysfatal("no axiom defined");
	if(ls->rules==nil)
		sysfatal("no rules defined");
	return ls;
}

Rule*
mkrule(char pred, char *succ)
{
	Rule *r;

	r = emalloc(sizeof(Rule));
	r->pred = pred;
	r->succ = strdup(succ);
	r->next = nil;
	return r;
}

void
skipws(Biobuf *bp)
{
	char c;

	for(;;){
		c = Bgetc(bp);
		if(!isspace(c)){
			Bungetc(bp);
			return;
		}
	}
}

char*
next(Biobuf *bp)
{
	char buf[Bufsize], c;
	int n;

	n = 0;
	for(;;){
		c = Bgetc(bp);
		if(c==0)
			break;
		if(isspace(c)){
			Bungetc(bp);
			break;
		}
		buf[n++] = c;
	}
	buf[n] = '\0';
	return strdup(buf);
}

char*
readstring(Biobuf *bp)
{
	char buf[Bufsize], c;
	int n;

	n = 0;
	c = Bgetc(bp);
	if(c != '\'')
		sysfatal("expected ' but got %c", c);
	if(c == 0)
		sysfatal("unexpected end of file");
	for(;;){
		c = Bgetc(bp);
		if(c == 0)
			sysfatal("unexpected end of file");
		if(c == '\'')
			break;
		if(!isalnum(c) && c!=' ')
			sysfatal("unexpected character %c in string", c);
		buf[n++] = c;
	}
	buf[n] = '\0';
	return strdup(buf);
}

char*
readnumber(Biobuf *bp, int real)
{
	char buf[Bufsize], c;
	int n, d;

	n = 0;
	d = 0;
	for(;;){
		c = Bgetc(bp);
		if(c == 0)
			sysfatal("unexpected end of file");
		if(isspace(c))
			break;
		if(isdigit(c) 
			|| (n==0 && (c=='-' || c=='+'))
			|| (real && c =='.' && d==0)){
			if(c=='.') d = 1;
			buf[n++] = c;
		}else{
			sysfatal("unexpected %c in number", c);
		}
	}
	buf[n] = '\0';
	return strdup(buf);
}
