
noinst_LTLIBRARIES = libjudo.la

libjudo_la_SOURCES = ./include/judo.h \
                     ./src/judo.cpp \
                     ./src/Element.cpp \
                     ./src/ElementStream.cpp

libjudo_la_LIBADD = ./expat/libexpat.la

INCLUDES = -I./expat \
           -I./include

if TEST
SUBDIRS = expat . test
else
SUBDIRS = expat . 
endif
