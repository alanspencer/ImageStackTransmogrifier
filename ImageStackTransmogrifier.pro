#-------------------------------------------------
#
# Project created by QtCreator 2013-11-19T18:46:10
#
#-------------------------------------------------

QT       += core \
            gui \

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QTPLUGIN     += qjpeg \
                qgif \
                qtiff \

TARGET = ImageStackTransmogrifier
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

# The application version
VERSION = 0.1

# Define the preprocessor macro to get the application version in our application.
DEFINES += APP_VERSION=$$VERSION
