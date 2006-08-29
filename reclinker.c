#include "reclinker.h"

int (*preselfcall)();
void (*postselfcall)();

void (*wantmydose)(struct pathnode *root);

struct myarray *prefix;
char *prefixorig = NULL;
char *whereabs;
char *fromabs;

int forceproperprefix = 0;

#define checkUG(stru_stat)											\
	if ((Uid != -1 && Uid != stru_stat.st_uid) ||								\
	   (Gid != -1 && Gid != stru_stat.st_gid))								\
	{													\
		if (verbosity >= 2)										\
			printf("%s%s skipped because of non-matching uid/gid\n",fromorep,where->strmid);	\
		return 0;											\
	}

int
removelink()
{
	struct stat whatstat;
	struct stat anotherstat;
	void removedir();
	
	if (lstat(what, &whatstat) != 0)	
		error("can't stat",fromorep,where->strmid,NULL);

	checkUG(whatstat);
	
	if ((whatstat.st_mode & S_IFMT) == S_IFDIR) {
		if (stat(where->str, &anotherstat) == 0) {	
			if ((anotherstat.st_mode & S_IFMT) == S_IFDIR) 
				return 1;
			else {
				if (verbosity >= 2) {
					printf("%s%s is not a dir...\n",whereorep,where->strmid);
				}
				removedir();
				return 0;
			}
		} else {
			if (verbosity >= 2) {
				printf("%s%s/: can't stat: %s\n",whereorep,where->strmid,strerror(errno));
			}
			return 0; /* We don't descend into "what" in vain */
		}
	}

	if (firstmodeused && mode != (whatstat.st_mode & S_IRWXAUGS)) {
		if (verbosity >= 2)
			printf("%s%s kept because of non-matching mode\n",whereorep,where->strmid);
		return 0;
	}	

	

	if (lstat(where->str, &whatstat) != 0)	{
		if (verbosity >= 2)
			printf("%s%s: can't stat: %s\n",whereorep,where->strmid,strerror(errno));
		return 0;
	}
	
	if ((uid != -1 && uid != whatstat.st_uid) ||
	   (gid != -1 && gid != whatstat.st_gid)) {
		if (verbosity >= 1)
			printf("%s%s kept because of non-matching uid/gid\n",whereorep,where->strmid);
		return 0;
	}


	if (stat(where->str, &anotherstat) != 0)
		/* where->str is a broken symlink */
		goto removal;

	
	if (force) {
	/* what's not a symlink is tried to be removed */
		if ((whatstat.st_mode & S_IFMT) != S_IFLNK)
			goto removal;
	}

	if (! othermodeused)  {	/* othermodeused used to be named onlybroken, 
			     	 * it's just parsimony in option usage
			     	 */  
	/* where->str is removed iff it's a symlink */
		if ((whatstat.st_mode & S_IFMT) == S_IFLNK)
			goto removal;
	}
		
	return 0;

removal:
	if (remove(where->str) == 0) {
		if (verbosity >= 1)
			printf("%s%s removed\n",whereorep,where->strmid);
	} else {
		fprintf(stderr,"%s%s: ", whereorep,where->strmid);
		perror("remove failed");
		program_retval = SMALLPROB;
	}
	
	return 0;
}	
		
			
void
removedir()
{
	struct stat whatstat;
	
	if (onlydir)
		return;

	if (lstat(where->str, &whatstat) != 0) {
		if (verbosity >= 2)
			printf("%s%s: can't stat: %s\n",whereorep,where->strmid,strerror(errno));
		return;
	}

	if ((uid != -1 && uid != whatstat.st_uid) ||
	   (gid != -1 && gid != whatstat.st_gid)) {
		return;
	}	

	if (remove(where->str) == 0) {
		if (verbosity >= 1)
			printf("%s%s/ removed\n",whereorep,where->strmid);
	} else if (verbosity >= 2) {
		printf("%s%s/: remove failed: %s\n", whereorep,where->strmid,strerror(errno));
	}
}	


#define dirsuck() {						\
	fprintf(stderr,"%s%s/: ", whereorep,where->strmid);	\
  	perror("dir cannot be created");			\
	program_retval = SMALLPROB;				\
}

/* The implementation of proceeding a file (if it's a dir, the
 * counterpart is created on the other side, else it's symlinked to the
 * other side)
 */

