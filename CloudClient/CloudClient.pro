QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Forget.cpp \
    Information.cpp \
    Logon.cpp \
    Network.cpp \
    ReceiveWork.cpp \
    SendWork.cpp \
    TaskWork.cpp \
    Work.cpp \
    main.cpp \
    CloudClient.cpp \
    Signup.cpp \
    OperatorWidget.cpp \
    HomePage.cpp \
    PersonalPage.cpp \
    protocol.cpp

HEADERS += \
    CloudClient.h \
    Forget.h \
    Information.h \
    Logon.h \
    Network.h \
    ReceiveWork.h \
    SendWork.h \
    Signup.h \
    OperatorWidget.h \
    HomePage.h \
    PersonalPage.h \
    TaskWork.h \
    Work.h \
    protocol.h

FORMS += \
    CloudClient.ui \
    Forget.ui \
    HomePage.ui \
    Logon.ui \
    OperatorWidget.ui \
    PersonalPage.ui \
    Signup.ui



# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
RC_ICONS=images/favicon.ico
