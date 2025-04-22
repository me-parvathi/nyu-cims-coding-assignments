MPL=mpl

main: *.sml *.mlb lib
	$(MPL) -default-type int64 -default-type word64 main.mlb

test: *.sml *.mlb lib
	$(MPL) -default-type int64 -default-type word64 test.mlb

lib:
	smlpkg sync

README.pdf: README.md
	pandoc README.md -V colorlinks=true -V linkcolor=blue -o README.pdf

hw2.tgz: README.pdf
	tar czf hw2.tgz -C .. hw2/lib hw2/lib-local hw2/LongestLine.sml hw2/main.mlb hw2/main.sml hw2/test.mlb hw2/test.sml hw2/sml.pkg hw2/Makefile hw2/README.md hw2/README.pdf hw2/longest-line-hint.png hw2/download_words_dataset.sh