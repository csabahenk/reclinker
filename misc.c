#include <stdarg.h>

#include "reclinker.h"

#ifndef GNU 
char *
gnu_getcwd ()
{
	size_t size = 100;
	void * MALLOC (size_t);

	while (1) {
		char *buffer = (char *) MALLOC (size);
		if (getcwd (buffer, size) == buffer)
			return buffer;
		free (buffer);
		if (errno != ERANGE)
			return 0;
		size *= 2;
	}
}
#endif

/* Erases "reduce_with" from "from" if it is an initial segment */
char *
strreduce(char *from, const char *reduce_with)
{
	while(*reduce_with == *from && *reduce_with != '\0') {
		reduce_with++;
		from++;
	}
	if(*reduce_with == '\0')
		return strdup(from);
	return NULL;
}

int
strsubtest(char *from, const char *reduce_with)
{
	while(*reduce_with == *from && *reduce_with != '\0') {
		reduce_with++;
		from++;
	}
	if(*reduce_with == '\0')
		return 1;
	return 0;
}


void *
xmalloc (size_t size)
{
	register void *value = (void*)malloc(size);
	if (value == 0)
		error ("virtual memory exhausted","malloc",NULL);
	return value;
}

void *
xrealloc (char* buf,size_t size)
{
	register void *value = (void*)realloc(buf,size);
	if (value == 0)
		error ("virtual memory exhausted","realloc",NULL);
	return value;
}

void
error(char *msg,...)
{
	va_list vl;
	char *s;

	va_start(vl,msg);
	while ((s = va_arg(vl, char *)))
		fprintf(stderr, "%s", s);
	va_end(vl);

	fprintf(stderr,": ");
	perror(msg);
	exit(FATAL);
}

void
chowner()
{
	if(uid == -1  && gid == -1)
		return;

	if (lchown(where->str,uid,gid) == 0) {
		  /*lchown was not mentioned in glibc docs, but it does exist */
		if (verbosity >= 2) { 
			printf("%s%s: setting ",whereorep,where->strmid);
			if (usrname != NULL)  
				printf("uid to %s ", usrname);
			if (usrname != NULL && grpname != NULL) 
				printf( "and ");
			if (grpname != NULL) 
				printf("gid to %s ", grpname);
			printf("\n");      
		}	
	} else { 
		program_retval = SMALLPROB;
		fprintf(stderr,"%s%s: setting ",whereorep,where->strmid);
		if (usrname != NULL)  
			fprintf(stderr, "uid to %s ", usrname);
		if (usrname != NULL && grpname != NULL) 
			fprintf(stderr, "and ");
		if (grpname != NULL) 
			fprintf(stderr, "gid to %s ", grpname);
		fprintf(stderr,"failed\n");      
	}	
}
     
/* How to proceed a directory (it's put to a separate function as both 
 * main() and linker() uses this)
 */

int
createdir()
{
	int retval = mkdir(where->str, mode);
	int errno_saved = errno;
		
	if(retval == 0)	{  
		if(verbosity >= 1) {
			printf("%s%s/",whereorep,where->strmid);
			if (verbosity >= 2) 
				printf(": dir created with mode %o",
			               (int)(mode & ~mask));
	  	  	putchar('\n');	
		}	

		chowner();
			
	} else if (errno == EEXIST 
#if defined(__NetBSD__) || defined(__FreeBSD__) || defined(__OpenBSD__) || \
    defined(__DragonFly__)
	           /* work around weird BSD errno upon mkdir("/") */
                   || errno == EISDIR
#endif
 	          ) {
		struct stat to_createstat;
		stat(where->str, &to_createstat);
		if ((to_createstat.st_mode & S_IFMT) == S_IFDIR) {
			if(verbosity >= 2) {
				printf("%s%s/: dir already exists\n",whereorep,where->strmid);
			} 
		} else {
		/*	uerror("the dir cannot be created");  */
			errno = errno_saved;
			return -2;
		}
	} else {
	/*	uerror("the dir cannot be created"); */
		errno = errno_saved;
		return -2;
	}
	/* Do we really have to be so strict and raise an error? */
 	return retval;
}  

void
defaults()
{
	program_retval = SUCCESS;
	verbosity=1;
	force=0;
	rel=0;
	/* mode = S_IRWXAUGS; */
	onlydir=0;
	deletemode=0;
	firstmodeused=0;
	othermodeused=0;
	uid = -1;
	gid = -1;
	Uid = -1;
	Gid = -1;
}




void
usage(FILE* desc)
{
	fprintf(desc,
"Recursive linking/deleting utility, version %s. Usage:\n\
\n\
%s [-l|-d|-t] [options] <from> <where> [<file>...]\n",VERSION,me);
}

