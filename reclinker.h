#include <sys/param.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <stdlib.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <limits.h>
#include <libgen.h>
#include <assert.h>

#include "config.h"

#define SIZE(x) strlen(x)
/* #define S_IRWXAUGS S_IRWXU | S_IRWXG | S_IRWXO | S_ISUID | S_ISGID | S_ISVTX */
#define S_IRWXAUGS ~S_IFMT

#if (defined __GNU_LIBRARY__ || defined __UCLIBC__)
#define GETCWD(x,y) getcwd(x,y)
#else
#define GETCWD(x,y) gnu_getcwd()
char *gnu_getcwd ();
#endif

/* return values */

#define SUCCESS 0
#define FATAL 1
#define SMALLPROB 2
#define BADOPT 3
#define VERSION "0.4"
#ifdef MAXSYMLINKS
	#define MY_MAXSYMLINKS MAXSYMLINKS
#else
	#define MY_MAXSYMLINKS MY_MAXSYMLINKS_DEFAULT
#endif		

#define MALLOC xmalloc
#define REALLOC xrealloc



struct myarray
{
	int length;
	char *array;
	char *str;
	char *strmid;
	char *strend;
};



extern int program_retval;
extern char* from;
extern char* fromorig;
extern char* fromorep;
/* char* where; */
extern struct myarray *where;
extern char * what;
extern char *whereorig;
extern char *whereorep;
extern int verbosity;
extern int force;
extern int rel;
extern mode_t mode;
extern mode_t mode_forced;
extern int onlydir;
extern int deletemode;
extern int firstmodeused;
extern int othermodeused;
extern mode_t mask;
extern int uid, gid;
extern int Uid, Gid;
/* uid_t uid, gid; */
/* uid_t Uid, Gid; */
extern char *usrname;
extern char *grpname;
extern char *Usrname;
extern char *Grpname;
extern char* me;
#ifdef DEBUG
extern long int count;
#endif

char *strreduce(char*, const char*);
int strsubtest(char*, const char*);
void * xmalloc (size_t);
void * xrealloc (char* buf,size_t size);
void error(char*, ...);
/* #define uerror(msg) error(msg,whereorig,where->strmid,NULL) */
void chowner();
int createdir();
void defaults();
/*void modeforcer();*/ /* now it's removed to reclinker.c, no declaration needed in header */
void usage(FILE*);
#define badopt {					\
	fprintf(stderr,"Invalid invocation.\n");	\
	usage(stderr);					\
	fprintf(stderr,"\nLearn more with -h !\n");	\
	exit(BADOPT);					\
}

void showhelp();
gid_t parsegid(char*);
uid_t parseuid(char* usrnam);
mode_t parsemode(char*);
void cleanup_aux(int);
char *my_realpath(char *str);
char * my_readlink (const char *filename);
char *soft_realpath(const char *filename);
char *canon_readlink(const char *filename);
int rec_pointto(char *linkname, char *filename);

/* What we use from pathparse.c */

struct pathnode
{
        char *name;
        struct pathnode *next;
        struct pathnode *prev;
};

extern struct pathnode *indivfile;

struct pathnode *strtopath(char *pathstr);
char *pathtostr(struct pathnode *p);
struct pathnode *relpath(struct pathnode *f, struct pathnode *t);
char *str_relpath(char *f,char *t);
struct pathnode* palloc(void);
void freepath(struct pathnode *p);
/* void printpath(struct pathnode *p); */
#define normalize(p) pathtostr(strtopath(p))
/* #define str_relpath(f,t) pathtostr(relpath(strtopath(f),strtopath(t))->next) */
#define initpath(r) r = (struct pathnode*)palloc(); r->name = strdup(""); r->next = NULL; r->prev = r
#define addnode(p,s) {					\
	p->next = (struct pathnode*)palloc();		\
	(p->next)->prev = p;				\
	p = p->next;					\
	p->next = NULL;					\
	p->name = strdup(s);				\
}

/* What we use from myarray.c */

#define initmyarray(arr) {						\
	arr = (struct myarray*) MALLOC(sizeof (struct myarray));	\
	arr->length = STARTLENGTH;					\
	arr->array = (char *) MALLOC(arr->length);			\
	arr->str = arr->array + arr->length/2;				\
	arr->strmid = arr->str;						\
	arr->strend = arr->str;						\
	*(arr->strend) = '\0';						\
}

int appendtomyarray(struct myarray *arr, char *string);
#define appendasadir(arr, string) {					\
	/*								\
	 * Ensure to have exactly one path separator between the	\
	 * original path and the addendum.				\
	 * If no right side yet for the path, also ensure strmid is	\
	 * set to the separator.					\
	 */								\
 	if (arr->str[0] == '/' && arr->str[1] == '\0') {		\
		assert(arr->strmid == arr->str + 1);			\
		assert(arr->strend == arr->str + 1);			\
		arr->strmid--;						\
	} else if (arr->str[0] == '\0' ||				\
	           arr->str[strlen(arr->str) - 1] != '/')		\
		appendtomyarray(arr,"/");				\
	appendtomyarray(arr,string);					\
}
int prependtomyarray(struct myarray *arr, char *string);
#define dupemyarray(arr,dupe) {						\
	dupe = (struct myarray*) MALLOC(sizeof (struct myarray));	\
	dupe->length = arr->length;					\
	dupe->array = (char *) MALLOC(dupe->length);			\
	dupe->str = dupe->array + (arr->str - arr->array);		\
	strcpy(dupe->str,arr->str);					\
	dupe->strmid = dupe->array + (arr->strmid - arr->array);	\
	dupe->strend = dupe->array + (arr->strend - arr->array);	\
} 
	
#define resetmyarray(arr) {			\
	*(arr->str) = '\0';			\
	arr->strmid = arr->str;			\
	arr->strend = arr->str;			\
}

#define freemyarray(arr) {			\
	free(arr->array);			\
	free(arr);				\
}


/* get_line_from_file.c */

extern int get_line_from_file(FILE *file, char *buf, size_t len, char sepchar);
