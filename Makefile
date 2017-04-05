# 974a7f7e2dfedd7b6b8599a2391df6ac
# Generated with vfnmkmc by the mc program.
PREFIX=/usr/local
CFLAGS=-O$(O)  -std=c++11
O=2
LFLAGS= -l pcre++ -l GL -l GLU -l glut
OBJS=objs/options.o objs/Side.o objs/Block.o objs/mechanizm.o objs/Map.o objs/client.o objs/Camera.o objs/Object.o objs/MapSection.o objs/pcre_utils.o objs/utils.o objs/vectors.o objs/doubles.o objs/system.o objs/strings.o objs/radix.o


.PHONY: all
all: objs client

./client:  $(OBJS)
	@ echo "    LINK ./client"
	@ $(CXX) $(OBJS) -o "./client" $(LFLAGS)

objs/options.o: src/options.cpp src/options.hpp src/tools.hpp
	@ echo "    CXX  src/options.cpp"
	@ $(CXX) $(CFLAGS) -c "src/options.cpp" -o $@
objs/Side.o: src/Side.cpp src/Side.hpp
	@ echo "    CXX  src/Side.cpp"
	@ $(CXX) $(CFLAGS) -c "src/Side.cpp" -o $@
objs/Block.o: src/Block.cpp src/Block.hpp
	@ echo "    CXX  src/Block.cpp"
	@ $(CXX) $(CFLAGS) -c "src/Block.cpp" -o $@
objs/mechanizm.o: src/mechanizm.cpp src/mechanizm.hpp src/tools.hpp \
 src/Object.hpp
	@ echo "    CXX  src/mechanizm.cpp"
	@ $(CXX) $(CFLAGS) -c "src/mechanizm.cpp" -o $@
objs/Map.o: src/Map.cpp src/Map.hpp src/tools.hpp src/MapSection.hpp \
 src/Side.hpp src/Block.hpp src/Camera.hpp src/Q.hpp
	@ echo "    CXX  src/Map.cpp"
	@ $(CXX) $(CFLAGS) -c "src/Map.cpp" -o $@
objs/client.o: src/client.cpp src/mechanizm.hpp src/tools.hpp src/Object.hpp \
 src/options.hpp src/Camera.hpp src/Side.hpp src/Block.hpp \
 src/MapSection.hpp src/Map.hpp src/Q.hpp
	@ echo "    CXX  src/client.cpp"
	@ $(CXX) $(CFLAGS) -c "src/client.cpp" -o $@
objs/Camera.o: src/Camera.cpp src/Camera.hpp
	@ echo "    CXX  src/Camera.cpp"
	@ $(CXX) $(CFLAGS) -c "src/Camera.cpp" -o $@
objs/Object.o: src/Object.cpp src/Object.hpp src/tools.hpp
	@ echo "    CXX  src/Object.cpp"
	@ $(CXX) $(CFLAGS) -c "src/Object.cpp" -o $@
objs/MapSection.o: src/MapSection.cpp src/MapSection.hpp src/tools.hpp \
 src/Side.hpp src/Block.hpp src/Camera.hpp
	@ echo "    CXX  src/MapSection.cpp"
	@ $(CXX) $(CFLAGS) -c "src/MapSection.cpp" -o $@
objs/pcre_utils.o: src/tmp_tools/pcre_utils.cpp src/tmp_tools/../tools.hpp
	@ echo "    CXX  src/tmp_tools/pcre_utils.cpp"
	@ $(CXX) $(CFLAGS) -c "src/tmp_tools/pcre_utils.cpp" -o $@
objs/utils.o: src/tmp_tools/utils.cpp src/tmp_tools/../sorters.hpp \
 src/tmp_tools/../tools.hpp
	@ echo "    CXX  src/tmp_tools/utils.cpp"
	@ $(CXX) $(CFLAGS) -c "src/tmp_tools/utils.cpp" -o $@
objs/vectors.o: src/tmp_tools/vectors.cpp src/tmp_tools/../tools.hpp
	@ echo "    CXX  src/tmp_tools/vectors.cpp"
	@ $(CXX) $(CFLAGS) -c "src/tmp_tools/vectors.cpp" -o $@
objs/doubles.o: src/tmp_tools/doubles.cpp src/tmp_tools/../tools.hpp
	@ echo "    CXX  src/tmp_tools/doubles.cpp"
	@ $(CXX) $(CFLAGS) -c "src/tmp_tools/doubles.cpp" -o $@
objs/system.o: src/tmp_tools/system.cpp src/tmp_tools/../tools.hpp
	@ echo "    CXX  src/tmp_tools/system.cpp"
	@ $(CXX) $(CFLAGS) -c "src/tmp_tools/system.cpp" -o $@
objs/strings.o: src/tmp_tools/strings.cpp src/tmp_tools/../tools.hpp
	@ echo "    CXX  src/tmp_tools/strings.cpp"
	@ $(CXX) $(CFLAGS) -c "src/tmp_tools/strings.cpp" -o $@
objs/radix.o: src/sorters/radix.cpp src/sorters/../sorters.hpp
	@ echo "    CXX  src/sorters/radix.cpp"
	@ $(CXX) $(CFLAGS) -c "src/sorters/radix.cpp" -o $@

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
	@ echo "[1muse \`make uninstall\` to remove client[0m"

.PHONY: uninstall
uninstall:
	@ rm $(PREFIX)/bin/client
	@ echo "[1;32m*[0m client removed from $(PREFIX)/bin[0m"

.PHONY: check-syntax
check-syntax:
	$(CXX) $(CFLAGS) -fsyntax-only -Wall -o /dev/null -S src/*