int
linker()
{

	struct stat whatstat;
	/* char *to_linkto; */     
	int cdrv = 0;
	
	if (lstat(what, &whatstat) != 0)	
		error("can't stat",fromorep,where->strmid,NULL);		
	
	checkUG(whatstat);
	
	if (! firstmodeused)
		mode = (whatstat.st_mode & S_IRWXAUGS); 
	
	if ((whatstat.st_mode & S_IFMT) == S_IFDIR) {
		cdrv = createdir();
		if (cdrv == -2) {
			if (!force) {
				dirsuck();
				return 0;
			}
		}
		else
			return 1;
	}	

	if (force) {
		if (remove(where->str) != 0 && errno != ENOENT) {
			/* Maybe this block should be replaced with the invocation of a more
			 * general error fuction than the actual one ?? 
			 */
			program_retval = SMALLPROB;
			fprintf(stderr,"%s%s: ",whereorep,where->strmid);
			perror("forcing creation failed");
		}
		if (cdrv == -2) {
		 	if (createdir() == -2) {
				dirsuck();
				return 0;
			}		
			else
				return 1;
		}
	}

	if (onlydir) return 0; 

	if (symlink(prefix->str,where->str) == 0) {
		if (verbosity >= 1) {
			printf("%s%s -> %s",whereorep,where->strmid,prefix->str);
			if (verbosity >= 2)
				printf(": symlink created");
			putchar('\n');
		}

		chowner();
	} else {              
		program_retval = SMALLPROB;
		fprintf(stderr,"%s%s: ",whereorep,where->strmid);
		perror("the symlink cannot be created");
	}
	
	return 0;
}

void
modeforcer()
{
	if (! othermodeused)
		return; 

	if (chmod(where->str,mode_forced) == 0) {
		if (verbosity >= 2) 
 			printf("%s%s/: mode set to %o\n",whereorep,where->strmid, mode_forced);
	} else {	
		program_retval = SMALLPROB;
 		fprintf(stderr,"%s%s/: ",whereorep,where->strmid); 
 		perror("setting mode failed");
	}
}
	
int
test()
{
        struct stat whatstat;
        struct stat wherestat;
	int retval;
	char *typeind;
	int stat_retval;
	
	/* First we stat the actually processed entity, what.
	   It's *very* strange if this fails, so then we go error */
	if (lstat(what, &whatstat) != 0)	
		error("can't stat",fromorep,where->strmid,NULL);
	
	checkUG(whatstat);
		
	if ((whatstat.st_mode & S_IFMT) == S_IFDIR) {
	  /* Here we'll stat where->str; 
	     if what was a dir, then we need it resolved
	     otherwise as is */
		retval = 1;
		typeind = "/";
		stat_retval = stat(where->str,&wherestat);	
	} else {
		retval = 0;
		typeind = "";
		stat_retval = lstat(where->str,&wherestat);	
	}
	
	if (stat_retval != 0) {
		printf("%s%s%s: can't stat: %s\n",whereorep,where->strmid,typeind,strerror(errno));
		program_retval = SMALLPROB;
		return 0; /* We don't descend into what in vain */	
	} else if ((firstmodeused && mode != (whatstat.st_mode & S_IRWXAUGS)) ||
		   (uid != -1 && uid != wherestat.st_uid) ||
		   (gid != -1 && gid != wherestat.st_gid)) {
		if (retval == 0) {
			printf("%s%s%s: permissions, owner or group is not as it's required\n",whereorep,where->strmid,typeind);
			program_retval = SMALLPROB;
		}
		return retval;
	}

	if (retval) {
		/* "what" is a dir */
		if ((wherestat.st_mode & S_IFMT) == S_IFDIR) {
			if (verbosity >= 2)
				printf("%s%s/ is OK\n",whereorep,where->strmid);
		} else {
			printf("%s%s/ exists but it's not a dir\n",whereorep,where->strmid);
			program_retval = SMALLPROB;
			retval = 0; /* don't descend into "what" in vain */
		}
	}
	else if ((wherestat.st_mode & S_IFMT) != S_IFLNK) {
		program_retval = SMALLPROB;
		printf("%s%s exists but it's not a symlink\n",whereorep,where->strmid);
		return retval;
	} else if (! rec_pointto(where->str,what)) {
		program_retval = SMALLPROB;
		printf("%s%s is an existing symlink but it's not resolved in %s%s\n",whereorep,where->strmid,fromorep,where->strmid);
		return retval;
	} else {	
		char *l, *f;
		int v;
		l = canon_readlink(where->str);
		f = soft_realpath(what);
		v = strcmp(l,f);
		free(l);
		free(f);
		if (v != 0) {
			program_retval = SMALLPROB;
			printf("%s%s is a symlink resolved in %s%s, but it's pointing there only indirectly\n",whereorep,where->strmid,fromorep,where->strmid);
			return retval;
		} else {
			f = my_readlink(where->str);
			v = strcmp(f,prefix->str);
			free(f);
			if (v != 0) {
				program_retval = SMALLPROB;
				printf("%s%s is a symlink pointing to where it should but the value is not named as it should be\n",whereorep,where->strmid);
			} else if (verbosity >= 2) {	
				printf("%s%s is OK\n",whereorep,where->strmid);
			}
		}
	}
	return retval;
}
					
			

