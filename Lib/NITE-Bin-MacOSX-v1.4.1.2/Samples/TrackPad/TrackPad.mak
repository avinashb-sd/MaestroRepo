OSTYPE := $(shell uname -s)

SRC_FILES = \
	main.cpp \
	kbhit.cpp \
	signal_catch.cpp

INC_DIRS += ../../../../../Samples/TrackPad

ifeq ("$(OSTYPE)","Darwin")
        LDFLAGS += -framework OpenGL -framework GLUT
else
        USED_LIBS += glut
endif

DEFINES = USE_GLUT

EXE_NAME = Sample-TrackPad
include ../NiteSampleMakefile

