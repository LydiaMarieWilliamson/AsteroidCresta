## Asteroid Style Game, QT-based makefile
## Big Angry Dog (c) 2010
## Revisions (c) 2021 Darth Spectra (Lydia Marie Williamson)

## QMake Notes: See <Qt Path>/doc/html/qmake-variable-reference.html for more info.
## HEADERS:	The application's header files.
## SOURCES:	The application's source files.
## FORMS:	The application's .ui files (created using Qt Designer).
## TARGET:	The application's name.
## DESTDIR:	The application's location.
## DEFINES:	Additional pre-processor defines (DEFINES +=).
## INCLUDEPATH:	Additional include paths needed for the application.
## OBJECTS_DIR:	Objects directory
## DEPENDPATH:	The application's dependency search path.
## VPATH:	The search path to find supplied files.
## LIBS:	Libraries i.e. LIBS += c:/mylibs/math.lib
## VERSION:	Application or library version, i.e. VERSION = 1.2.3
## TEMPLATE:	The template to use for the project. (app: default, lib: see Qt docs)
## DEF_FILE:	Windows only: A .def file to be linked against for the application.
## RC_FILE:	Windows only: A resource file for the application.
## RES_FILE:	Windows only: A resource file to be linked against for the application.
## Operators: i.e. += append, -= remove, *= append if not present
## Variables: i.e. $${DEFINES}

## Configuration
## Setup:
TEMPLATE = app
CONFIG += qt windows warn_on release
QT += phonon

## Paths:
TARGET = Asteroid
DESTDIR = .
INCLUDEPATH = .

## Defines:
win32:DEFINES *= WINDOWS WIN32 UNICODE NO_DEBUG
#debug:DEFINES *= _DEBUG

## Resources:
RESOURCES = Image.qrc
RC_FILE = Icon.rc

## Objects and Temp files:
debug:OBJECTS_DIR = Temp
debug:RCC_DIR = Temp
debug:MOC_DIR = Temp
release:OBJECTS_DIR = Temp
release:RCC_DIR = Temp
release:MOC_DIR = Temp

## Header Files:
HEADERS += About.h
HEADERS += Arena.h
HEADERS += Engine.h
HEADERS += Game.h
HEADERS += Objects.h
HEADERS += Version.h

## Source Files:
SOURCES += About.cpp
SOURCES += Arena.cpp
SOURCES += Asteroid.cpp
SOURCES += Engine.cpp
SOURCES += Game.cpp
SOURCES += Objects.cpp