void
noop()
{
}

/* The following two functions can serve as "wantmydose" */
void
fetchdir(struct pathnode *root)
{
	DIR *dir;
	struct dirent *entry;
	struct pathnode *pn;
	dir = opendir(".");
	pn = root;
	while ((entry = readdir(dir))) {
		addnode(pn,entry->d_name);
	}
	if (closedir(dir) != 0)                                                  
		error((char *)GETCWD(NULL,0),"unable to close dir");
}

void
nextinpath(struct pathnode *root)
{
	if (indivfile->next != NULL) {
		indivfile = indivfile->next;
		addnode(root,indivfile->name);
	}
}


#define backabit(arr,n)		\
{				\
	arr->strend -= n;	\
	*(arr->strend) = '\0';	\
}

#define forthabit(arr,n) arr->str += n

struct pathstate {
	struct myarray *prefix;
	struct pathnode *root;
	struct pathnode *pn;
	int postgrowth;
};


/*
 * A recursive function to walk throgh all the source dir tree and
 * calling linker on the certain files
 */
void
reclinker()
{
	/*
	 * State information which is kept around over recursive calls
	 * is quarantined in pstate
	 */
	struct pathstate pstate;
	memset(&pstate, 0, sizeof(pstate));

	initpath(pstate.root);
	wantmydose(pstate.root);
	pstate.pn = pstate.root;

	while ((pstate.pn = pstate.pn->next)) {
		int prune = 0;
		char *aux, *realwhere = NULL;

		what = pstate.pn->name;
		if (strcmp(what,".") == 0 || strcmp(what,"..") == 0)
			continue;

		pstate.postgrowth = strlen(what) + 1;
		appendasadir(where,what);
		if (deletemode != 1)
			appendasadir(prefix,what);

		/*
		 * We don't descend into directories which reside inside
		 * the mirrored tree
		 */
		realwhere = my_realpath(where->str);
		if (realwhere)
			aux = strreduce(realwhere, fromabs);
		if (aux != NULL && (*aux == '\0' || *aux == '/')) {
			struct stat astat;

			stat(realwhere, &astat);
			if ((astat.st_mode & S_IFMT) == S_IFDIR) {
				prune = 1;
				if (verbosity >= 2)
					printf("pruning %s%s as it's contained in original copy\n", whereorep, where->strmid);
			}
		}
		free(aux);

		if (! prune && preselfcall()) {
		/* preselfcall is a function pointer, may point to
		 * linker, removelink or test */
			if (chdir(what) == 0) {
				if (rel) {
						struct stat whatstat;
						lstat(where->str,&whatstat);
						if (forceproperprefix || (whatstat.st_mode & S_IFMT) == S_IFLNK) {
							char *abspref, *pathbetween;

							abspref = (char *)MALLOC(strlen(whereabs) + strlen(prefixorig) + strlen(where->strmid) + 2);
							strcpy(abspref,whereabs);
							strcat(abspref,"/");
							strcat(abspref,prefixorig);
							strcat(abspref,where->strmid);
							if (!forceproperprefix)
								dupemyarray(prefix, pstate.prefix);

							resetmyarray(prefix);
							pathbetween = str_relpath(realwhere,abspref);
							appendtomyarray(prefix,pathbetween);

							free(abspref);
							free(pathbetween);
						}
						else
							prependtomyarray(prefix,"../");
				}

				free(realwhere);
				reclinker();

				if (chdir("..") == 0)	{
					if (rel) {
						if (pstate.prefix == NULL) {
							forthabit(prefix,3);
						} else {
							freemyarray(prefix);
							prefix = pstate.prefix;
						}
					}
					/* If we want to set the mode of
					 * created dirs arbitrarily, we have to
					 * do it here, after recursive calls of
 					 * reclinker() are over, thus avioding
					 * potential read/write conflicts of
					 * children reclinker()'s
					 */
					postselfcall();
					/* It's a function pointer which may
					 * point to modeforcer, removedir or
					 * noop
					 */
				} else
					error("can't go back to parent dir",what,NULL);
					/*Unlikely error*/
			} else {
				free(realwhere);
				program_retval = SMALLPROB;
				fprintf(stderr,"%s%s: ",fromorep,where->strmid);
				perror("unable to enter dir");
			}
		} else
			free(realwhere);
		backabit(where, pstate.postgrowth);
		if (deletemode != 1)
			backabit(prefix, pstate.postgrowth);
	}
	freepath(pstate.root);
}




