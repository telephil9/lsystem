#include <u.h>
#include <libc.h>
#include <String.h>
#include <draw.h>
#include <event.h>
#include <keyboard.h>
#include "a.h"

Lsystem	*ls;
State	*state;
int 	x;
int 	y;
double 	α;
char 	*curgen;

void
pushstate(void)
{
	State *s;

	s = emalloc(sizeof(State));
	s->x = x;
	s->y = y;
	s->α = α;
	s->prev = state;
	state = s;
}

void
popstate(void)
{
	State *s;

	s = state;
	x = s->x;
	y = s->y;
	α = s->α;
	state = state->prev;
	free(s);
}

char*
production(char c)
{
	Rule *r;

	for(r = ls->rules; r!=nil; r=r->next)
		if(r->pred == c)
			return r->succ;
	return nil;
}

void
nextgen(void)
{
	String *s;
	char *p, *q;

	s = s_new();
	for(p = curgen; *p; p++){
		q = production(*p);
		if(q)
			s_append(s, q);
		else
			s_putc(s, *p);
	}
	s_terminate(s);
	free(curgen);
	curgen = strdup(s_to_c(s));
	s_free(s);
}

void
forward(void)
{
	int x1, y1;

	x1 = x + ls->linelen*cos(α * (PI/180.0));
	y1 = y + ls->linelen*sin(α * (PI/180.0));
	line(screen, Pt(x,y), Pt(x1, y1), 1, 1, 0, display->black, ZP);
	x = x1;
	y = y1;
}

void
rotate(double angle)
{
	α += angle;
	if(α >= 360.0)
		α -= 360.0;
	if(α <= 0.0)
		α += 360.0;
}

void
redraw(void)
{
	char *s;

	x = screen->r.min.x + 10;
	y = screen->r.max.y - 10;
	α = ls->initangle;
	draw(screen, screen->r, display->white, nil, ZP);
	string(screen, addpt(screen->r.min, Pt(10, 10)), display->black, ZP, font, ls->name);
	for(s = curgen; *s; s++){
		switch(*s){
		case 'F':
		case 'G':
			forward();
			break;
		case '-':
			rotate(ls->leftangle);
			break;
		case '+':
			rotate(ls->rightangle);
			break;
		case '[':
			pushstate();
			break;
		case ']':
			popstate();
			break;
		}
	}
}

void
eresized(int new)
{
	if(new && getwindow(display, Refnone)<0)
		sysfatal("cannot reattach: %r");
	redraw();
}

void
main(int argc, char *argv[])
{
	Event e;

	if(argc != 2){
		fprint(2, "usage: %s <filename>\n", argv[0]);
		exits("usage");
	}
	ls = parse(argv[1]);
	curgen = strdup(ls->axiom);
	if(initdraw(nil, nil, "lsystem")<0)
		sysfatal("initdraw: %r");
	einit(Ekeyboard|Emouse);
	eresized(0);
	for(;;){
		switch(event(&e)){
		case Ekeyboard:
			if(e.kbdc=='q' || e.kbdc==Kdel)
				exits(nil);
			break;
		case Emouse:
			if(e.mouse.buttons&1){
				nextgen();
				redraw();
			}
			break;
		}
	}
}