void
showhelp()
{
	printf(
"\n\
The program has a linker, a deleter and a test mode. By default it's in\n\
linker mode. It gets into deleter mode if it's invoked as %s, and into\n\
test mode if it's invoked as %s. Linker/deleter/test mode may be\n\
forced with the -l/-d/-t switches.\n\
\n\
In linker mode it mirrors the directory tree under <from> to under <where>\n\
and symlinks files there. The symlinks point to the basename of the file\n\
processed, prefixed with <prefix>. If <prefix> is not given, it defaults\n\
to the absolute pathname of <from> (ie., the symlink points to the\n\
correct absolute filename).\n\
\n\
In deleter mode it deletes non-broken symlinks (empty directories) of\n\
the form <where>/foo, where <from>/foo is an existing file (directory).\n\
(A dir is also considered empty if it gets empty during the deletion\n\
procedure.)\n\
\n\
In test mode it tests whether symlinks/dirs under <where> corresponding\n\
to files/dirs under <from> exist, and whether are they created properly.\n\
\n\
If extra arguments are given, then they will be treated as files named\n\
relatively from <from>, and they will be proceeded individually \n\
(no mass action is taken). If \"-\" is among these extra arguments,\n\
then individual file names are also read from stdin; if \"0-\" is among\n\
them, individual file names are also read from stdin, being separated\n\
by  '\\0'.\n\
\n\
Options common for all modes:\n\
\n\
  -h\t\tprint this message and exit\n\
  -U <user>\tskip file under <from> if it's not owned by <user>\n\
  -G <group>\tskip file under <from> if it doesn't belong to <group>\n\
  -v/-q\t\tincrease/decrease verbosity level\n\
  \t\t(currently 0, 1, 2 are in use, default is 1)\n\
\n\
Options in linker mode are:\n\
\n\
  -r\t\tproduce relative symlinks\n\
  -f\t\toverwrite existing files\n\
  -m <mode>\tcreate new directories with mode <mode> ( &'d with umask!)\n\
  -o\t\tforce <mode> given by -m for all processed directories \n\
  \t\t(if -m is not used, the mode of the actually processed\n\
  \t\tdir is forced)\n\
  -u <user>\tnewly created dirs/symlinks shall be owned by <user>\n\
  -g <group>\tnewly created dirs/symlinks shall belong to <group>\n\
  -D\t\tonly directories are proceeded \n\
  -p <prefix>\tprepend <prefix> to link targets \n\
  \n\
Options in deleter mode are:\n\
\n\
  -f\t\tdelete corresponding file even it's not a symlink\n\
  -m <mode>\tdeletes <where>/foo only if <from>/foo is of mode <mode>\n\
  -o\t\tonly broken symlinks are deleted\n\
  -u <user>\tdeletes <where>/foo only if it belongs to <user>\n\
  -g <group>\tdeletes <where>/foo only if it belongs to <group>\n\
  -D\t\tdon't delete directories \n\
 \n\
Options in test mode are:\n\
\n\
  -r\t\ttest symlinks as if they were created using -r in linker mode\n\
  -m <mode>\tskip foo if <from>/foo isn't of mode <mode>\n\
  -u <user>\tskip foo if <where>/foo is not owned by <user>\n\
  -g <group>\tskip foo if <where>/foo doesn't belong to <group>\n\
  -p <prefix>\ttest links assuming they were created using \"-p <prefix>\" \n\
\n\
Return values: 0 - success; 1 - fatal error; 2 - some act failed but\n\
just kept on doing; 3 - arguments imply nothing happens (eg., bad\n\
options used)\n\
",DELETE_INVOKE,TEST_INVOKE);
}



gid_t
parsegid(char* grpnam)
{
	struct group* grp;
	char *p = NULL;
	gid_t _gid_;

	_gid_ = strtoul(grpnam, &p, 10);  /* Is it numeric? */
	if (grpnam == p) {
/* #ifdef __UCLIBC__ 
 that was an oooold uClibc, now I dare to forget about this issue
*/
#if 0
/* It's a damn stupid bug in uClibc (I met with it in 0.9.21): if <group> 
resides on the 0x*d-th row of /etc/group, then calling twice 
getgrnam(<group>) with no intermediate usage of getgrnam results in a segfault
*/
		getgrnam("root");
#endif
		grp = getgrnam(grpnam);
	}
	else
		grp = getgrgid(_gid_);

	if (grp == NULL)
		return -1;
	return grp->gr_gid;  
}

	
uid_t
parseuid(char* usrnam) /* based on Busybox code */
{
	struct passwd* usr;
	char *p = NULL;
	uid_t _uid_; 

	_uid_ = strtoul(usrnam, &p, 10);  /* Is it numeric? */
	if (usrnam == p)
		usr = getpwnam(usrnam);
	else
		usr = getpwuid(_uid_);

	if (usr == NULL)
		return -1;
	return usr->pw_uid;  
}