int
main(int argc, char **argv)
{
	int o;
	char *aux;
	int auxlength;
	void act_as_reclinker(int argc, char** argv);
	void act_as_recdeleter(int argc, char** argv);	
	void act_as_reclinktester(int argc, char** argv);
	int len;		

	me = (char*)basename(argv[0]);

	defaults();	

	if (strcmp(me,DELETE_INVOKE) == 0) deletemode=1;
	if (strcmp(me,TEST_INVOKE) == 0) deletemode=2;
	
	while ((o = getopt (argc, argv, "hvqdltrfDm:ou:g:U:G:p:")) != -1) {
		switch (o) {
		case 'h':
			usage(stdout);
			showhelp();
			exit(BADOPT);
		case 'v':
			verbosity += 1;
			break;
		case 'q':
			verbosity -= 1;
			break;
		case 'r': 
			rel=1;
			break;
		case 'd':
			deletemode=1;
			break;
		case 'l':
			deletemode=0;
			break;
		case 't':
			deletemode=2;
			break;		
		case 'f':
			force=1;
			break;
		case 'D':
			onlydir=1;
			break;
		case 'm':
			mode = parsemode(optarg);
			firstmodeused = 1;
			break;
		case 'o':
			othermodeused = 1;
			break;
		case 'u':
			usrname = optarg;
			if ((uid = parseuid(optarg)) == -1) {
				fprintf(stderr, "%s: no such user\n", usrname);
				exit(FATAL);
			}
			break;
		case 'g':
			grpname = optarg;
			if ((gid = parsegid(optarg)) == -1) {
				fprintf(stderr, "%s: no such group\n", grpname);
				exit(FATAL);
			}
			break;
		case 'U':
			Usrname = optarg;
			if ((Uid = parseuid(optarg)) == -1) {
				fprintf(stderr, "%s: no such user\n", Usrname);
				exit(FATAL);
			}
			break;
		case 'G':
			Grpname = optarg;
			if ((Gid = parsegid(optarg)) == -1) {
				fprintf(stderr, "%s: no such group\n", Grpname);
				exit(FATAL);
			}
			break;
		case 'p':
			prefixorig = optarg;
			break;	
		case '?':
			badopt;
		}	 	
	}
	
	if (argc < optind + 2) {
		badopt;
	}

	aux = (char*) GETCWD(NULL,0);
	auxlength = strlen(aux) + 1;
	aux = (char *)REALLOC(aux,auxlength);
	strcat(aux,"/");
	
#define makerepstr(wg,wp) {			\
	len = strlen(wg);			\
	len--;					\
	if (*(wg + len) == '/') {		\
		wp = (char *)malloc(len + 1);	\
		memcpy(wp,wg,len);		\
		*(wp + len) = '\0';		\
	} else {				\
		wp = wg;			\
	}					\
}

	whereorig = argv[optind+1];
	makerepstr(whereorig,whereorep);
	initmyarray(where);
	if (*whereorig == '/') {
		appendtomyarray(where,normalize(whereorig));
	} else {
		aux = (char *)REALLOC(aux,auxlength + strlen(whereorig) + 1);
		appendtomyarray(where,normalize(strcat(aux,whereorig)));	
	}
	where->strmid = where->strend;

	fromorig = argv[optind];
	makerepstr(fromorig,fromorep);
	if (*fromorig == '/') {
		from = normalize(fromorig);
	} else {
		aux = (char *)REALLOC(aux,auxlength + strlen(fromorig) + 1);
		*(aux + auxlength) = '\0'; 
		from = normalize(strcat(aux,fromorig));	
	}
	free(aux);
	fromabs = my_realpath(from);

	switch (deletemode) {
	case 0:
		act_as_reclinker(argc,argv);
		break;
	case 1: 
		act_as_recdeleter(argc,argv);
		break;
	case 2:
		act_as_reclinktester(argc,argv);
		break;
	default:
		fprintf(stderr,"%d: no such mode defined\n",deletemode);
		exit(BADOPT);
	}	
	
	if (argc >= optind + 3) {
		int i;
		wantmydose = nextinpath;
		for (i=optind+2;i < argc;i++) {
			if (chdir(from) != 0) {
				fprintf(stderr,"%s/: ",from);
				perror("can't enter source directory");
				exit(FATAL);
			}
			if (strcmp(argv[i],"-") != 0 &&
			   strcmp(argv[i],"0-") != 0) {
				indivfile = strtopath(argv[i]);
				reclinker();
				postselfcall();
			} else {			
				char *line, sepchar;
				switch((*argv[i])) {
				case '-':
					sepchar = '\n';
					break;
				case '0':
					sepchar = '\0';
				}	
				while ((line = get_line_from_file(stdin,sepchar))) {
					indivfile = strtopath(line);
					reclinker();
					postselfcall();
					free(line);
				}
			}
		}
	} else {
		wantmydose = fetchdir; 	
		reclinker();
		postselfcall();
	}

	exit(program_retval);
}
	
