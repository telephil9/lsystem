#include <u.h>
#include <libc.h>

void*
emalloc(ulong size)
{
	void *p;

	p = mallocz(size, 1);
	if(p==nil)
		sysfatal("emalloc: %r");
	return p;
}
