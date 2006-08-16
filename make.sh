#!/bin/sh

OS=${TARGET_OS:=`uname -s`}
MAKE=${MAKE:=make}
M4=${M4:=m4}

$M4 -DOS=$OS Makefile.m4 | $MAKE -f - "$@"
