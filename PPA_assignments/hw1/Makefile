MPL=mpl

main: *.sml *.mlb
	$(MPL) -default-type int64 -default-type word64 main.mlb

test-ext: *.sml *.mlb
	$(MPL) -default-type int64 -default-type word64 test-ext.mlb

lib:
	smlpkg sync

hw1.tgz: lib
	tar czf hw1.tgz -C .. hw1/lib hw1/lib-local hw1/ChessBoard.sml hw1/NQueens.sml hw1/NQueensExt.sml hw1/main.mlb hw1/main.sml hw1/test-ext.mlb hw1/test-ext.sml hw1/sml.pkg hw1/Makefile hw1/README.md hw1/3queens-tree.png
