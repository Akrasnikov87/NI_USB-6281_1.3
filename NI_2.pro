QT += core\
      gui\
      widgets\
      printsupport\

CONFIG += c++11

TARGET = National_Instruments_1.3
TEMPLATE = app

LIBS += -L$$PWD -lNIDAQmx


SOURCES += main.cpp\
           widget.cpp

HEADERS += widget.h \
           NIDAQmx.h
