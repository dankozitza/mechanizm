# 16152424fc71c433c4b10c691fdb4557
# Generated with vfnmkmc by the mc program.
PREFIX=/usr/local
CFLAGS=-O$(O)  -std=c++11
O=2
LFLAGS= -l GL -l GLU -l glut -l jsoncpp
OBJS=objs/Camera.o objs/Group.o objs/Object.o objs/ObjectMap.o objs/Sphere.o objs/Tetrahedron.o objs/Triangle.o objs/Vertex.o objs/commands.o objs/mechanizm.o objs/mechanizm_game.o objs/options.o objs/radix.o objs/doubles.o objs/json_utils.o objs/ogl_utils.o objs/pcre_utils.o objs/strings.o objs/system.o objs/utils.o objs/vectors.o


.PHONY: all
all: objs mechanizm_game

./mechanizm_game:  $(OBJS)
	@ echo "    LINK ./mechanizm_game"
	@ $(CXX) $(OBJS) -o "./mechanizm_game" $(LFLAGS)

objs/Camera.o: src/Camera.cpp src/Camera.hpp
	@ echo "    CXX  src/Camera.cpp"
	@ $(CXX) $(CFLAGS) -c "src/Camera.cpp" -o $@
objs/Group.o: src/Group.cpp src/Group.hpp src/tools.hpp src/Vertex.hpp \
 src/Triangle.hpp src/Object.hpp src/ObjectMap.hpp src/Tetrahedron.hpp
	@ echo "    CXX  src/Group.cpp"
	@ $(CXX) $(CFLAGS) -c "src/Group.cpp" -o $@
objs/Object.o: src/Object.cpp src/Object.hpp src/tools.hpp src/Vertex.hpp \
 src/ObjectMap.hpp src/Tetrahedron.hpp src/Triangle.hpp
	@ echo "    CXX  src/Object.cpp"
	@ $(CXX) $(CFLAGS) -c "src/Object.cpp" -o $@
objs/ObjectMap.o: src/ObjectMap.cpp src/ObjectMap.hpp src/tools.hpp \
 src/Vertex.hpp
	@ echo "    CXX  src/ObjectMap.cpp"
	@ $(CXX) $(CFLAGS) -c "src/ObjectMap.cpp" -o $@
objs/Sphere.o: src/Sphere.cpp src/Sphere.hpp
	@ echo "    CXX  src/Sphere.cpp"
	@ $(CXX) $(CFLAGS) -c "src/Sphere.cpp" -o $@
objs/Tetrahedron.o: src/Tetrahedron.cpp src/Tetrahedron.hpp src/Vertex.hpp \
 src/Triangle.hpp
	@ echo "    CXX  src/Tetrahedron.cpp"
	@ $(CXX) $(CFLAGS) -c "src/Tetrahedron.cpp" -o $@
objs/Triangle.o: src/Triangle.cpp src/Triangle.hpp src/Vertex.hpp
	@ echo "    CXX  src/Triangle.cpp"
	@ $(CXX) $(CFLAGS) -c "src/Triangle.cpp" -o $@
objs/Vertex.o: src/Vertex.cpp src/Vertex.hpp
	@ echo "    CXX  src/Vertex.cpp"
	@ $(CXX) $(CFLAGS) -c "src/Vertex.cpp" -o $@
objs/commands.o: src/commands.cpp src/commands.hpp src/tools.hpp \
 src/Vertex.hpp
	@ echo "    CXX  src/commands.cpp"
	@ $(CXX) $(CFLAGS) -c "src/commands.cpp" -o $@
objs/mechanizm.o: src/mechanizm.cpp src/mechanizm.hpp src/tools.hpp \
 src/Vertex.hpp src/Object.hpp src/ObjectMap.hpp src/Tetrahedron.hpp \
 src/Triangle.hpp
	@ echo "    CXX  src/mechanizm.cpp"
	@ $(CXX) $(CFLAGS) -c "src/mechanizm.cpp" -o $@
