QT       += core \
            serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = follower
TEMPLATE = app

CONFIG += c++11 \          
          console


SOURCES += \
    CANWorkerThread.cpp \
    Follower.cpp \
    followerui.cpp \
    Kinematics.cpp \
    main.cpp \
    TRobot.cpp \
    WorkerObjectBase.cpp \
    WorkerObjectCAN.cpp \
    WorkerThreadBase.cpp \
    Data/DataLayerBase.cpp \
    Data/DataLayerCAN.cpp \
    Data/DataLayerConfig.cpp \
    Data/PacketConfig.cpp \
    Data/PacketHandler.cpp \
    Data/Typedef.cpp \
    Spine/SpineCommCAN.cpp \
    Algorithms/algorithmbuilder.cpp \
    Algorithms/algorithmwander.cpp \
    Algorithms/algorithmtrack.cpp \
    Algorithms/algorithmturn.cpp

HEADERS  +=  \
    CANWorkerThread.h \
    Follower.h \
    followerui.h \
    Kinematics.h \
    TRobot.h \
    WorkerObjectBase.h \
    WorkerObjectCAN.h \
    WorkerThreadBase.h \
    Data/CANmessages.h \
    Data/Data.h \
    Data/DataLayerBase.h \
    Data/DataLayerCAN.h \
    Data/DataLayerConfig.h \
    Data/PacketConfig.h \
    Data/PacketHandler.h \
    Data/Typedef.h \
    Spine/CSpineComm.h \
    Spine/SpineCommCAN.h \
    Algorithms/ialgorithm.h \
    Algorithms/algorithmbuilder.h \
    Algorithms/algorithmwander.h \
    Algorithms/algorithmtrack.h \
    Algorithms/algorithmturn.h

INCLUDEPATH += \
	 ./Data \
     ./Spine \
     ./Algorithms

FORMS    += \
    followerui.ui
