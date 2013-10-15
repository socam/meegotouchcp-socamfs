

TEMPLATE = lib
CONFIG += plugin \
          gui \
          meegotouchcore \
          mobility \
          duicontrolpanel \
          link_pkgconfig \
          qmsystem2


QT += dbus gui
MOBILITY += systeminfo

MOC_DIR = .moc
OBJECTS_DIR = .objects

QMAKE_LFLAGS_RPATH = -Wl
QMAKE_CXXFLAGS += -fvisibility=hidden -fvisibility-inlines-hidden -fPIC

HEADERS =                   \
    src/debug.h            \
    src/dcpdisplay.h            \
    src/logowidget.h            \
    src/socamfsbusinesslogic.h    \
    src/socamfsapplet.h           \
    src/socamfswidget.h

SOURCES =                   \
    src/debug.cpp          \
    src/logowidget.cpp          \
    src/socamfsbusinesslogic.cpp  \
    src/socamfsapplet.cpp         \
    src/socamfswidget.cpp


INCLUDEPATH += /usr/include/socam/socamfs

DESTDIR = lib
desktop.files += *.desktop
desktop.path = $$(DEBIAN_DESTDIR)/usr/lib/duicontrolpanel

css.files = libsocamfsapplet.css
css.path += $$(DEBIAN_DESTDIR)/usr/share/themes/base/meegotouch/libsocamfsapplet/style

target.path += $$(DEBIAN_DESTDIR)$$[QT_INSTALL_LIBS]/duicontrolpanel/applets

INSTALLS += target  \
            desktop \
            css

LIBS += -lSocamOSApi