objs/mechanizm_game.o: src/mechanizm_game.cpp src/commands.hpp src/tools.hpp \
 src/Vertex.hpp src/mechanizm.hpp src/Object.hpp src/ObjectMap.hpp \
 src/Tetrahedron.hpp src/Triangle.hpp src/options.hpp src/Camera.hpp \
 src/Group.hpp src/Sphere.hpp
	@ echo "    CXX  src/mechanizm_game.cpp"
	@ $(CXX) $(CFLAGS) -c "src/mechanizm_game.cpp" -o $@
objs/options.o: src/options.cpp src/options.hpp src/tools.hpp src/Vertex.hpp
	@ echo "    CXX  src/options.cpp"
	@ $(CXX) $(CFLAGS) -c "src/options.cpp" -o $@
objs/radix.o: src/sorters/radix.cpp src/sorters/../sorters.hpp
	@ echo "    CXX  src/sorters/radix.cpp"
	@ $(CXX) $(CFLAGS) -c "src/sorters/radix.cpp" -o $@
objs/doubles.o: src/tools/doubles.cpp src/tools/../tools.hpp \
 src/tools/../Vertex.hpp
	@ echo "    CXX  src/tools/doubles.cpp"
	@ $(CXX) $(CFLAGS) -c "src/tools/doubles.cpp" -o $@
objs/json_utils.o: src/tools/json_utils.cpp src/tools/../tools.hpp \
 src/tools/../Vertex.hpp
	@ echo "    CXX  src/tools/json_utils.cpp"
	@ $(CXX) $(CFLAGS) -c "src/tools/json_utils.cpp" -o $@
objs/ogl_utils.o: src/tools/ogl_utils.cpp src/tools/../tools.hpp \
 src/tools/../Vertex.hpp
	@ echo "    CXX  src/tools/ogl_utils.cpp"
	@ $(CXX) $(CFLAGS) -c "src/tools/ogl_utils.cpp" -o $@
objs/pcre_utils.o: src/tools/pcre_utils.cpp src/tools/../tools.hpp \
 src/tools/../Vertex.hpp
	@ echo "    CXX  src/tools/pcre_utils.cpp"
	@ $(CXX) $(CFLAGS) -c "src/tools/pcre_utils.cpp" -o $@
objs/strings.o: src/tools/strings.cpp src/tools/../tools.hpp \
 src/tools/../Vertex.hpp
	@ echo "    CXX  src/tools/strings.cpp"
	@ $(CXX) $(CFLAGS) -c "src/tools/strings.cpp" -o $@
objs/system.o: src/tools/system.cpp src/tools/../tools.hpp \
 src/tools/../Vertex.hpp
	@ echo "    CXX  src/tools/system.cpp"
	@ $(CXX) $(CFLAGS) -c "src/tools/system.cpp" -o $@
objs/utils.o: src/tools/utils.cpp src/tools/../sorters.hpp \
 src/tools/../tools.hpp src/tools/../Vertex.hpp
	@ echo "    CXX  src/tools/utils.cpp"
	@ $(CXX) $(CFLAGS) -c "src/tools/utils.cpp" -o $@
objs/vectors.o: src/tools/vectors.cpp src/tools/../tools.hpp \
 src/tools/../Vertex.hpp
	@ echo "    CXX  src/tools/vectors.cpp"
	@ $(CXX) $(CFLAGS) -c "src/tools/vectors.cpp" -o $@

objs:
	@ mkdir "objs"

.PHONY: c clean
c: clean
clean:
	@ if [ -d "objs" ]; then rm -r "objs"; fi
	@ rm -f "./mechanizm_game"
	@ echo "    CLEAN"
.PHONY: f fresh
f: fresh
fresh: clean
	@ make all --no-print-directory
.PHONY: r run
r: run
run: all
	@ ././mechanizm_game

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
	@ install -D -m 755 mechanizm_game $(PREFIX)/bin/mechanizm_game
	@ echo "[1;32m*[0m mechanizm_game installed in $(PREFIX)/bin"
	@ echo "[1muse \`make uninstall\` to remove mechanizm_game[0m"

.PHONY: uninstall
uninstall:
	@ rm $(PREFIX)/bin/mechanizm_game
	@ echo "[1;32m*[0m mechanizm_game removed from $(PREFIX)/bin[0m"

.PHONY: check-syntax
check-syntax:
	$(CXX) $(CFLAGS) -fsyntax-only -Wall -o /dev/null -S src/*
