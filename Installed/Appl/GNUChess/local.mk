GEODE = gnuchess

ENGINE_DIR   = $(ROOT_DIR)/Appl/GNUChess/engine
ENGINE_TOOLS = $(ROOT_DIR)/Appl/GNUChess/tools

CPP_OBJS = \
        attack.obj board.obj eval.obj fen.obj gcbridge.obj hash.obj \
        list.obj material.obj move.obj move_check.obj move_do.obj \
        move_evasion.obj move_gen.obj move_legal.obj option.obj pawn.obj \
        piece.obj pst.obj pv.obj random.obj recog.obj search.obj \
        search_full.obj see.obj sort.obj square.obj trans.obj util.obj \
        value.obj vector.obj

CPP_EOBJS = \
        attack.eobj board.eobj eval.eobj fen.eobj gcbridge.eobj hash.eobj \
        list.eobj material.eobj move.eobj move_check.eobj move_do.eobj \
        move_evasion.eobj move_gen.eobj move_legal.eobj option.eobj pawn.eobj \
        piece.eobj pst.eobj pv.eobj random.eobj recog.eobj search.eobj \
        search_full.eobj see.eobj sort.eobj square.eobj trans.eobj util.eobj \
        value.eobj vector.eobj

#include <$(SYSMAKEFILE)>

.SUFFIXES : .cpp
.PATH.cpp : $(ENGINE_DIR)
.PATH.h   : $(ENGINE_DIR)

ENGINE_HEADERS = \
        attack.h board.h colour.h eval.h fen.h gcbridge.h hash.h list.h \
        material.h move.h move_check.h move_do.h move_evasion.h move_gen.h \
        move_legal.h option.h pawn.h piece.h portlib.h porttypes.h protocol.h \
        pst.h pv.h random.h recog.h search.h search_full.h see.h sort.h \
        square.h trans.h util.h value.h vector.h

$(CPP_OBJS) $(CPP_EOBJS) : $(ENGINE_HEADERS)

.cpp.obj :
	perl "$(ENGINE_TOOLS)/compile_cpp.pl" nc "$(.IMPSRC)" "$(.TARGET)"

.cpp.eobj :
	perl "$(ENGINE_TOOLS)/compile_cpp.pl" ec "$(.IMPSRC)" "$(.TARGET)"

gnuchess-normalize : gnuchess.obj .EXEC
	perl "$(ENGINE_TOOLS)/omf_normalize.pl" gnuchess.obj

gnuchessec-normalize : gnuchess.eobj .EXEC
	perl "$(ENGINE_TOOLS)/omf_normalize.pl" gnuchess.eobj

gnuchess.geo : $(CPP_OBJS) gnuchess-normalize
gnuchessec.geo : $(CPP_EOBJS) gnuchessec-normalize

cpp : $(CPP_OBJS)
cppec : $(CPP_EOBJS)

prebuild : gnuchess-normalize $(CPP_OBJS)
prebuildec : gnuchessec-normalize $(CPP_EOBJS)

gnuchess-local-clean : .EXEC
	perl -e "unlink @ARGV" $(CPP_OBJS) $(CPP_EOBJS)

clean : gnuchess-local-clean
