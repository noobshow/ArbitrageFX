QT -= gui

CONFIG += console
CONFIG += c++11
CONFIG -= app_bundle

QMAKE_CXXFLAGS += -pthread
QMAKE_CXXFLAGS -= -Wdeprecated

QMAKE_CXXFLAGS_RELEASE -= -O1
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3

LIBS += -ldl
LIBS += -lzmq
LIBS += -lstdc++fs
LIBS += -lquickfix

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += $$PWD/../src

SOURCES += \
    main.cpp \
    ../src/fix/FixApplication.cpp \
    ../src/fix/FixServer.cpp \
    ../src/common/Config.cpp \
    ../src/common/loguru.cpp \
    ../src/common/Symbol.cpp \
    ../src/common/Broker.cpp \
    ../src/arbitrage/ArbitrageManager.cpp \
    ../src/arbitrage/ArbitrageCore.cpp \
    ../src/simulation/MuxMt4Recorder.cpp \
    ../src/simulation/DemuxMt4Stream.cpp \
    ../src/simulation/TickMt4Recorder.cpp \
    ../src/simulation/SimulatorMt4Pub.cpp \
    ../src/decoder/PriceMt4Decoder.cpp \
    ../src/decoder/TradeMt4Decoder.cpp \
    ../src/iostream/PriceMt4Pull.cpp \
    ../src/iostream/TradeMt4Pull.cpp \
    ../src/iostream/TradeMt4Publisher.cpp \
    ../src/arbitrage/ArbitrageReport.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    ../src/common/Types.hpp \
    ../src/common/AsyncThreadProcess.hpp \
    ../src/common/Dispatcher.hpp \
    ../src/common/Router.hpp \
    ../src/common/ExecutionTimer.hpp \
    ../src/common/Observer.hpp \
    ../src/common/Singleton.hpp \
    ../src/common/Utils.hpp \
    ../src/common/Config.hpp \
    ../src/common/mt4Protocol.hpp \
    ../src/common/zhelpers.hpp \
    ../src/common/options.hpp \
    ../src/common/Config.hpp \
    ../src/common/loguru.hpp \
    ../src/common/inicpp.hpp \
    ../src/common/Symbol.hpp \
    ../src/common/Broker.hpp \
    ../src/common/defines.hpp \
    ../src/common/ctpl_stl.h \
    ../src/fix/FixApplication.hpp \
    ../src/fix/FixServer.hpp \
    ../src/arbitrage/ArbitrageManager.hpp \
    ../src/arbitrage/ArbitrageCore.hpp \
    ../src/simulation/MuxMt4Recorder.hpp \
    ../src/simulation/DemuxMt4Stream.hpp \
    ../src/simulation/TickMt4Recorder.hpp \
    ../src/simulation/SimulatorMt4Pub.hpp \
    ../src/decoder/PriceMt4Decoder.hpp \
    ../src/decoder/TradeMt4Decoder.hpp \
    ../src/iostream/IoStream.hpp \
    ../src/iostream/PriceMt4Pull.hpp \
    ../src/iostream/TradeMt4Pull.hpp \
    ../src/iostream/Mt4Publisher.hpp \
    ../src/iostream/TradeMt4Publisher.hpp \
    ../src/arbitrage/ArbitrageReport.hpp
