# e58554fdd7943fa674ba901e22e9d1a7
# Generated with vfnmkmc by the mc program.
PREFIX=/usr/local
CFLAGS=-O$(O) -std=c++11 -Wl,-u,_WinMain@16#-Wl,--enable-stdcall-fixup 
O=2
# MinGW
# LFLAGS= -l pcre2-8 -l SDL2 -l SDL2main -l opengl32 -l glew32 -mwindows
# cygwin
LFLAGS= -l pcre2-8 -l SDL2 -l SDL2main -l GLEW -l GL -mwindows
OBJS=objs/mechanizm.o objs/options.o objs/Object.o objs/client.o objs/radix.o objs/vectors.o objs/pcre2.o objs/doubles.o objs/system.o objs/utils.o objs/strings.o

.PHONY: all
all: objs client

client:  $(OBJS)
	@ echo "    LINK ./client"
	@ g++ $(OBJS) -o "./client" $(LFLAGS)

objs/mechanizm.o: src/mechanizm.cpp src/mechanizm.hpp src/tools.hpp src/Object.hpp
	@ echo "    CXX  src/mechanizm.cpp"
	@ g++ $(CFLAGS) -c "src/mechanizm.cpp" -o $@
objs/options.o: src/options.cpp src/options.hpp src/tools.hpp
	@ echo "    CXX  src/options.cpp"
	@ g++ $(CFLAGS) -c "src/options.cpp" -o $@
objs/Object.o: src/Object.cpp src/Object.hpp src/tools.hpp
	@ echo "    CXX  src/Object.cpp"
	@ g++ $(CFLAGS) -c "src/Object.cpp" -o $@
objs/client.o: src/client.cpp src/mechanizm.hpp src/tools.hpp src/Object.hpp \
 src/options.hpp
	@ echo "    CXX  src/client.cpp"
	@ g++ $(CFLAGS) -c "src/client.cpp" -o $@
objs/radix.o: src/sorters/radix.cpp src/sorters/../sorters.hpp
	@ echo "    CXX  src/sorters/radix.cpp"
	@ g++ $(CFLAGS) -c "src/sorters/radix.cpp" -o $@
objs/vectors.o: src/tools/vectors.cpp src/tools/../tools.hpp
	@ echo "    CXX  src/tools/vectors.cpp"
	@ g++ $(CFLAGS) -c "src/tools/vectors.cpp" -o $@
objs/pcre2.o: src/tools/pcre2.cpp src/tools/../tools.hpp
	@ echo "    CXX  src/tools/pcre2.cpp"
	@ g++ $(CFLAGS) -c "src/tools/pcre2.cpp" -o $@
objs/doubles.o: src/tools/doubles.cpp src/tools/../tools.hpp
	@ echo "    CXX  src/tools/doubles.cpp"
	@ g++ $(CFLAGS) -c "src/tools/doubles.cpp" -o $@
objs/system.o: src/tools/system.cpp src/tools/../tools.hpp
	@ echo "    CXX  src/tools/system.cpp"
	@ g++ $(CFLAGS) -c "src/tools/system.cpp" -o $@
objs/utils.o: src/tools/utils.cpp src/tools/../sorters.hpp \
 src/tools/../tools.hpp
	@ echo "    CXX  src/tools/utils.cpp"
	@ g++ $(CFLAGS) -c "src/tools/utils.cpp" -o $@
objs/strings.o: src/tools/strings.cpp src/tools/../tools.hpp
	@ echo "    CXX  src/tools/strings.cpp"
	@ g++ $(CFLAGS) -c "src/tools/strings.cpp" -o $@

objs:
	@ mkdir "objs"

.PHONY: c clean
c: clean
clean:
	@ if [ -d "objs" ]; then rm -r "objs"; fi
	@ rm -f "./client"
	@ echo "    CLEAN"
.PHONY: f fresh
f: fresh
fresh: clean
	@ make all --no-print-directory
.PHONY: r run
r: run
run: all
	@ ././client

.PHONY: d debug
d: debug
debug: CFLAGS += -DDEBUG -g3 -Wall -Wextra
debug: O=0
debug: all

.PHONY: sc superclean
sc: superclean
superclean: clean
	@ rm -rf "deps"
	@ echo "    SUPERCLEAN"

.PHONY: install
install: all
	@ install -D -m 755 client $(PREFIX)/bin/client
	@ echo "[1;32m*[0m client installed in $(PREFIX)/bin"
	@ echo "[1muse /`make uninstall/` to remove client[0m"

.PHONY: uninstall
uninstall:
	@ rm $(PREFIX)/bin/client
	@ echo "[1;32m*[0m client removed from $(PREFIX)/bin[0m"

.PHONY: check-syntax
check-syntax:
	g++ $(CFLAGS) -fsyntax-only -Wall -o /dev/null -S src/*
