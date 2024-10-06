# Linux
CC := gcc
CXX := g++
HOST_ARCH := $(shell uname -m)
CFLAGS :=

# Android
NDK_BUILD := NDK_PROJECT_PATH=. ndk-build NDK_APPLICATION_MK=./Application.mk

# Name for the release ZIP
GIT_SHA_FETCH := $(shell git rev-parse HEAD | cut -c 1-8)
ZIP_NAME := uidump-parser-$(GIT_SHA_FETCH).zip

# Retrieve binary name from Android.mk
BIN := $(shell cat Android.mk | grep LOCAL_MODULE  | head -n1 | cut -d' ' -f3)

# Out folder, where binaries are built to
BIN_PATH := libs/arm64-v8a/$(BIN)
HOST_BIN_PATH := libs/linux-$(shell uname -m)

all: linux android

$(BIN_PATH):
	$(NDK_BUILD)

linux: main.o tinyxml2.o
	@echo "Building Linux"
	mkdir -p $(HOST_BIN_PATH)
	$(CXX) -o $(HOST_BIN_PATH)/$(BIN) $^

android:
	@echo "Building Android"
	$(NDK_BUILD)

tinyxml2.o: src/tinyxml2/tinyxml2.cpp
	$(CXX) -c src/tinyxml2/tinyxml2.cpp

main.o: src/main.cpp
	$(CXX) -c src/main.cpp

release: all
	zip -r $(ZIP_NAME) libs

clean:
	$(NDK_BUILD) clean

distclean: clean
	$(RM) -rf libs obj *.zip