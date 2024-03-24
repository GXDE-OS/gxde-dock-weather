#include(../dde-dock/interfaces/interfaces.pri)

QT              += widgets network #svg
TEMPLATE         = lib
CONFIG          += plugin c++11 link_pkgconfig
PKGCONFIG       += dtkwidget

TARGET           = HTYWeather
DISTFILES       += weather.json

HEADERS += \
    weatherplugin.h \
    weatherwidget.h \
    forcastwidget.h

SOURCES += \
    weatherplugin.cpp \
    weatherwidget.cpp \
    forcastwidget.cpp

CONFIG(release, debug|release) {
    TRANSLATIONS = $$files($$PWD/translations/*.ts)
    #遍历目录中的ts文件，调用lrelease将其生成为qm文件
    for(tsfile, TRANSLATIONS) {
        qmfile = $$replace(tsfile, .ts$, .qm)
        system(lrelease $$tsfile -qm $$qmfile) | error("Failed to lrelease")
    }
}

target.path = $${PREFIX}/lib/dde-dock/plugins/
INSTALLS += target

RESOURCES += res.qrc

TRANSLATIONS += translations/zh_CN.ts

APPSHAREDIR = /usr/share/$$TARGET
unix:!android: target.path = /usr/lib/dde-dock/plugins

translations.path = $$APPSHAREDIR/translations
translations.files = $$PWD/translations/*.qm

!isEmpty(target.path): INSTALLS += target translations
