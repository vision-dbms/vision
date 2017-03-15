#########################################################
#		Local Compiler Rules			#
#########################################################

#--objectrule--#	$(CXX) $(CFLAGS) $(CDEFS) $(CINCS) -o $@ -c 
#--objectsuffix--#	$B
#--targetsuffix--#	$B
#--tarulesuffix--#	$B
#--buildtargets--#	targets

.SUFFIXES :

B	= 
%-dbg 	: B = -dbg

D	= 
%-dbg 	: D = /dbg

CDEFS	=  -DVisionBuild
CINCS	=
CBASE	= -fexceptions -frtti -Wno-trigraphs -Wno-undefined-bool-conversion -Wno-format -Wno-logical-op-parentheses -Wno-switch -Wno-shift-op-parentheses -Wno-shift-negative-value -Wno-parentheses -Wno-deprecated-declarations
CDBG	= -g -fno-limit-debug-info
CREL	= -O2
CVER	= ${CREL}

CFLAGS	= ${CVER} ${CBASE}

LINKER	= VLINK-CC
LBASE	=
LIBASE	= ${LOCALLIBS} -lpthread
LIBS	= ${LBASE} -L../lib$D ${LIBASE}

release : targets

debug : targets-dbg

targets-dbg :
	@$(MAKE) B=-dbg D=/dbg CVER="${CDBG}" targets

clean-dbg :
	@$(MAKE) B=-dbg D=/dbg clean
	
clean-all : clean clean-dbg
