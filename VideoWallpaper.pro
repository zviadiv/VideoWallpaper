!win32: error("This project only supports Win32 platform!")
isEmpty(ROOT): ROOT = $$PWD
TARGET = VideoWallpaper
BIN_DIR = $$ROOT/bin
contains(QT_ARCH, x86_64) {
    BIN_DIR = $$join(BIN_DIR,,,64)
    TARGET = $$join(TARGET,,,64)
}
CONFIG(debug, debug|release): TARGET = $$join(TARGET,,,d)
QT += quick gui widgets svg av avwidgets
TEMPLATE = app
DEFINES += QT_DEPRECATED_WARNINGS QT_DISABLE_DEPRECATED_BEFORE=0x050603
CONFIG *= c++11
CONFIG -= app_bundle
LIBS += -lUser32 -lDwmapi
CONFIG(release, debug|release): LIBS += -lQt5AV -lQt5AVWidgets
CONFIG(debug, debug|release): LIBS += -lQt5AVd -lQt5AVWidgetsd

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Refer to the documentation for the
# deprecated API to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        desktopvideoplayer.cpp \
        main.cpp \
        application.cpp \
        overlayfiler.cpp \
        settingsmanager.cpp \
        videowallpaperviewcontroller.cpp

RESOURCES += qml.qrc

win32 {
    QMAKE_TARGET_PRODUCT     = Desktop Wallpaper
    QMAKE_TARGET_DESCRIPTION = App that makes your desktop alive
    QMAKE_TARGET_COMPANY     = mylivewallpapers.com
    QMAKE_TARGET_COPYRIGHT   = Copyright © mylivewallpapers.com. All Rights Reserved.
    RC_ICONS                 = appicon.ico
}

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

target.path = $$BIN_DIR

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    application.h \
    desktopvideoplayer.h \
    overlayfilter.h \
    settingsmanager.h \
    videowallpaperviewcontroller.h

FORMS +=

    libs.path = $$BIN_DIR
    libs.files = \
        $$[QT_INSTALL_BINS]/QtAV*.dll \
        $$[QT_INSTALL_BINS]/avcodec-*.dll \
        $$[QT_INSTALL_BINS]/avdevice-*.dll \
        $$[QT_INSTALL_BINS]/avfilter-*.dll \
        $$[QT_INSTALL_BINS]/avformat-*.dll \
        $$[QT_INSTALL_BINS]/avresample-*.dll \
        $$[QT_INSTALL_BINS]/avutil-*.dll \
        $$[QT_INSTALL_BINS]/ass.dll \
        $$[QT_INSTALL_BINS]/libass.dll \
        $$[QT_INSTALL_BINS]/OpenAL32*.dll \
        $$[QT_INSTALL_BINS]/libEGL.dll \
        $$[QT_INSTALL_BINS]/libGLESv2.dll \
        $$[QT_INSTALL_BINS]/postproc-*.dll \
        $$[QT_INSTALL_BINS]/swresample-*.dll \
        $$[QT_INSTALL_BINS]/swscale-*.dll
    isEmpty(windeployqt): windeployqt = $$[QT_INSTALL_BINS]/windeployqt.exe
    exists("$${windeployqt}") {
        libs.commands = $$quote(\"$${windeployqt}\" --plugindir \"$${BIN_DIR}/plugins\" --force --no-translations --no-system-d3d-compiler --compiler-runtime --no-angle --no-opengl-sw -opengl --qmldir \"$${ROOT}\" --no-qmltooling --no-quickwidgets --no-svg --list source \"$${BIN_DIR}/$${TARGET}.exe\")
        libs.commands = $$join(libs.commands, $$escape_expand(\\n\\t))
    }
    INSTALLS += libs
