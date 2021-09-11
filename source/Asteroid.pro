# -----------------------------------------------------------------------------
#  PROJECT     : Asteroid Style Game
#  FILE NAME   : asteroid.pro
#  DESCRIPTION : QT based makefile
#  COPYRIGHT   : Big Angry Dog (C) 2010
#  This file is part of the "Asteroid Style Game" program.
#  Asteroid Style Game is free software: you can redistribute it and/or
#  modify it under the terms of the GNU General Public License as published
#  by the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#  Asteroid Style Game is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#  You should have received a copy of the GNU General Public License
#  along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
# -----------------------------------------------------------------------------

# -----------------------------------------------------------------------------
# QMAKE NOTES
# See <Qt Path>/doc/html/qmake-variable-reference.html for more info
# -----------------------------------------------------------------------------
# HEADERS - A list of all the header files for the application.
# SOURCES - A list of all the source files for the application.
# FORMS - A list of all the .ui files (created using Qt Designer) for the application.
# TARGET - Name of the executable for the application.
# DESTDIR - The directory in which the target executable is placed.
# DEFINES - A list of any additional pre-processor defines (DEFINES += ).
# INCLUDEPATH - A list of any additional include paths needed for the application.
# OBJECTS_DIR - objects directory
# DEPENDPATH - The dependency search path for the application.
# VPATH - The search path to find supplied files.
# DEF_FILE - Windows only: A .def file to be linked against for the application.
# LIBS - Libraries i.e. LIBS += c:/mylibs/math.lib
# RC_FILE - Windows only: A resource file for the application.
# RES_FILE - Windows only: A resource file to be linked against for the application.
# VERSION - Application or library version, i.e. VERSION = 1.2.3
# TEMPLATE - The template to use for the project. (app (default), lib - see Qt docs)
# Operators: i.e. += append, -= remove, *= append if not present
# Variables: i.e. $${DEFINES}
# -----------------------------------------------------------------------------

# -----------------------------------------------------------------------------
# CONFIGURATION
# -----------------------------------------------------------------------------

# Setup
TEMPLATE = app
CONFIG += qt windows warn_on release
QT += phonon

# Paths
TARGET = asteroid
DESTDIR = ./bin
INCLUDEPATH = src_cpp src_qt

# Defines
win32:DEFINES *= WINDOWS WIN32 UNICODE NO_DEBUG
#debug:DEFINES *= _DEBUG

# Resources
RESOURCES = ./res/res_qt.qrc
RC_FILE = ./res/res_win.rc

# Objects and temp files
debug:OBJECTS_DIR = ./tmp/debug_obj
release:OBJECTS_DIR = ./tmp/release_obj
debug:RCC_DIR = ./tmp/debug_rcc
release:RCC_DIR = ./tmp/release_rcc
debug:MOC_DIR = ./tmp/debug_moc
release:MOC_DIR = ./tmp/release_moc

# -----------------------------------------------------------------------------
# HEADER FILES
# -----------------------------------------------------------------------------
HEADERS += src_qt/app_version.h
HEADERS += src_qt/main_window.h
HEADERS += src_qt/about_window.h
HEADERS += src_qt/game_widget.h
# HEADERS += src_cpp/app_version.h
HEADERS += src_cpp/game_objects.h
HEADERS += src_cpp/game_engine.h

# -----------------------------------------------------------------------------
# SOURCE FILES
# -----------------------------------------------------------------------------
SOURCES += src_qt/main.cpp
SOURCES += src_qt/main_window.cpp
SOURCES += src_qt/about_window.cpp
SOURCES += src_qt/game_widget.cpp
SOURCES += src_cpp/game_objects.cpp
SOURCES += src_cpp/game_engine.cpp
