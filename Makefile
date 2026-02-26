CC=gcc
CFLAGS0=-g -Wall
CFLAGS=-g -Wall -std=c99
DBGCFLAGS=-g -Wall -DDBG
SPECLIBS=
BZLIBS=-lbz2

# looking for uov? chekc the juegoca repository

EXECUTABLES=gl0 coordrd prd genrd onesrd

gl0: gl0.c
	${CC} ${CFLAGS} -o $@ $^
prd: prd.c
	${CC} ${CFLAGS} -o $@ $^
# using my old matead program
# read coords.
coordrd: coordrd.c
	${CC} ${CFLAGS} -o $@ $^
# print out pbm in a way the eye can manually verify
genrd: genrd.c
	${CC} ${CFLAGS} -o $@ $^
# get the coords of the ones.
onesrd: onesrd.c
	${CC} ${CFLAGS} -o $@ $^

.PHONY: clean

clean:
	rm -f ${EXECUTABLES}
