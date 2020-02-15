typedef struct Lsystem Lsystem;
typedef struct Rule Rule;
typedef struct State State;

struct Lsystem
{
	char*	name;
	char*	axiom;
	Rule*	rules;
	int	linelen;
	double	initangle;
	double	leftangle;
	double	rightangle;
};

struct Rule
{
	char	pred;
	char*	succ;
	Rule*	next;
};

struct State
{
	int	x;
	int	y;
	double	α;
	State*	prev;
};

void* emalloc(ulong size);
Lsystem* parse(char *filename);
