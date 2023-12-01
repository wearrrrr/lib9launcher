# lib9launcher Makefile

	CC = clang
	DLL_NAME = lib9launcher.dll
	INPUT_FILES = main.cpp
	WIN32UTF8 = ../thcrap/libs/win32_utf8
	THCRAP_EXTERNAL_DEPS = ../thcrap/libs/external_deps/include/
	THCRAP_HEADERS = ../thcrap/thcrap/src

	RELEASE = TRUE

build:
	${CC} -target i686-pc-win32-gnu -shared ${INPUT_FILES} -Oz thcrap.lib -I${WIN32UTF8} -I${THCRAP_EXTERNAL_DEPS} -I${THCRAP_HEADERS} -D_GLIBCXX_USE_DEPRECATED=0 -nostdlib -o ${DLL_NAME}
	@if [ -f ${DLL_NAME} ]; then \
		echo "Build successful!"; \
	else \
		echo "Build failed!"; \
	fi

strip:
	@echo "Stripping symbols..."
	@strip ${DLL_NAME}
	@echo "Done!"

clean:
	@echo "Cleaning up..."
	@rm -f ${DLL_NAME}
	@echo "Done!"

.PHONY: clean build strip

all: clean build strip