MXX = mxx
MXVMC = mxvmc

UNITS = BlockTypes PieceUnit GridUnit GameUI
PROGRAM = MasterPiece

UNIT_MXVM = $(addsuffix .mxvm, $(UNITS))
PROG_MXVM = $(PROGRAM).mxvm

all: $(UNIT_MXVM) $(PROG_MXVM)

BlockTypes.mxvm: BlockTypes.pas
	$(MXX) BlockTypes.pas BlockTypes.mxvm

PieceUnit.mxvm: PieceUnit.pas BlockTypes.mxvm
	$(MXX) PieceUnit.pas PieceUnit.mxvm

GridUnit.mxvm: GridUnit.pas BlockTypes.mxvm PieceUnit.mxvm
	$(MXX) GridUnit.pas GridUnit.mxvm

GameUI.mxvm: GameUI.pas BlockTypes.mxvm PieceUnit.mxvm GridUnit.mxvm
	$(MXX) GameUI.pas GameUI.mxvm

MasterPiece.mxvm: MasterPiece.pas BlockTypes.mxvm PieceUnit.mxvm GridUnit.mxvm GameUI.mxvm
	$(MXX) MasterPiece.pas MasterPiece.mxvm

run: all
	$(MXVMC) $(PROG_MXVM)

compile: all
	LDFLAGS="-lSDL2 -lSDL2_ttf" mxvmc $(PROG_MXVM) --action compile

clean:
	rm -f $(UNIT_MXVM) $(PROG_MXVM) *.s *.o $(PROGRAM)

.PHONY: all clean run compile