mode_t
parsemode(char* modestring)
{
	char* modestring_orig = modestring;
	int i=0;

	while (*modestring != '\0') {
		if((i++ > 3) || (*modestring < '0') || (*modestring++ > '7'))
			goto badmode;
	}

	return strtol(optarg,NULL,8);
	
badmode:
	fprintf(stderr,"%s: invalid mode value\n", modestring_orig);
	exit(BADOPT);
}



void
cleanup_aux(int isnew)
{
	if(isnew == 0) {
		int rmsuc = rmdir(where->str);
		if(rmsuc == 0) {
			if(verbosity >=2)
				printf("%s removed\n",whereorig);
		} else {	
			error("cleanup failed",whereorig,NULL);    
		}
	}
}

char *
my_realpath(char *str)
{
	char *aux = (char *)MALLOC(PATH_MAX+1);
	char *aux2 = (char *)realpath(str,aux); /* why do get I warnings if I omit the 
					           (char *) constraint ??? */
	char *aux3;

	if (aux2 == NULL) {
		free(aux);
		return NULL;
	}
	aux3 = strdup(aux2);
	free(aux);

	return aux3;
}

char *
my_readlink (const char *filename)
/* based on sample code of the glibc manual */
{
	int size = STARTLENGTH;
	
	while (1) {
		char *buffer = (char *) MALLOC (size);
		int nchars = readlink (filename, buffer, size);
		if (nchars < 0)
			return NULL;
		if (nchars < size) {
			*(buffer + nchars)  = '\0';
			return buffer;
		}
		free (buffer);
		size *= 2;
	}
}

/* Now comes stuff for deciding whether a symlink points to file. */

char *
soft_realpath(const char *filename)
/* the same as my_realpath, it just gives a symlinkless for broken
 * symlinks as well (symlinkess, except for the symlink proceeded on 
 */
{
	char *fname, *bname, *dname, *rdname, *finalname;
	
	fname = strdup(filename);

	if ((bname = (char *)basename(fname)) == NULL)
		return NULL;
	if ((dname = (char *)dirname(fname)) == NULL)
		return NULL;
	if ((rdname = (char *)my_realpath(dname)) == NULL)
		return NULL;	
	/* free(dname); */
	
	finalname = (char *)MALLOC(strlen(rdname) + strlen(bname) + 2);

	strcpy(finalname,rdname);
	free(rdname);

	strcat(finalname,"/");
	strcat(finalname,bname);
	/* free(bname); */

	return strdup(finalname);
}

				
char *
canon_readlink(const char *filename)
/* gives the soft_realpath of the target of a symlink */
/* freeing should also happen if a we return NULL due to a failed function call,
 * ehh...
 */
{
	char *pretarget, *target, *rtarget;

	if ((pretarget = (char *)my_readlink(filename)) == NULL)
		return NULL;
	if ((*pretarget) == '/') {
		target = pretarget;
	} else {		
		char *rname, *dname;

		if ((rname = (char *)soft_realpath(filename)) == NULL)
			return NULL;
		if ((dname = (char *)dirname(rname)) == NULL)
			return NULL;
		/* free(rname); */
	
		target = (char *)MALLOC(strlen(dname) + strlen(pretarget) + 2);
		strcpy(target,dname);
		/* free(dname);	*/
	
		strcat(target,"/");
		strcat(target,pretarget);
		free(pretarget);
	}

	if ((rtarget = (char *)soft_realpath(target)) == NULL)
		return NULL;
	free(target);
	
	return strdup(rtarget);
}
	
int rec_pointto(char *linkname, char *filename)
{
	char *currpos, *newpos, *rname;
	int i;

	if((currpos = (char *)soft_realpath(linkname)) == NULL ||
	   (rname = (char *)soft_realpath(filename)) == NULL)
		return 0;

	for (i=0; i < MY_MAXSYMLINKS; i++) {
		if (strcmp(currpos,rname) == 0)
			return 1;
		if ((newpos = (char *)canon_readlink(currpos)) == NULL)
			return 0;
		free(currpos);
		currpos = newpos;
	}
	
	errno = ELOOP;
	return 0;
}	

/*
 * Read at most len bytes from file into buf, until we hit EOF or sepchar or 0.
 * Upon hitting EOF or sepchar without error, buf is 0 terminated, and the
 * length of the resulting string (excluding the final zero) is returned.
 *
 * Else -- ie., we got an error, we read 0 when sepchar is not zero,
 * we read len bytes without sepchar or zero -- -1 is returned in
 * order to signal an error.
 */
int
get_line_from_file(FILE *file, char *buf, size_t len, char sepchar)
{
	int i = 0;
	char c;	

	if (len == 0)
		return 0;

	for (; i < len; i++) {
		c = fgetc(file);
		if (c == EOF || c == sepchar) {
			if (ferror(file))
				return -1;
			buf[i] = '\0';
			return i;
		}
		if (c == '\0')
			return -1;
		buf[i] = c;
	}

	return -1;
}
