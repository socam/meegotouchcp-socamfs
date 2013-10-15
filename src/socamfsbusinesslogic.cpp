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
#include "socamfsbusinesslogic.h"
#include <QFile>
#include <QTimer>
#include <QSystemInfo>
#include <QSystemDeviceInfo>

QTM_USE_NAMESPACE

#include <QDir>
#include <QRegExp>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QtDBus>
#include <QProcess>
#include <QDBusInterface>
#include <QDBusObjectPath>

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>

#define OS_NAME_FALLBACK "MeeGo"

#define DEBUG
#define WARNING
#include "debug.h"


SocamFSBusinessLogic::SocamFSBusinessLogic() : QThread (0),
    m_generator (0)
{
    setTerminationEnabled (true);
    settings = new SocamFSSettings();

    //Profiles
    m_ProfileList.push_back("SMARTPHONE");
    m_ProfileList.push_back("NETBOOK");
    m_ProfileList.push_back("SMARTTV");
    m_ProfileList.push_back("EMBEDDED");

    //Log levels
    m_LogLevelList.push_back("TRACE");
    m_LogLevelList.push_back("DEBUG");
    m_LogLevelList.push_back("INFO");
    m_LogLevelList.push_back("WARN");
    m_LogLevelList.push_back("ERROR");
    m_LogLevelList.push_back("FATAL");

    logLevels.insert(std::pair<std::string,std::string>("TRACE","1"));
    logLevels.insert(std::pair<std::string,std::string>("DEBUG","2"));
    logLevels.insert(std::pair<std::string,std::string>("INFO","3"));
    logLevels.insert(std::pair<std::string,std::string>("WARN","4"));
    logLevels.insert(std::pair<std::string,std::string>("ERROR","5"));
    logLevels.insert(std::pair<std::string,std::string>("FATAL","6"));
    logLevels.insert(std::pair<std::string,std::string>("1","TRACE"));
    logLevels.insert(std::pair<std::string,std::string>("2","DEBUG"));
    logLevels.insert(std::pair<std::string,std::string>("3","INFO"));
    logLevels.insert(std::pair<std::string,std::string>("4","WARN"));
    logLevels.insert(std::pair<std::string,std::string>("5","ERROR"));
    logLevels.insert(std::pair<std::string,std::string>("6","FATAL"));

}

SocamFSBusinessLogic::~SocamFSBusinessLogic()
{
    delete m_generator;
    m_generator = 0;
}



void
SocamFSBusinessLogic::run ()
{
    setPriority (QThread::LowPriority);
}



/*!
 * Returns reference to a map from log level ids to translation ids.
 */
list<string> SocamFSBusinessLogic::getLogLevelValues () {
	return m_LogLevelList;
}


/*!
 * Returns reference to a map from profile ids to translation ids.
 */
list<string> SocamFSBusinessLogic::getProfileValues ()
{
    return m_ProfileList;
}



string SocamFSBusinessLogic::getLogLevel() {
	return logLevels[settings->getValue(SOCAMFS_LOG_LEVEL)];
}



void SocamFSBusinessLogic::setLogLevel(string value) {
	settings->setValue(SOCAMFS_LOG_LEVEL, logLevels[value]);
}


string SocamFSBusinessLogic::getProfile(){
	return settings->getValue(SOCAMFS_PROFILE);
}


void SocamFSBusinessLogic::setProfile(string value){
	settings->setValue(SOCAMFS_PROFILE, value);
}


int SocamFSBusinessLogic::getCatalogUpdatePeriod(){
	string value = settings->getValue(SOCAMFS_UPDATE_CATALOG_PERIOD);
	int period = atoi(value.c_str());
	if(period<=0) 
		period = DEFAULT_CATALOG_UPDATE_PERIOD;
	return period;
}


void SocamFSBusinessLogic::setCatalogUpdatePeriod(int value){
	ostringstream oss;
	oss<<value;
	settings->setValue(SOCAMFS_UPDATE_CATALOG_PERIOD, oss.str());
}


int SocamFSBusinessLogic::getCleanInterval(){
	string value = settings->getValue(SOCAMFS_CLEAN_INTERVAL);
	int interval = atoi(value.c_str());
	if(interval<=0) 
		interval = DEFAULT_CLEAN_INTERVAL;
	return interval;
}

