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
#ifndef SocamFSBusinessLogic_H
#define SocamFSBusinessLogic_H

#include <stdio.h>
#include <stdlib.h>
#include <QMap>
#include <QList>
#include <QString>
#include <QStringList>
#include <QPointer>
#include <QImage>
#include <QThread>
#include <QVariant>
#include <sstream>
#include <SocamFSSettings.h>
#include <stdio.h>
#include <errno.h>
#include <sys/statfs.h>
#include <mntent.h>


#include <qmdisplaystate.h>
#include <qmdevicemode.h>
#include <QObject>

#define ROOTFS "rootfs"
#define DEFAULT_MAX_FILES_TO_PREDICT 1
#define CLEAN_INTERVAL_MIN 1 // 1 MIN
#define CLEAN_INTERVAL_MAX 1440 // 1 DAY
#define CLEAN_INTERVAL_UNIT 60 // 1 DAY
#define DEFAULT_CLEAN_INTERVAL 3600 // 1 HOUR
#define CATALOG_UPDATE_PERIOD_MIN 1 // 1 MIN
#define CATALOG_UPDATE_PERIOD_MAX 1440 // 1 DAY
#define CATALOG_UPDATE_PERIOD_UNIT 60 // 1 DAY
#define DEFAULT_CATALOG_UPDATE_PERIOD 60 // 1 HOUR
#define FILE_SYSTEM_SIZE_MIN 1024 // 1 MB
#define FILE_SYSTEM_SIZE_UNIT 1024 // KB
#define MIN_FREE_DISK_UNIT 1 // MB
#define DEPENDENCY_THRESHOLD_MIN 0
#define DEPENDENCY_THRESHOLD_MAX 100
#define DEPENDENCY_THRESHOLD_FACTOR 100
#define MAX_FILES_TO_PREDICT_MIN 0
#define MAX_FILES_TO_PREDICT_MAX 10
#define DEFAULT_MAX_FILES_TO_PREDICT 0

using namespace std;

class QProcess;


class SocamFSBusinessLogic : public QThread
{
    Q_OBJECT

public:
    SocamFSBusinessLogic ();
    ~SocamFSBusinessLogic ();

    string getLogLevel();
    list<string> getLogLevelValues ();
    void setLogLevel(string value);

    string getProfile();
    list<string> getProfileValues ();
    void setProfile(string value);

    int getCatalogUpdatePeriod();
    void setCatalogUpdatePeriod(int value);

    int getMaxFilesToPredict();
    void setMaxFilesToPredict(int value);

    int getCleanInterval();
    void setCleanInterval(int value);

    /** Get total size of rootfs partition in KB */
    unsigned long long getMaxFileSystemSize();
    unsigned long long getFileSystemSize();
    void setFileSystemSize(unsigned long long value);
    unsigned long long getMinFreeDisk();
    void setMinFreeDisk(unsigned long long value);
    float getDependencyThreshold();
    void setDependencyThreshold(float value);
    
    string getURL();
    void setURL(string value);
    string getUser();
    void setUser(string value);
    string getPassword();
    void setPassword(string value);

    void save();


protected:
    virtual void run ();

private:

    SocamFSSettings* settings;
    list<string>   m_LogLevelList;
    list<string>   m_ProfileList;
    QProcess *m_generator;
    std::map<std::string, std::string> logLevels;

    unsigned long long getDiskSpace(const char* devPath);


#ifdef UNIT_TEST
    friend class Ut_SocamFSBusinessLogic;
    friend class Ut_SocamFSApplet;
#endif
};


#endif
