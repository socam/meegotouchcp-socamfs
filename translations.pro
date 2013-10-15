

#LANGUAGES =            # empty if only engineering English is needed!
TRANSLATIONS = socamfsapplet.ts
CATALOGNAME = socamfsapplet
SOURCES =                   \
    src/debug.cpp          \
    src/logowidget.cpp          \
    src/socamfsbusinesslogic.cpp  \
    src/socamfsapplet.cpp         \
    src/socamfswidget.cpp
SOURCEDIR = $$PWD/src
TRANSLATIONDIR = $$PWD
TRANSLATION_INSTALLDIR = /usr/share/l10n/meegotouch
include($$[QT_INSTALL_DATA]/mkspecs/features/meegotouch_defines.prf)
include($$[QT_INSTALL_DATA]/mkspecs/features/meegotouch_translations.prf)

# hack around again?
LRELEASE_OPTIONS -= -markuntranslated '!!\\ '


