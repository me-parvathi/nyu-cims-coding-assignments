MPL=mpl

test: *.sml *.mlb lib
	$(MPL) -default-type int64 -default-type word64 test.mlb

lib:
	smlpkg sync

README.pdf: README.md
	pandoc README.md -V colorlinks=true -V linkcolor=blue -o README.pdf

hw5.tgz: *.sml *.mlb lib README.pdf
	tar czf hw5.tgz -C .. hw5/Makefile hw5/lib hw5/lib-local hw5/TREE_DATA.sml hw5/JUST_JOIN.sml hw5/AVL.sml hw5/TreeFuncs.sml hw5/Splitters.sml hw5/test.mlb hw5/test.sml hw5/README.pdf