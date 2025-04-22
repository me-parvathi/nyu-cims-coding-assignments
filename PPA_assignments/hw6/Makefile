main: *.sml *.mlb lib-local/*.sml lib-local/*.mlb
	mpl -default-type int64 -default-type word64 main.mlb

test: *.sml *.mlb lib-local/*.sml lib-local/*.mlb
	mpl -default-type int64 -default-type word64 test.mlb

README.pdf: README.md
	pandoc README.md -V colorlinks=true -V linkcolor=blue -o README.pdf

hw6.tgz:
	tar czf hw6.tgz -C .. hw6/Makefile hw6/lib hw6/lib-local hw6/IntersectionCount.sml hw6/TriangleCount.sml hw6/main.mlb hw6/main.sml hw6/test.mlb hw6/test.sml hw6/ReferenceSolutions.sml hw6/README.pdf hw6/test-graph