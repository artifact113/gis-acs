TEMPLATE = app
TARGET = gis-acs

QT += xml

HEADERS += \
    graph.h \
    graphmodel.h

SOURCES += \
    graph.cpp \
    main.cpp \
    graphmodel.cpp

OTHER_FILES += \
    test_short_paths.xml
