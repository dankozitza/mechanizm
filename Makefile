# eec9080481811c9f16a5bdf17583353e
# Generated with vfnmkmc by the mc program.
PREFIX=/usr/local
CFLAGS=-O$(O)  -std=c++11
O=2
LFLAGS= -l pcre2-8 -l GL -l GLU -l glut -l glfw -l GLEW -l assimp
OBJS=objs/mechanizm.o objs/options.o objs/Camera.o objs/anim_test.o objs/Object.o objs/radix.o objs/scene.o objs/glstuff.o objs/png_loader.o objs/vectors.o objs/pcre2.o objs/doubles.o objs/system.o objs/utils.o objs/strings.o


.PHONY: all
all: objs client

./client:  $(OBJS)
	@ echo "    LINK ./client"
	@ $(CXX) $(OBJS) -o "./client" $(LFLAGS)

objs/mechanizm.o: src/mechanizm.cpp src/mechanizm.hpp src/tools.hpp \
 src/Object.hpp
	@ echo "    CXX  src/mechanizm.cpp"
	@ $(CXX) $(CFLAGS) -c "src/mechanizm.cpp" -o $@
objs/options.o: src/options.cpp src/options.hpp src/tools.hpp
	@ echo "    CXX  src/options.cpp"
	@ $(CXX) $(CFLAGS) -c "src/options.cpp" -o $@
objs/Camera.o: src/Camera.cpp src/Camera.hpp
	@ echo "    CXX  src/Camera.cpp"
	@ $(CXX) $(CFLAGS) -c "src/Camera.cpp" -o $@
objs/anim_test.o: src/anim_test.cpp src/../include/linealg.h \
 src/../include/vector2.h src/../include/vector3.h \
 src/../include/vector4.h src/../include/matrix3.h \
 src/../include/matrix4.h src/assimp_wrapper/glstuff.h \
 src/assimp_wrapper/scene.h
	@ echo "    CXX  src/anim_test.cpp"
	@ $(CXX) $(CFLAGS) -c "src/anim_test.cpp" -o $@
objs/Object.o: src/Object.cpp src/Object.hpp src/tools.hpp
	@ echo "    CXX  src/Object.cpp"
	@ $(CXX) $(CFLAGS) -c "src/Object.cpp" -o $@
objs/radix.o: src/sorters/radix.cpp src/sorters/../sorters.hpp
	@ echo "    CXX  src/sorters/radix.cpp"
	@ $(CXX) $(CFLAGS) -c "src/sorters/radix.cpp" -o $@
objs/scene.o: src/assimp_wrapper/scene.cpp src/assimp_wrapper/scene.h \
 src/assimp_wrapper/png_loader.h src/assimp_wrapper/glstuff.h
	@ echo "    CXX  src/assimp_wrapper/scene.cpp"
	@ $(CXX) $(CFLAGS) -c "src/assimp_wrapper/scene.cpp" -o $@
objs/glstuff.o: src/assimp_wrapper/glstuff.cpp src/assimp_wrapper/glstuff.h
	@ echo "    CXX  src/assimp_wrapper/glstuff.cpp"
	@ $(CXX) $(CFLAGS) -c "src/assimp_wrapper/glstuff.cpp" -o $@
objs/png_loader.o: src/assimp_wrapper/png_loader.cpp
	@ echo "    CXX  src/assimp_wrapper/png_loader.cpp"
	@ $(CXX) $(CFLAGS) -c "src/assimp_wrapper/png_loader.cpp" -o $@
objs/vectors.o: src/tools/vectors.cpp src/tools/../tools.hpp
	@ echo "    CXX  src/tools/vectors.cpp"
	@ $(CXX) $(CFLAGS) -c "src/tools/vectors.cpp" -o $@
objs/pcre2.o: src/tools/pcre2.cpp src/tools/../tools.hpp
	@ echo "    CXX  src/tools/pcre2.cpp"
	@ $(CXX) $(CFLAGS) -c "src/tools/pcre2.cpp" -o $@
objs/doubles.o: src/tools/doubles.cpp src/tools/../tools.hpp
	@ echo "    CXX  src/tools/doubles.cpp"
	@ $(CXX) $(CFLAGS) -c "src/tools/doubles.cpp" -o $@
objs/system.o: src/tools/system.cpp src/tools/../tools.hpp
	@ echo "    CXX  src/tools/system.cpp"
	@ $(CXX) $(CFLAGS) -c "src/tools/system.cpp" -o $@
objs/utils.o: src/tools/utils.cpp src/tools/../sorters.hpp \
 src/tools/../tools.hpp
	@ echo "    CXX  src/tools/utils.cpp"
	@ $(CXX) $(CFLAGS) -c "src/tools/utils.cpp" -o $@
objs/strings.o: src/tools/strings.cpp src/tools/../tools.hpp
	@ echo "    CXX  src/tools/strings.cpp"
	@ $(CXX) $(CFLAGS) -c "src/tools/strings.cpp" -o $@

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
