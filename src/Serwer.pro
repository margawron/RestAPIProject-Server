QT -= gui

CONFIG += c++17 console
CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += /soft/libs/boost_1_70_0
LIBS += /soft/libs/boost_1_70_0/stage/lib/libboost_system.a
LIBS += /soft/libs/boost_1_70_0/stage/lib/libboost_filesystem.a

SOURCES += \
        connection.cpp \
        database.cpp \
        databaseadaptor.cpp \
        exception.cpp \
        http_codes.cpp \
        iconnectionadaptor.cpp \
        iserializable.cpp \
        main.cpp \
        server.cpp \
        user.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    connection.h \
    database.h \
    databaseadaptor.h \
    exception.h \
    http_codes.h \
    iconnectionadaptor.h \
    iserializable.h \
    server.h \
    user.h
