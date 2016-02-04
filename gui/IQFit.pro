######################################################################################
#
# This program solves the 2D puzzle "IQ Fit" based on a precomputed set of solutions.
# Copyright (C) 2016  Dominik Vilsmeier

# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
######################################################################################

#-------------------------------------------------
#
# Project created by QtCreator 2016-01-10T00:32:02
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += static
LIBS += -static -lpthread -static-libgcc -static-libstdc++

TARGET = IQFit
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    boardwidget.cpp \
    piece.cpp \
    containerwidget.cpp \
    rowsolver.cpp \
    waiter.cpp

HEADERS  += mainwindow.h \
    boardwidget.h \
    piece.h \
    containerwidget.h \
    rowsolver.h \
    waiter.h