#define initprefix {							\
	if (prefixorig == NULL) {					\
		if (rel) {						\
			prefixorig = str_relpath(whereabs,from);	\
		} else							\
			prefixorig = from;				\
	}	 							\
	initmyarray(prefix);						\
	appendtomyarray(prefix,prefixorig);				\
	if (!strsubtest(prefix->str,"../"))				\
		forceproperprefix = 1;					\
}

void
act_as_reclinker(int argc, char** argv)
{
	char* currdir; 
	char* aux = NULL;
	int isnew;

	if (firstmodeused) {
		mode_forced = mode;
		mode = (mode| S_IWUSR | S_IXUSR);
		
		if (mode != mode_forced && (! othermodeused)) {
			fprintf(stderr,
"Prescribed mode given by -m is supported only for modes including\n"
"write/execute permissions for user -- if you *really* want another mode,\n"
"use -o as well\n");
			exit(BADOPT);
		}
	} else { struct stat fromstat;
		if (stat(from,&fromstat) != 0) {
			error("can't stat",fromorig,NULL);
		}
		mode = fromstat.st_mode & S_IRWXAUGS;
	}

	/* The createdir() fucntion uses the value of mask (for reoporting only) */
	mask = umask (0);
	umask (mask);	

	isnew = createdir();
	if (isnew == -2) {
		/* Bah. I couldn't help putting here this block which is taken
		 * from linker() with small modifications. Ugly solution,
		 * anyway. 
		 */
		if (force) {
			if (remove(where->str) != 0 && errno != ENOENT) {
				/* Maybe this block should be replaced with the invocation of a more
				 * general error fuction than the actual one ?? 
				 */
				program_retval = SMALLPROB;
				fprintf(stderr,"%s%s: ",whereorep,where->strmid);
				perror("forcing creation failed");
			} else
				isnew = createdir();
			if (isnew != -2)
				goto goon;
		}
		fprintf(stderr, "creating target directory failed\n");
		exit(FATAL);
	}
goon:
	if (chdir(from) != 0) {
		fprintf(stderr,"%s: ",fromorig);
		perror("can't enter source directory");
		cleanup_aux(isnew);
		exit(FATAL);
	}

	/* The following block is for avoiding infite loops and overwriting 
	 * files in source dir with symlinks
	 */
	whereabs = my_realpath(where->str);
	currdir = (char*) GETCWD(NULL,0);
	if (whereabs)
		aux = strreduce(whereabs, currdir);
	if (aux != NULL && (*aux == '\0' || *aux == '/')) { 
		/* Eh, and if from is subdir of where, ain't that a problem?? */
		fprintf(stderr,"Target dir cannot be subdir of source dir\n");
		/* Problem: "whereorig" is created regardless the above anomaly shows
		 * up; quick'n'dirty solution: if it's newly created, we remove it
		 * immediately
		 */      
		cleanup_aux(isnew);
		exit(BADOPT);
	}
	free(currdir);
	free(aux);
 
	initprefix;
	
	preselfcall = linker;
	postselfcall = modeforcer;
}


void
act_as_recdeleter(int argc, char** argv)
{
	if (chdir(from) != 0) {
		fprintf(stderr,"%s/: ",from);
		perror("can't enter source directory");
		exit(FATAL);
	}

	/* what = from; */
	
	preselfcall = removelink;
	postselfcall = removedir;

	what = my_realpath(from);
	if (preselfcall() == 0)
		exit(SUCCESS);
	free(what);

	rel=0; /*recdeleter makes no use of -r*/
}

void
act_as_reclinktester(int argc, char** argv)
{
	/* struct stat wherestat; */

	if (chdir(from) != 0) {
		fprintf(stderr,"%s/: ",from);
		perror("can't enter source directory");
		exit(FATAL);
	}

	whereabs = my_realpath(where->str);
 
	initprefix;
	
	preselfcall = test;
	postselfcall = noop;

	what = my_realpath(from);
	if (preselfcall() == 0)
		exit(SUCCESS);
	free(what);
}
