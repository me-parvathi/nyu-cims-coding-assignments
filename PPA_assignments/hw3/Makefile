MPL=mpl

main: *.sml *.mlb lib
	$(MPL) -default-type int64 -default-type word64 main.mlb

test: *.sml *.mlb lib
	$(MPL) -default-type int64 -default-type word64 test.mlb

lib:
	smlpkg sync

README.pdf: README.md
	pandoc README.md -V colorlinks=true -V linkcolor=blue -o README.pdf

hw3.tgz: README.pdf *.sml *.mlb lib
	tar czf hw3.tgz -C .. hw3/lib hw3/lib-local hw3/test-json hw3/JsonFindStrings.sml hw3/JsonNesting.sml hw3/ReferenceSolutions.sml hw3/RainbowBrackets.sml hw3/main.mlb hw3/main.sml hw3/test.mlb hw3/test.sml hw3/sml.pkg hw3/Makefile hw3/README.md hw3/README.pdf hw3/example-rainbow-brackets.png