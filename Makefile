# This is a really dumb makefile. Eventually
# it should be autoconf'ed. However, we support
# only host=mingw, so all we require is that CC
# be set appropriately.

CC = gcc
EXE_EXT = .exe
O = o

all: tee$(EXE_EXT) cleanfile$(EXE_EXT)

tee$(EXE_EXT): tee.$(O)
	$(CC) -s -static -static-libgcc -o $@ $^

cleanfile$(EXE_EXT): cleanfile.$(O)
	$(CC) -s -static -static-libgcc -o $@ $^

.PHONY: srcdist
srcdist:
	-rm -rf srcdir/mingw-get-inst
	mkdir -p srcdir/mingw-get-inst && \
	for f in Makefile RELEASE_NOTES.txt mingw-get-inst.iss \
		tee.c cleanfile.c msys.ico ChangeLog ; do \
		cp $$f srcdir/mingw-get-inst/ ;\
	done && \
	(cd srcdir && tar -cJf ../mingw-get-inst-src.tar.xz mingw-get-inst)

.PHONY: clean
clean:
	-rm -f tee$(EXE_EXT) cleanfile$(EXE_EXT) *.$(O)
