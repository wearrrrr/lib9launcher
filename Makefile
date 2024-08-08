# lib9launcher Makefile

	CC = clang
	DLL_NAME = lib9launcher.dll
	OBJECT_NAME = lib9launcher.o
	INPUT_FILES = main.cpp
	WIN32UTF8 = ../thcrap/libs/win32_utf8
	THCRAP_EXTERNAL_DEPS = ../thcrap/libs/external_deps/include/
	THCRAP_HEADERS = ../thcrap/thcrap/src
	PACKER = upx
	PACKED_DLL_NAME = ${DLL_NAME}_packed
	LIBS = thcrap.lib

	RELEASE = TRUE

build:
	${CC} -target i686-pc-win32-gnu -shared ${INPUT_FILES} -Oz ${LIBS} -I${WIN32UTF8} -I${THCRAP_EXTERNAL_DEPS} -I${THCRAP_HEADERS} -std=c++20 -D_GLIBCXX_USE_DEPRECATED=0 -Wno-ignored-attributes -Wno-deprecated-volatile -o ${DLL_NAME}
	@if [ -f ${DLL_NAME} ]; then \
		echo "Build successful!"; \
	else \
		echo "Build failed!"; \
	fi

strip:
	@echo "Stripping symbols..."
	@strip ${DLL_NAME}
	@echo "DLL stripped!"

clean:
	@echo "Cleaning up..."
	@rm -f ${DLL_NAME}
	@echo "Output files removed!"

pack:
	@echo "Packing with " ${PACKER}
	# Currently only UPX works properly, but I am not prioritizing packing anyways so, it doesn't matter too much.
	@${PACKER} ${DLL_NAME} -o packed_${DLL_NAME}
	@echo "DLL Packed!"

.PHONY: clean build strip

all: clean build strip