void SocamFSBusinessLogic::setCleanInterval(int value){
	ostringstream oss;
	oss<<value;
	settings->setValue(SOCAMFS_CLEAN_INTERVAL, oss.str());
}



int SocamFSBusinessLogic::getMaxFilesToPredict(){
	string value = settings->getValue(SOCAMFS_MAX_FILES_TO_PREDICT);
	int maxFiles=atoi(value.c_str());
	if(maxFiles<=0) 
		maxFiles = DEFAULT_MAX_FILES_TO_PREDICT;
	return maxFiles;
}



void SocamFSBusinessLogic::setMaxFilesToPredict(int value){
	ostringstream oss;
	oss<<value;
	settings->setValue(SOCAMFS_MAX_FILES_TO_PREDICT, oss.str());
}


unsigned long long SocamFSBusinessLogic::getDiskSpace( const char* devPath) {
	bool retVal = false;
	struct statfs fs;
	if( ( statfs( devPath, &fs ) ) < 0 ) {
	    printf( "Failed to stat %s: %s\n", devPath ,strerror( errno ) );
	    return 0;
	} else {
	    return fs.f_blocks * fs.f_bsize;
	}
}

unsigned long long SocamFSBusinessLogic::getMaxFileSystemSize() {
	struct mntent *ent;
	FILE *aFile;
	unsigned long long size = 0;

	aFile = setmntent("/proc/mounts", "r");

	if (aFile == NULL) {
		perror("setmntent");
		exit(1);
	}

	while (NULL != (ent = getmntent(aFile))) {
		if(!strcmp(ent->mnt_fsname, ROOTFS)) {
			size = getDiskSpace(ent->mnt_dir);
			break;
		}
	}

	endmntent(aFile);
	cout << "SocamFSBusinessLogic::getMaxFileSystemSize=" << size << endl;
	return size / 1024;
}


unsigned long long SocamFSBusinessLogic::getFileSystemSize(){
	string value = settings->getValue(SOCAMFS_FILE_SYSTEM_SIZE);
	unsigned long long size=atoll(value.c_str());
	if(size<=0) 
		size = FILE_SYSTEM_SIZE_MIN;
	return size;
}


void SocamFSBusinessLogic::setFileSystemSize(unsigned long long value){
        cout << "setFileSystemSize:" << value << endl; 
	ostringstream oss;
	oss<<value;
	settings->setValue(SOCAMFS_FILE_SYSTEM_SIZE, oss.str());
}


unsigned long long SocamFSBusinessLogic::getMinFreeDisk() {
	string value = settings->getValue(SOCAMFS_MIN_FREE_DISK);
	unsigned long long freeDisk=atoll(value.c_str());
	if(freeDisk<=0) 
		freeDisk = 0;
	return freeDisk;
}


void SocamFSBusinessLogic::setMinFreeDisk(unsigned long long value){
	ostringstream oss;
	oss<<value;
	settings->setValue(SOCAMFS_MIN_FREE_DISK, oss.str());
}


float SocamFSBusinessLogic::getDependencyThreshold(){
	string value = settings->getValue(SOCAMFS_DEPENDENCY_THRESHOLD);
	float threshold=atof(value.c_str());
	if(threshold<0) 
		threshold=0;
	else if(threshold>1) 
		threshold=1;
	return threshold;

}


void SocamFSBusinessLogic::setDependencyThreshold(float value){
	ostringstream oss;
	oss<<value;
	settings->setValue(SOCAMFS_DEPENDENCY_THRESHOLD, oss.str());
}


string SocamFSBusinessLogic::getURL(){
	return settings->getValue(SOCAMFS_URL_SERVER);

}

  
void SocamFSBusinessLogic::setURL(string value){
	settings->setValue(SOCAMFS_URL_SERVER, value);
}


string SocamFSBusinessLogic::getUser(){
	return settings->getValue(SOCAMFS_USER);

}


void SocamFSBusinessLogic::setUser(string value){
	settings->setValue(SOCAMFS_USER, value);

}


string SocamFSBusinessLogic::getPassword(){
	return settings->getValue(SOCAMFS_PASSWORD );
}


void SocamFSBusinessLogic::setPassword(string password){
	settings->setValue(SOCAMFS_PASSWORD, password);
}


void SocamFSBusinessLogic::save() {
	settings->save();
}















