TEMPLATE = app
TARGET = gis-acs

QT += xml

HEADERS += \
    graph.h \
    graphmodel.h \
    mainwindow.h \
    singletons.h \
    graphgeneratorwidget.h

SOURCES += \
    graph.cpp \
    main.cpp \
    graphmodel.cpp \
    mainwindow.cpp \
    graphgeneratorwidget.cpp

OTHER_FILES += \
    test_short_paths.xml

FORMS += \
    mainwindow.ui
