//#include "reclinker.h"
#include <pwd.h>
main(int argc, char **argv){ getpwnam(argv[1]); getpwnam(argv[1]); printf("noprob with %s\n",argv[1]) ; exit(0); }
