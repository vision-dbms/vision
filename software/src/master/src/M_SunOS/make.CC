#########################################################
#		Local Compiler Rules			#
#########################################################

#--objectrule--#	CC $(CFLAGS) $(CDEFS) $(CINCS) -o $@ -c 
#--objectsuffix--#	$B
#--targetsuffix--#	$B
#--tarulesuffix--#	$B
#--buildtargets--#	targets

.SUFFIXES :

B	= 
D	= 
CDEFS	= -D_POSIX_C_SOURCE=199506L -D__EXTENSIONS__ -DVisionBuild
CINCS	=
CBASE	= -mt -KPIC -template=no%extdef
CDBG	= -g -xs
CREL	= -O -xvector
CVER	= ${CREL}
C32	= -m32
C64	= -m64 -xmemalign=8i
CARCH	= ${C64}
CFLAGS	= ${CVER} ${CBASE} ${CARCH}

LINKER	= VLINK-CC
LBASE	= -norunpath -z ignore -z lazyload -B direct
LIBASE	= -lrt -lpthread -luuid ${LOCALLIBS} -lsocket -lnsl -Bstatic -lsunmath -Bdynamic -lm
LIBS	= ${LBASE} -R '$$ORIGIN/../lib${D}' -L ../lib${D} ${LIBASE}

release : targets

release64 : targets

release32 : targets-32

debug : targets-dbg

debug64 : targets-dbg

debug32 : targets-dbg32

targets-32:
	@${MAKE} B=-32 D=/32 CARCH='$${C32}' targets

targets-dbg:
	@${MAKE} B=-dbg D=/dbg CVER='$${CDBG}' targets

targets-dbg32:
	@${MAKE} B=-dbg32 D=/dbg32 CVER='$${CDBG}' CARCH='$${C32}' targets

clean-32:
	@${MAKE} B=-32 D=/32 clean
	
clean-dbg:
	@${MAKE} B=-dbg D=/dbg clean

clean-dbg32:
	@${MAKE} B=-dbg32 D=/dbg32 clean

clean-all: clean clean-32 clean-dbg clean-dbg32
