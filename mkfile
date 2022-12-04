</$objtype/mkfile

BIN=/$objtype/bin
TARG=slug
LIB=lua/liblua.a.$O
HFILES=a.h
OFILES=slug.$O api.$O color.$O pixels.$O
MAN=/sys/man/1

</sys/src/cmd/mkone

CFLAGS=-FTVw -p -Ilua/shim -Ilua -DLUA_USE_PLAN9

$LIB:V:
	@{cd lua; mk}

install:V: man

pull:V:
	@{if(test -d lua){cd lua; git/pull}
	  if not git/clone https://git.sr.ht/~kvik/lu9-lua lua}
	
clean nuke:V:
	@{ cd lua; mk $target }
	rm -f *.[$OS] [$OS].out $TARG
