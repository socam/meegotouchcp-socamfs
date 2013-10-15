/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (directui@nokia.com)
**
** This file is part of meegotouch-controlpanelapplets.
**
** If you have questions regarding the use of this file, please contact
** Nokia at directui@nokia.com.
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation
** and appearing in the file LICENSE.LGPL included in the packaging
** of this file.
**
****************************************************************************/
#ifndef SOCAMFSAPPLET_H
#define SOCAMFSAPPLET_H

#include "socamfswidget.h"

#include <dcpappletif.h>
#include <QObject>
#include <QPointer>

class SocamFSWidget;

class Q_DECL_EXPORT SocamFSApplet : public QObject, public DcpAppletIf
{
    Q_OBJECT
    Q_INTERFACES (DcpAppletIf)

public:
    SocamFSApplet ();
    ~SocamFSApplet ();

    virtual void init ();
    virtual DcpWidget* constructWidget (int widgetId);
    virtual QString title () const;
    virtual QVector<MAction *> viewMenuItems ();

private:
    QPointer<SocamFSWidget>        m_MainWidget;

    #ifdef UNIT_TEST
    friend class Ut_SocamFSApplet;
    #endif
};
#endif
