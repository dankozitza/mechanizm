# a8ae93db67fdf82ceff133f01db062b9
# Generated with vfnmkmc by the mc program.
PREFIX=/usr/local
CFLAGS=-O$(O)  -std=c++11
O=2
LFLAGS= -l pcre++ -l GL -l GLU -l glut -l jsoncpp
OBJS=objs/Block.o objs/Camera.o objs/client.o objs/commands.o objs/Map.o objs/MapSection.o objs/mechanizm.o objs/Object.o objs/options.o objs/Side.o objs/Sphere.o objs/radix.o objs/doubles.o objs/json_utils.o objs/pcre_utils.o objs/strings.o objs/system.o objs/utils.o objs/vectors.o


.PHONY: all
all: objs client

./client:  $(OBJS)
	@ echo "    LINK ./client"
	@ $(CXX) $(OBJS) -o "./client" $(LFLAGS)

objs/Block.o: src/Block.cpp src/Block.hpp src/Side.hpp src/tools.hpp
	@ echo "    CXX  src/Block.cpp"
	@ $(CXX) $(CFLAGS) -c "src/Block.cpp" -o $@
objs/Camera.o: src/Camera.cpp src/Camera.hpp
	@ echo "    CXX  src/Camera.cpp"
	@ $(CXX) $(CFLAGS) -c "src/Camera.cpp" -o $@
objs/client.o: src/client.cpp src/commands.hpp src/tools.hpp src/mechanizm.hpp \
 src/Object.hpp src/options.hpp src/Camera.hpp src/Side.hpp src/Block.hpp \
 src/MapSection.hpp src/Q.hpp src/Map.hpp
	@ echo "    CXX  src/client.cpp"
	@ $(CXX) $(CFLAGS) -c "src/client.cpp" -o $@
objs/commands.o: src/commands.cpp src/commands.hpp src/tools.hpp
	@ echo "    CXX  src/commands.cpp"
	@ $(CXX) $(CFLAGS) -c "src/commands.cpp" -o $@
objs/Map.o: src/Map.cpp src/Map.hpp src/tools.hpp src/MapSection.hpp \
 src/Block.hpp src/Side.hpp src/Camera.hpp src/Q.hpp
	@ echo "    CXX  src/Map.cpp"
	@ $(CXX) $(CFLAGS) -c "src/Map.cpp" -o $@
objs/MapSection.o: src/MapSection.cpp src/MapSection.hpp src/tools.hpp \
 src/Block.hpp src/Side.hpp src/Camera.hpp src/Q.hpp
	@ echo "    CXX  src/MapSection.cpp"
	@ $(CXX) $(CFLAGS) -c "src/MapSection.cpp" -o $@
objs/mechanizm.o: src/mechanizm.cpp src/mechanizm.hpp src/tools.hpp \
 src/Object.hpp
	@ echo "    CXX  src/mechanizm.cpp"
	@ $(CXX) $(CFLAGS) -c "src/mechanizm.cpp" -o $@
objs/Object.o: src/Object.cpp src/Object.hpp src/tools.hpp
	@ echo "    CXX  src/Object.cpp"
	@ $(CXX) $(CFLAGS) -c "src/Object.cpp" -o $@
objs/options.o: src/options.cpp src/options.hpp src/tools.hpp
	@ echo "    CXX  src/options.cpp"
	@ $(CXX) $(CFLAGS) -c "src/options.cpp" -o $@
objs/Side.o: src/Side.cpp src/Side.hpp
	@ echo "    CXX  src/Side.cpp"
	@ $(CXX) $(CFLAGS) -c "src/Side.cpp" -o $@
objs/Sphere.o: src/Sphere.cpp src/Sphere.hpp
	@ echo "    CXX  src/Sphere.cpp"
	@ $(CXX) $(CFLAGS) -c "src/Sphere.cpp" -o $@
objs/radix.o: src/sorters/radix.cpp src/sorters/../sorters.hpp
	@ echo "    CXX  src/sorters/radix.cpp"
	@ $(CXX) $(CFLAGS) -c "src/sorters/radix.cpp" -o $@
objs/doubles.o: src/tools/doubles.cpp src/tools/../tools.hpp
	@ echo "    CXX  src/tools/doubles.cpp"
	@ $(CXX) $(CFLAGS) -c "src/tools/doubles.cpp" -o $@
objs/json_utils.o: src/tools/json_utils.cpp src/tools/../tools.hpp
	@ echo "    CXX  src/tools/json_utils.cpp"
	@ $(CXX) $(CFLAGS) -c "src/tools/json_utils.cpp" -o $@
objs/pcre_utils.o: src/tools/pcre_utils.cpp src/tools/../tools.hpp
	@ echo "    CXX  src/tools/pcre_utils.cpp"
	@ $(CXX) $(CFLAGS) -c "src/tools/pcre_utils.cpp" -o $@
objs/strings.o: src/tools/strings.cpp src/tools/../tools.hpp
	@ echo "    CXX  src/tools/strings.cpp"
	@ $(CXX) $(CFLAGS) -c "src/tools/strings.cpp" -o $@
objs/system.o: src/tools/system.cpp src/tools/../tools.hpp
	@ echo "    CXX  src/tools/system.cpp"
	@ $(CXX) $(CFLAGS) -c "src/tools/system.cpp" -o $@
objs/utils.o: src/tools/utils.cpp src/tools/../sorters.hpp \
 src/tools/../tools.hpp
	@ echo "    CXX  src/tools/utils.cpp"
	@ $(CXX) $(CFLAGS) -c "src/tools/utils.cpp" -o $@
objs/vectors.o: src/tools/vectors.cpp src/tools/../tools.hpp
	@ echo "    CXX  src/tools/vectors.cpp"
	@ $(CXX) $(CFLAGS) -c "src/tools/vectors.cpp" -o $@

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
