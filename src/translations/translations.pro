
warning(TRANSLAAAAAAATIONS: $$PWD)


CATALOGNAME = socamfsapplet

SOURCEDIR = ..
TRANSLATIONDIR = $$PWD
TRANSLATION_INSTALLDIR = /usr/share/l10n/meegotouch
include($$[QT_INSTALL_DATA]/mkspecs/features/meegotouch_defines.prf)
include($$[QT_INSTALL_DATA]/mkspecs/features/meegotouch_translations.prf)

# hack around again?
LRELEASE_OPTIONS -= -markuntranslated '!!\\ '

