
# ******************************************************************************
#  event_capture.pro                                               Tao project
# ******************************************************************************
# File Description:
# Qt build file for the event_capture module
# ******************************************************************************
# This software is property of Taodyne SAS - Confidential
# Ce logiciel est la propriété de Taodyne SAS - Confidentiel
# (C) 2010 Catherine Burvelle <cathy@taodyne.com>
# (C) 2010 Taodyne SAS
# ******************************************************************************

MODINSTDIR = tao_synchro

include(../modules.pri)

release:QMAKE_CXXFLAGS -= -Werror  # Workaround QTBUG-14437 (GCC 4.4)

TBL_SOURCES = tao_synchro.tbl

QT += core gui opengl network

OTHER_FILES += \
    tao_synchro.xl \
    tao_synchro.tbl \
    tao_synchro.taokey.notsigned

HEADERS += \
    tao_synchro.h \
    event_capture.h \
    traces.tbl \
    tao_control_event.h \
    event_handler.h \
    doc/tao_synchro.doxy.h

SOURCES += \
    tao_synchro.cpp \
    event_capture.cpp \
    tao_control_event.cpp \
    event_handler.cpp

LICENSE_FILES = tao_synchro.taokey.notsigned
include(../licenses.pri)
