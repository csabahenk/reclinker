#!/bin/sh -ex

CC=${CC-gcc}

$CC $CFLAGS -I. -Wall -Wstrict-prototypes -Wshadow -mpreferred-stack-boundary=2 -falign-functions=0 -falign-jumps=0 -falign-loops=0 -fomit-frame-pointer -DL_fgetpwent_r -c pwd_grp.c -o fgetpwent_r.o
$CC $CFLAGS -I. -Wall -Wstrict-prototypes -Wshadow -mpreferred-stack-boundary=2 -falign-functions=0 -falign-jumps=0 -falign-loops=0 -fomit-frame-pointer -DL_fgetgrent_r -c pwd_grp.c -o fgetgrent_r.o
$CC $CFLAGS -I. -Wall -Wstrict-prototypes -Wshadow -mpreferred-stack-boundary=2 -falign-functions=0 -falign-jumps=0 -falign-loops=0 -fomit-frame-pointer -DL_fgetpwent -c pwd_grp.c -o fgetpwent.o
$CC $CFLAGS -I. -Wall -Wstrict-prototypes -Wshadow -mpreferred-stack-boundary=2 -falign-functions=0 -falign-jumps=0 -falign-loops=0 -fomit-frame-pointer -DL_fgetgrent -c pwd_grp.c -o fgetgrent.o
$CC $CFLAGS -I. -Wall -Wstrict-prototypes -Wshadow -mpreferred-stack-boundary=2 -falign-functions=0 -falign-jumps=0 -falign-loops=0 -fomit-frame-pointer -DL_getpwnam_r -c pwd_grp.c -o getpwnam_r.o
$CC $CFLAGS -I. -Wall -Wstrict-prototypes -Wshadow -mpreferred-stack-boundary=2 -falign-functions=0 -falign-jumps=0 -falign-loops=0 -fomit-frame-pointer -DL_getgrnam_r -c pwd_grp.c -o getgrnam_r.o
$CC $CFLAGS -I. -Wall -Wstrict-prototypes -Wshadow -mpreferred-stack-boundary=2 -falign-functions=0 -falign-jumps=0 -falign-loops=0 -fomit-frame-pointer -DL_getpwuid_r -c pwd_grp.c -o getpwuid_r.o
$CC $CFLAGS -I. -Wall -Wstrict-prototypes -Wshadow -mpreferred-stack-boundary=2 -falign-functions=0 -falign-jumps=0 -falign-loops=0 -fomit-frame-pointer -DL_getgrgid_r -c pwd_grp.c -o getgrgid_r.o
$CC $CFLAGS -I. -Wall -Wstrict-prototypes -Wshadow -mpreferred-stack-boundary=2 -falign-functions=0 -falign-jumps=0 -falign-loops=0 -fomit-frame-pointer -DL_getpwuid -c pwd_grp.c -o getpwuid.o
$CC $CFLAGS -I. -Wall -Wstrict-prototypes -Wshadow -mpreferred-stack-boundary=2 -falign-functions=0 -falign-jumps=0 -falign-loops=0 -fomit-frame-pointer -DL_getgrgid -c pwd_grp.c -o getgrgid.o
$CC $CFLAGS -I. -Wall -Wstrict-prototypes -Wshadow -mpreferred-stack-boundary=2 -falign-functions=0 -falign-jumps=0 -falign-loops=0 -fomit-frame-pointer -DL_getpwnam -c pwd_grp.c -o getpwnam.o
$CC $CFLAGS -I. -Wall -Wstrict-prototypes -Wshadow -mpreferred-stack-boundary=2 -falign-functions=0 -falign-jumps=0 -falign-loops=0 -fomit-frame-pointer -DL_getgrnam -c pwd_grp.c -o getgrnam.o
$CC $CFLAGS -I. -Wall -Wstrict-prototypes -Wshadow -mpreferred-stack-boundary=2 -falign-functions=0 -falign-jumps=0 -falign-loops=0 -fomit-frame-pointer -DL_getpw -c pwd_grp.c -o getpw.o
$CC $CFLAGS -I. -Wall -Wstrict-prototypes -Wshadow -mpreferred-stack-boundary=2 -falign-functions=0 -falign-jumps=0 -falign-loops=0 -fomit-frame-pointer -DL_getpwent_r -c pwd_grp.c -o getpwent_r.o
$CC $CFLAGS -I. -Wall -Wstrict-prototypes -Wshadow -mpreferred-stack-boundary=2 -falign-functions=0 -falign-jumps=0 -falign-loops=0 -fomit-frame-pointer -DL_getgrent_r -c pwd_grp.c -o getgrent_r.o
$CC $CFLAGS -I. -Wall -Wstrict-prototypes -Wshadow -mpreferred-stack-boundary=2 -falign-functions=0 -falign-jumps=0 -falign-loops=0 -fomit-frame-pointer -DL_getpwent -c pwd_grp.c -o getpwent.o
$CC $CFLAGS -I. -Wall -Wstrict-prototypes -Wshadow -mpreferred-stack-boundary=2 -falign-functions=0 -falign-jumps=0 -falign-loops=0 -fomit-frame-pointer -DL_getgrent -c pwd_grp.c -o getgrent.o
$CC $CFLAGS -I. -Wall -Wstrict-prototypes -Wshadow -mpreferred-stack-boundary=2 -falign-functions=0 -falign-jumps=0 -falign-loops=0 -fomit-frame-pointer -DL_initgroups -c pwd_grp.c -o initgroups.o
$CC $CFLAGS -I. -Wall -Wstrict-prototypes -Wshadow -mpreferred-stack-boundary=2 -falign-functions=0 -falign-jumps=0 -falign-loops=0 -fomit-frame-pointer -DL_putpwent -c pwd_grp.c -o putpwent.o
$CC $CFLAGS -I. -Wall -Wstrict-prototypes -Wshadow -mpreferred-stack-boundary=2 -falign-functions=0 -falign-jumps=0 -falign-loops=0 -fomit-frame-pointer -DL_putgrent -c pwd_grp.c -o putgrent.o
$CC $CFLAGS -I. -Wall -Wstrict-prototypes -Wshadow -mpreferred-stack-boundary=2 -falign-functions=0 -falign-jumps=0 -falign-loops=0 -fomit-frame-pointer -DL___parsepwent -c pwd_grp.c -o __parsepwent.o
$CC $CFLAGS -I. -Wall -Wstrict-prototypes -Wshadow -mpreferred-stack-boundary=2 -falign-functions=0 -falign-jumps=0 -falign-loops=0 -fomit-frame-pointer -DL___parsegrent -c pwd_grp.c -o __parsegrent.o
$CC $CFLAGS -I. -Wall -Wstrict-prototypes -Wshadow -mpreferred-stack-boundary=2 -falign-functions=0 -falign-jumps=0 -falign-loops=0 -fomit-frame-pointer -DL___pgsreader -c pwd_grp.c -o __pgsreader.o
$CC $CFLAGS -I. -Wall -Wstrict-prototypes -Wshadow -mpreferred-stack-boundary=2 -falign-functions=0 -falign-jumps=0 -falign-loops=0 -fomit-frame-pointer -DL_fgetspent_r -c pwd_grp.c -o fgetspent_r.o
$CC $CFLAGS -I. -Wall -Wstrict-prototypes -Wshadow -mpreferred-stack-boundary=2 -falign-functions=0 -falign-jumps=0 -falign-loops=0 -fomit-frame-pointer -DL_fgetspent -c pwd_grp.c -o fgetspent.o
$CC $CFLAGS -I. -Wall -Wstrict-prototypes -Wshadow -mpreferred-stack-boundary=2 -falign-functions=0 -falign-jumps=0 -falign-loops=0 -fomit-frame-pointer -DL_sgetspent_r -c pwd_grp.c -o sgetspent_r.o
$CC $CFLAGS -I. -Wall -Wstrict-prototypes -Wshadow -mpreferred-stack-boundary=2 -falign-functions=0 -falign-jumps=0 -falign-loops=0 -fomit-frame-pointer -DL_getspnam_r -c pwd_grp.c -o getspnam_r.o
$CC $CFLAGS -I. -Wall -Wstrict-prototypes -Wshadow -mpreferred-stack-boundary=2 -falign-functions=0 -falign-jumps=0 -falign-loops=0 -fomit-frame-pointer -DL_getspnam -c pwd_grp.c -o getspnam.o
$CC $CFLAGS -I. -Wall -Wstrict-prototypes -Wshadow -mpreferred-stack-boundary=2 -falign-functions=0 -falign-jumps=0 -falign-loops=0 -fomit-frame-pointer -DL_getspent_r -c pwd_grp.c -o getspent_r.o
$CC $CFLAGS -I. -Wall -Wstrict-prototypes -Wshadow -mpreferred-stack-boundary=2 -falign-functions=0 -falign-jumps=0 -falign-loops=0 -fomit-frame-pointer -DL_getspent -c pwd_grp.c -o getspent.o
$CC $CFLAGS -I. -Wall -Wstrict-prototypes -Wshadow -mpreferred-stack-boundary=2 -falign-functions=0 -falign-jumps=0 -falign-loops=0 -fomit-frame-pointer -DL_sgetspent -c pwd_grp.c -o sgetspent.o
$CC $CFLAGS -I. -Wall -Wstrict-prototypes -Wshadow -mpreferred-stack-boundary=2 -falign-functions=0 -falign-jumps=0 -falign-loops=0 -fomit-frame-pointer -DL_putspent -c pwd_grp.c -o putspent.o
$CC $CFLAGS -I. -Wall -Wstrict-prototypes -Wshadow -mpreferred-stack-boundary=2 -falign-functions=0 -falign-jumps=0 -falign-loops=0 -fomit-frame-pointer -DL___parsespent -c pwd_grp.c -o __parsespent.o
ar -ro libpwdgrp.a fgetpwent_r.o fgetgrent_r.o fgetpwent.o fgetgrent.o getpwnam_r.o getgrnam_r.o getpwuid_r.o getgrgid_r.o getpwuid.o getgrgid.o getpwnam.o getgrnam.o getpw.o getpwent_r.o getgrent_r.o getpwent.o getgrent.o initgroups.o putpwent.o putgrent.o __parsepwent.o __parsegrent.o __pgsreader.o fgetspent_r.o fgetspent.o sgetspent_r.o getspnam_r.o getspnam.o getspent_r.o getspent.o sgetspent.o putspent.o __parsespent.o
