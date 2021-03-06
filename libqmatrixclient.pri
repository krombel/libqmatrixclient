QT += network
CONFIG += c++14 warn_on rtti_off

win32-msvc* {
    QMAKE_CXXFLAGS_WARN_ON += -wd4100
} else {
    QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-parameter
}

INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/connectiondata.h \
    $$PWD/connection.h \
    $$PWD/room.h \
    $$PWD/user.h \
    $$PWD/avatar.h \
    $$PWD/util.h \
    $$PWD/events/event.h \
    $$PWD/events/eventcontent.h \
    $$PWD/events/roommessageevent.h \
    $$PWD/events/simplestateevents.h \
    $$PWD/events/roommemberevent.h \
    $$PWD/events/roomavatarevent.h \
    $$PWD/events/typingevent.h \
    $$PWD/events/receiptevent.h \
    $$PWD/events/accountdataevents.h \
    $$PWD/events/redactionevent.h \
    $$PWD/jobs/requestdata.h \
    $$PWD/jobs/basejob.h \
    $$PWD/jobs/checkauthmethods.h \
    $$PWD/jobs/passwordlogin.h \
    $$PWD/jobs/sendeventjob.h \
    $$PWD/jobs/postreceiptjob.h \
    $$PWD/jobs/joinroomjob.h \
    $$PWD/jobs/roommessagesjob.h \
    $$PWD/jobs/syncjob.h \
    $$PWD/jobs/mediathumbnailjob.h \
    $$PWD/jobs/setroomstatejob.h \
    $$files($$PWD/jobs/generated/*.h, false) \
    $$PWD/logging.h \
    $$PWD/settings.h \
    $$PWD/networksettings.h \
    $$PWD/networkaccessmanager.h \
    $$PWD/jobs/downloadfilejob.h \
    $$PWD/jobs/postreadmarkersjob.h

SOURCES += \
    $$PWD/connectiondata.cpp \
    $$PWD/connection.cpp \
    $$PWD/room.cpp \
    $$PWD/user.cpp \
    $$PWD/avatar.cpp \
    $$PWD/events/event.cpp \
    $$PWD/events/eventcontent.cpp \
    $$PWD/events/roommessageevent.cpp \
    $$PWD/events/roommemberevent.cpp \
    $$PWD/events/typingevent.cpp \
    $$PWD/events/receiptevent.cpp \
    $$PWD/jobs/requestdata.cpp \
    $$PWD/jobs/basejob.cpp \
    $$PWD/jobs/checkauthmethods.cpp \
    $$PWD/jobs/passwordlogin.cpp \
    $$PWD/jobs/sendeventjob.cpp \
    $$PWD/jobs/postreceiptjob.cpp \
    $$PWD/jobs/joinroomjob.cpp \
    $$PWD/jobs/roommessagesjob.cpp \
    $$PWD/jobs/syncjob.cpp \
    $$PWD/jobs/mediathumbnailjob.cpp \
    $$PWD/jobs/setroomstatejob.cpp \
    $$files($$PWD/jobs/generated/*.cpp, false) \
    $$PWD/logging.cpp \
    $$PWD/settings.cpp \
    $$PWD/networksettings.cpp \
    $$PWD/networkaccessmanager.cpp \
    $$PWD/jobs/downloadfilejob.cpp
