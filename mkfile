</$objtype/mkfile

BIN=/$objtype/bin
TARG=slug
LIB=lua/liblua.a.$O
HFILES=
OFILES=slug.$O

</sys/src/cmd/mkone

CFLAGS=-FTVw -p -Ilua/shim -Ilua -DLUA_USE_PLAN9

$LIB:V:
	@{cd lua; mk}

clean nuke:V:
	@{ cd lua; mk $target }
	rm -f *.[$OS] [$OS].out $TARG
