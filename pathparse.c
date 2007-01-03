#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stddef.h>

#include "reclinker.h"

#define DIRSEP '/'  /* this would make sense if other parts of the prog
		     * also used this notation... 
		     */

struct pathnode *
palloc(void)
{
	return (struct pathnode*) MALLOC(sizeof (struct pathnode));
}

struct pathnode *
freenode(struct pathnode *p)
{
	/* This function may cause segfault if the "prev" tag of p is undefined
	 * However, this won't occur if paths are built properly
	 */
	struct pathnode* q;
	if (p == NULL)
		return NULL;
	q = p->next;
	if ((p->prev) != NULL)
		(p->prev)->next = NULL; 
	free(p->name);
	free(p);
	if (q != NULL)
		q->prev = NULL;
	return q;
}

void
freepath(struct pathnode *p)
{
	while ((p = freenode(p)) != NULL); 
}

/* strtok sucks (doesn't let free its output strings later on)
 * strtok_r in not standard enough to my taste
 * so I implement my own strtok-like macro and it does the job well
 */
#define my_strtok(p,q,w) {					\
	while(*p == DIRSEP) p++;				\
	if(*p == '\0')						\
		w = NULL;					\
	else {							\
		q = p;						\
		while(*q != DIRSEP && *q != '\0') q++;		\
		w = (char *) MALLOC(q - p + 1);			\
		strncpy(w,p,q-p);				\
		*(w+(q-p)) = '\0';				\
		p = q;						\
	}							\
}

struct pathnode *
strtopath(char *pathstr)
{
	struct pathnode *root;
	struct pathnode *p;
	char *w;
	char *aux;	
	
	initpath(root);

	for (p = root; p != NULL; p = p->next) {
		my_strtok(pathstr,aux,w);
		p->next = NULL;
		while (w != NULL && !(strcmp(".",w) && strcmp("..",w)) ) {
			if (strcmp("..",w) == 0) {
				p = p->prev;
				if (p->next != NULL)
					freenode(p->next);
			}
			free(w);
			my_strtok(pathstr,aux,w);
		}
	
		if (w != NULL) {
			p->next = palloc();
			(p->next)->name = w;
			(p->next)->prev = p; 
		}		
	}
	
	return root;
}

void
pathcopy(struct pathnode *f, struct pathnode *t)
{
	struct pathnode *p = palloc();

	if (f == NULL) {
		t->next = NULL;
		return;
	}
	p->prev = t;
	p->next = NULL;
	p->name = strdup(f->name);

	t->next = p;
	
	if ((f->next) != NULL)
		pathcopy(f->next,p);
}
	

struct pathnode *
relpath(struct pathnode *f, struct pathnode *t)
{
	struct pathnode *p = f;
	struct pathnode *q = t;	
	struct pathnode *r;	
	struct pathnode root;	

	/* We deal only with proper path-roots */
	if (f == NULL || t == NULL)
		return NULL;
	
	while (p->next != NULL &&
	       q->next != NULL &&
	       strcmp((p->next)->name, (q->next)->name) == 0) {
		p = p->next;
		q = q->next;
	}

	root.next = NULL;
	r = &root;
	while (p->next != NULL) {
		r->next = palloc();
		(r->next)->prev = r;
		r = r->next;
		r->name = strdup("..");
		r->next = NULL;
		p = p->next;
	}
	
	pathcopy(q->next,r);
	if (! root.next) {
		/*
		 * The two paths are the same, the relative path in between
		 * is NULL.
		 * We rather add a simple dot component and refrain from using
		 * NULL paths.
		 */
		r = palloc();
		root.next = r;
		r->name = strdup(".");
		r->next = NULL;
	}
	root.next->prev = NULL;
	return root.next;
}

char *
pathtostr(struct pathnode *p)
/* Based on the (better) concat() sample function of the GNU libc manual */
{
	size_t allocated = STARTLENGTH;
	char *result = (char *) MALLOC (allocated);
	char *wp;
	char *newp;

	assert(p);

	wp = result;
	for (; p != NULL; p = p->next) {
		size_t len = strlen (p->name);
		/* Resize the allocated memory if necessary.  */
		if (wp + len + 2 > result + allocated) {
			allocated = (allocated + len) * 2;
			newp = (char *)REALLOC(result, allocated);
			wp = newp + (wp - result);
			result = newp;
		}
		wp = memcpy (wp, p->name, len) + len; /* + 1; */
		*wp++ = DIRSEP;
	}
	/*
         * strip trailing slash unless we're at root (ie., whole pathname is
         * nothing but a slash).
         */
	if (--wp == result)
		*(++wp) = '\0';
	else
		*wp = '\0';
	/* Resize memory to the optimal size.  */
	return (char *)REALLOC(result, wp - result + 1);
}
	
char *
str_relpath(char *f,char *t)
/* This function is written in order to provide proper freeing of the 
 * auxiliary chains of structures
*/
{
	struct pathnode *p;
	struct pathnode *r;
	struct pathnode *q;
	char *w;

	p = strtopath(f);
	q = strtopath(t);
	r = relpath(strtopath(f),strtopath(t));

	w = pathtostr(r);

	freepath(p);
	freepath(q);
	freepath(r);

	return w;
}
