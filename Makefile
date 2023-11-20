# lib9launcher Makefile

	CC = i686-w64-mingw32-gcc
	DLL_NAME = lib9launcher.dll
	INPUT_FILES = main.cpp
	WIN32UTF8 = ../thcrap/libs/win32_utf8
	THCRAP_EXTERNAL_DEPS = ../thcrap/libs/external_deps/include/
	THCRAP_HEADERS = ../thcrap/thcrap/src

build:
	${CC} -shared -o ${DLL_NAME} ${INPUT_FILES} thcrap.lib -I${WIN32UTF8} -I${THCRAP_EXTERNAL_DEPS} -I${THCRAP_HEADERS}

clean:
	@echo "Cleaning up..."
	@rm -f ${DLL_NAME}
	@echo "Done!"
	make build
