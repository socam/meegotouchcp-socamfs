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
#include "socamfswidget.h"
#include "socamfsbusinesslogic.h"
#include "styles.h"
#include "dcpdisplay.h"

#undef DEBUG
#define WARNING
#include "debug.h"

/*
 * define this if you want to show the top-close switch
 */
#undef TOP_CLOSE_ENABLED

#include <QVariant>
#include <MButton>
#include <MWidgetController>
#include <MComboBox>
#include <MLabel>
#include <MLayout>
#include <MSlider>
#include <MLinearLayoutPolicy>
#include <QGraphicsLinearLayout>
#include <MSeparator>
#include <MHelpButton>
#include <MGConfItem>

//TODO REMOVE
#include <iostream>
using namespace std;

SocamFSWidget::SocamFSWidget (QGraphicsWidget *parent) :
        DcpWidget (parent),
        m_logic (0),
        m_MainLayout (0),
        m_TitleLabel (0),
        m_SubTitleLabel (0),
        m_logLevelsCBox (0)
{
    setReferer (DcpDisplay::None);
    setContentsMargins (0, 0, 0, 0);
    m_logic = new SocamFSBusinessLogic;

    /*
     * Lets have these configurable via gconf keys
     */
    MGConfItem eggs(EGGS_GCONF_KEY);

    if (eggs.value().toBool()) {
        m_topCloseChangeable = true;
    } else {
        
        MGConfItem tcs ("/meegotouch/settings/top_swipe_switchable");
        m_topCloseChangeable = tcs.value (false).toBool ();
    }

    initWidget ();
}

SocamFSWidget::~SocamFSWidget ()
{
    delete m_logic;
    m_logic = 0;
}

void SocamFSWidget::initWidget ()
{

    rows = 0;
    MLayout     *layout;

    /*
     * Creating a layout that holds the rows of the internal widgets.
     */
    layout = new MLayout (this);
    m_MainLayout = new MLinearLayoutPolicy (layout, Qt::Vertical);
    m_MainLayout->setContentsMargins (0., 0., 0., 0.);
    m_MainLayout->setSpacing (0.);
    setLayout (layout);

    addProfilesCBox ();

    //URL
    addTextBox(m_UrlLabel, m_UrlBox, QT_TRID_NOOP("qtn_socamfs_url"), m_logic->getURL().c_str());	

    //User
    addTextBox(m_UserLabel, m_UserBox, QT_TRID_NOOP("qtn_socamfs_user"), m_logic->getUser().c_str());

    //Password
    addTextBox(m_PasswordLabel, m_PasswordBox, QT_TRID_NOOP("qtn_socamfs_password"), m_logic->getPassword().c_str());

    //% "day"
    txtDay =    string(qtTrId(QT_TRID_NOOP("qtn_socamfs_day")).toUtf8().data());
    txtHour =   string(qtTrId(QT_TRID_NOOP("qtn_socamfs_hour")).toUtf8().data());
    txtMinute = string(qtTrId(QT_TRID_NOOP("qtn_socamfs_minute")).toUtf8().data());

    //Catalog update period
    int period = m_logic->getCatalogUpdatePeriod()/CATALOG_UPDATE_PERIOD_UNIT;
    if (period < CATALOG_UPDATE_PERIOD_MIN) {
	period = CATALOG_UPDATE_PERIOD_MIN;
    }
    if (period > CATALOG_UPDATE_PERIOD_MAX) {
	period = CATALOG_UPDATE_PERIOD_MAX;
    }
    m_CatalogPeriodLabelText = string(qtTrId(QT_TRID_NOOP("qtn_socamfs_catalog_update_period")).toUtf8().data()) + SLIDER_VALUE_SEPARATOR + translateTime(period);  
    addSlider(m_CatalogPeriodLabel, m_CatalogPeriodSlider, m_CatalogPeriodLabelText.c_str(), 
        CATALOG_UPDATE_PERIOD_MIN, CATALOG_UPDATE_PERIOD_MAX, period);
    
    //Clean interval
    int interval = m_logic->getCleanInterval()/CLEAN_INTERVAL_UNIT;
    if (interval < CLEAN_INTERVAL_MIN) {
	interval = CLEAN_INTERVAL_MIN;
    }
    if (interval > CLEAN_INTERVAL_MAX) {
	interval = CLEAN_INTERVAL_MAX;
    }
    m_CleanIntervalLabelText = string(qtTrId(QT_TRID_NOOP("qtn_socamfs_clean_interval")).toUtf8().data()) + SLIDER_VALUE_SEPARATOR + translateTime(interval); 
    addSlider(m_CleanIntervalLabel, m_CleanIntervalSlider, 
        m_CleanIntervalLabelText.c_str(), 
        CLEAN_INTERVAL_MIN, CLEAN_INTERVAL_MAX, 
        interval);

    //File system size
    unsigned long long minSize = FILE_SYSTEM_SIZE_MIN/FILE_SYSTEM_SIZE_UNIT; // 1024 KB = 1MB
    unsigned long long maxSize = m_logic->getMaxFileSystemSize() / FILE_SYSTEM_SIZE_UNIT; // (B -> KB)
    unsigned long long size = m_logic->getFileSystemSize()/FILE_SYSTEM_SIZE_UNIT;
    if (minSize > maxSize) {
	minSize = maxSize;
    }
    if (size > maxSize) {
	size = maxSize;
    }
    if (size < minSize) {
	size = minSize;
    }
    
    m_FileSystemSizeLabelText = string(qtTrId(QT_TRID_NOOP("qtn_socamfs_file_system_size")).toUtf8().data()) + SLIDER_VALUE_SEPARATOR + sizeToText(size, MB); 
    addSlider(m_FileSystemSizeLabel, m_FileSystemSizeSlider, m_FileSystemSizeLabelText.c_str(), 
        minSize, maxSize, size);

    //Min free disk
    unsigned long long minFreeDisk = m_logic->getMinFreeDisk()/MIN_FREE_DISK_UNIT;
    if(minFreeDisk>size) {
	minFreeDisk = size;
    }
    m_MinFreeDiskLabelText = string(qtTrId(QT_TRID_NOOP("qtn_socamfs_min_free_disk")).toUtf8().data()) +  SLIDER_VALUE_SEPARATOR + sizeToText(minFreeDisk, MB); 
    addSlider(m_MinFreeDiskLabel, m_MinFreeDiskSlider, 
        m_MinFreeDiskLabelText.c_str(), 
        0, maxSize*MIN_FREE_DISK_UNIT/FILE_SYSTEM_SIZE_UNIT, 
        minFreeDisk);

    //Dependency threshold
    float dependencyThreshold = m_logic->getDependencyThreshold();
    ostringstream m_DependencyThresholdLabelOSS;
    m_DependencyThresholdLabelOSS << qtTrId(QT_TRID_NOOP("qtn_socamfs_dependency_threshold")).toUtf8().data() << SLIDER_VALUE_SEPARATOR << dependencyThreshold;
    m_DependencyThresholdLabelText = m_DependencyThresholdLabelOSS.str();
    addSlider(m_DependencyThresholdLabel, m_DependencyThresholdSlider, 
        m_DependencyThresholdLabelText.c_str(), 
        DEPENDENCY_THRESHOLD_MIN, DEPENDENCY_THRESHOLD_MAX, 
        dependencyThreshold*DEPENDENCY_THRESHOLD_FACTOR);

    //Max files to predict
    int maxFilesToPredict = m_logic->getMaxFilesToPredict();
    if(maxFilesToPredict > MAX_FILES_TO_PREDICT_MAX) {
	maxFilesToPredict = MAX_FILES_TO_PREDICT_MAX;
    }
    if(maxFilesToPredict < MAX_FILES_TO_PREDICT_MIN) {
	maxFilesToPredict = MAX_FILES_TO_PREDICT_MIN;
    }
    ostringstream m_MaxFileToPredictLabelStr;
    m_MaxFileToPredictLabelStr << qtTrId(QT_TRID_NOOP("qtn_socamfs_max_files_to_predict")).toUtf8().data() << SLIDER_VALUE_SEPARATOR << maxFilesToPredict;
    m_MaxFileToPredictLabelText = m_MaxFileToPredictLabelStr.str();
    addSlider(m_MaxFileToPredictLabel, m_MaxFileToPredictSlider, 
        m_MaxFileToPredictLabelText.c_str(), 
        MAX_FILES_TO_PREDICT_MIN, MAX_FILES_TO_PREDICT_MAX, maxFilesToPredict);

    addLogLevelsCBox ();

    //Buttons
    m_SaveButton = new MButton();
    m_SaveButton->setText(qtTrId ("qtn_socamfs_save"));
    m_MainLayout->addItem(m_SaveButton);

    m_MainLayout->addStretch();

    QObject::connect(m_SaveButton, SIGNAL(clicked()), this, SLOT(saveButtonPressed()));
    QObject::connect(m_CatalogPeriodSlider, SIGNAL(valueChanged(int)), this, SLOT(catalogPeriodSliderChanged(int)));
    QObject::connect(m_CleanIntervalSlider, SIGNAL(valueChanged(int)), this, SLOT(cleanIntervalSliderChanged(int)));
    QObject::connect(m_FileSystemSizeSlider, SIGNAL(valueChanged(int)), this, SLOT(fileSystemSizeSliderChanged(int)));
    QObject::connect(m_MinFreeDiskSlider, SIGNAL(valueChanged(int)), this, SLOT(minFreeDiskSliderChanged(int)));
    QObject::connect(m_DependencyThresholdSlider, SIGNAL(valueChanged(int)), this, SLOT(dependencyThresholdSliderChanged(int)));
    QObject::connect(m_MaxFileToPredictSlider, SIGNAL(valueChanged(int)), this, SLOT(maxFileToPredictSliderChanged(int)));

}


void
SocamFSWidget::retranslateUi ()
{
}


void SocamFSWidget::addTextBox(MLabel* &label, MTextEdit* &textBox, const char* name, const char* text) {

    label = new MLabel();
    label->setWordWrap (true);

    textBox = new MTextEdit(MTextEditModel::SingleLine, QString(), this);
    textBox->setText(text);

    m_MainLayout->addItem (label);
    label->setColor(QColor(222,222,222,255));
    label->setText (qtTrId (name));

    m_MainLayout->addItem(textBox);
    m_MainLayout->setAlignment(textBox, Qt::AlignTop);
    m_MainLayout->setItemSpacing(rows++,ROW_SPACING_SHORT);  
    m_MainLayout->setItemSpacing(rows++,ROW_SPACING);

}


void SocamFSWidget::addSlider(MLabel* &label, MSlider* &slider, const char* name, unsigned long long min, unsigned long long max, int value) {

    label = new MLabel();
    label->setText(name);
    label->setWordWrap (true);
    m_MainLayout->addItem (label);
    label->setColor(QColor(222,222,222,255));
    if (label->text().size()==0) {
    	label->setText (qtTrId (name));
    }

    slider = new MSlider;
    slider->setContentsMargins (0,0,0,0);
    slider->setStyleName ("CommonSliderInverted");
    slider->setObjectName (name);
    
    slider->setRange (min, max);
    slider->setValue (value);
    slider->setHandleLabelVisible(true);

    m_MainLayout->addItem(slider);    
    m_MainLayout->setItemSpacing(rows++,ROW_SPACING_SHORT);  
    m_MainLayout->setItemSpacing(rows++,ROW_SPACING);

}

/*
 * Construct and init profiles selector
 */
void
SocamFSWidget::addLogLevelsCBox ()
{

    /*
     * Get the values from the business-logic
     */
    m_logLevelsCBox = new MComboBox;

    m_logLevelsCBox->setTitle (qtTrId ("qtn_socamfs_log_level"));
    m_logLevelsCBox->setStyleName ("CommonComboBoxInverted");
    m_logLevelsCBox->setLayoutPosition (M::VerticalCenterPosition);

    //Fill data
    list<string> logLevels = m_logic->getLogLevelValues();
    string level = m_logic->getLogLevel();
    bool valueFound = false;

    int i = 0;
    for (std::list<string>::const_iterator iterator = logLevels.begin(), end = logLevels.end(); iterator != end; ++iterator) {
        string text = *iterator;
        QString itemText = QString(text.c_str());
        m_logLevelsCBox->insertItem (i, itemText);
	if(!strcmp(level.c_str(),text.c_str())) { 
		m_logLevelsCBox->setCurrentIndex(i);
		valueFound = true;
	}
	i++;
    }

    if(!valueFound) {
        m_logLevelsCBox->setCurrentIndex(0);
    }

    m_MainLayout->addItem (m_logLevelsCBox);
    m_MainLayout->setStretchFactor (m_logLevelsCBox, 0);
    m_MainLayout->setItemSpacing(rows,ROW_SPACING);
    rows++;

}



/*
 * Construct and init profiles selector
 */
void
SocamFSWidget::addProfilesCBox ()
{

    /*
     * Get the values from the business-logic
     */
    m_profilesCBox = new MComboBox;

    m_profilesCBox->setTitle (qtTrId ("qtn_socamfs_profile"));
    m_profilesCBox->setStyleName ("CommonComboBoxInverted");
    m_profilesCBox->setLayoutPosition (M::VerticalCenterPosition);

    //Fill data
    list<string> profiles = m_logic->getProfileValues();
    string profile = m_logic->getProfile();
    bool valueFound = false;

    int i = 0;
    for (std::list<string>::const_iterator iterator = profiles.begin(), end = profiles.end(); iterator != end; ++iterator) {
        string text = *iterator;
        QString itemText = QString(text.c_str());
        m_profilesCBox->insertItem (i, itemText);
	if(!strcmp(profile.c_str(),text.c_str())) { 
		m_profilesCBox->setCurrentIndex(i);
		valueFound = true;
	}
	i++;
    }

    if(!valueFound) {
        m_profilesCBox->setCurrentIndex(0);
    }

    m_MainLayout->addItem (m_profilesCBox);
    m_MainLayout->setStretchFactor (m_profilesCBox, 0);
    m_MainLayout->setItemSpacing(rows,ROW_SPACING);
    rows++;

}

/**
* Catalog update period value changed: update handle and label text 
*/
void SocamFSWidget::catalogPeriodSliderChanged(int value) {
	m_CatalogPeriodSlider->setHandleLabel(translateTime(value).c_str());
	m_CatalogPeriodSlider->setHandleLabelVisible(true);
	m_CatalogPeriodLabelText = string(qtTrId(QT_TRID_NOOP("qtn_socamfs_catalog_update_period")).toUtf8().data());
	m_CatalogPeriodLabelText += ": " + translateTime(value);	
	m_CatalogPeriodLabel->setText(m_CatalogPeriodLabelText.c_str());
}

/**
* Clean interval value changed: update handle and label text 
*/
void SocamFSWidget::cleanIntervalSliderChanged(int value) {
	m_CleanIntervalSlider->setHandleLabel(translateTime(value).c_str());
	m_CleanIntervalSlider->setHandleLabelVisible(true);
	m_CleanIntervalLabelText = string(qtTrId(QT_TRID_NOOP("qtn_socamfs_clean_interval")).toUtf8().data());
	m_CleanIntervalLabelText += ": " + translateTime(value); 
	m_CleanIntervalLabel->setText(m_CleanIntervalLabelText.c_str());

}

/**
* File system size changed: update handle and label text, update maximum value for "minimum free disk" 
*/
void SocamFSWidget::fileSystemSizeSliderChanged(int value) {
	m_FileSystemSizeSlider->setHandleLabel(sizeToText(value, MB).c_str());
	m_FileSystemSizeSlider->setHandleLabelVisible(true);

        unsigned long long fileSystemSize = m_FileSystemSizeSlider->value();
	m_MinFreeDiskSlider->setMaximum(fileSystemSize);
	if (m_MinFreeDiskSlider->value() > m_MinFreeDiskSlider->maximum()) {
		m_MinFreeDiskSlider->setValue(m_MinFreeDiskSlider->maximum());
	}

	m_FileSystemSizeLabelText = string(qtTrId(QT_TRID_NOOP("qtn_socamfs_file_system_size")).toUtf8().data());
	m_FileSystemSizeLabelText += ": " + sizeToText(value, MB); 


	m_FileSystemSizeLabel->setText(m_FileSystemSizeLabelText.c_str());

}

/**
* Minimum free disk value changed: update handle and label text 
*/
void SocamFSWidget::minFreeDiskSliderChanged(int value) {
	m_MinFreeDiskSlider->setHandleLabel(sizeToText(value, MB).c_str());
	m_MinFreeDiskSlider->setHandleLabelVisible(true);
	m_MinFreeDiskLabelText = string(qtTrId(QT_TRID_NOOP("qtn_socamfs_min_free_disk")).toUtf8().data());
	m_MinFreeDiskLabelText += ": " + sizeToText(value, MB); 
	m_MinFreeDiskLabel->setText(m_MinFreeDiskLabelText.c_str());
}

/**
* Dependency threshold value changed: update handle and label text 
*/
void SocamFSWidget::dependencyThresholdSliderChanged(int value) {
	ostringstream oss;
	oss << ((float)value)/DEPENDENCY_THRESHOLD_FACTOR;
	m_DependencyThresholdSlider->setHandleLabel(oss.str().c_str());
	m_DependencyThresholdSlider->setHandleLabelVisible(true);
	ostringstream m_DependencyThresholdLabelOSS;
	m_DependencyThresholdLabelOSS << qtTrId(QT_TRID_NOOP("qtn_socamfs_dependency_threshold")).toUtf8().data() << ": " << oss.str().c_str();
	m_DependencyThresholdLabelText = m_DependencyThresholdLabelOSS.str();
	m_DependencyThresholdLabel->setText(m_DependencyThresholdLabelText.c_str());
}

/**
* Maximum files to predict changed: update handle and label text 
*/
void SocamFSWidget::maxFileToPredictSliderChanged(int value) {
	ostringstream oss;
	oss << value;
	m_MaxFileToPredictSlider->setHandleLabel(oss.str().c_str());
	m_MaxFileToPredictSlider->setHandleLabelVisible(true);


	ostringstream m_MaxFileToPredictLabelStr;
	m_MaxFileToPredictLabelStr << qtTrId(QT_TRID_NOOP("qtn_socamfs_max_files_to_predict")).toUtf8().data() << ": " << value;
	m_MaxFileToPredictLabelText = m_MaxFileToPredictLabelStr.str();

	m_MaxFileToPredictLabel->setText(m_MaxFileToPredictLabelText.c_str());

}


/**
* Generate a formatted time string (e.g. 1d12h20m)
*/
string SocamFSWidget::translateTime(int minutes) {

	int days=0, hours=0, mins=minutes;

        mins = minutes%60;
	hours = ((minutes-mins)/60)%24;
	days = (minutes - mins - hours*60) / 1440;
	 
	ostringstream result;
        if(days>0) {
            result<<days<<txtDay;
        }
        if(hours>0) {
		result<<hours<<txtHour;
        }
        if(mins>0) {
		result<<mins<<txtMinute;
        }
	return result.str();

}


string SocamFSWidget::sizeToText(int size, SizeUnit unit) {

	char str[100];

	int sizeInKB = size;
	if (unit == MB) {
		sizeInKB *= 1024;
	} else if (unit == GB) {
		sizeInKB *= 1024*1024;
	}

	if(sizeInKB<1024) {
 	        sprintf (str, "%dKB", sizeInKB);

	} else if(sizeInKB<(1024*1024)) {
 	        sprintf (str, "%.2fMB",  ((float)sizeInKB)/1024   );
	} else {
 	        sprintf (str, "%.2fGB",  ((float)sizeInKB)/(1024*1024));
	}
	
	return str;

}


void SocamFSWidget::saveButtonPressed () {

	m_logic->setLogLevel(m_logLevelsCBox->currentText().toLatin1().data());
	m_logic->setProfile(m_profilesCBox->currentText().toLatin1().data());

	m_logic->setCatalogUpdatePeriod(m_CatalogPeriodSlider->value()*CATALOG_UPDATE_PERIOD_UNIT);
	m_logic->setCleanInterval(m_CleanIntervalSlider->value()*CLEAN_INTERVAL_UNIT);
	m_logic->setFileSystemSize(m_FileSystemSizeSlider->value()*FILE_SYSTEM_SIZE_UNIT);
	m_logic->setMinFreeDisk(m_MinFreeDiskSlider->value()*MIN_FREE_DISK_UNIT);
	float threshold = ((float)m_DependencyThresholdSlider->value())/DEPENDENCY_THRESHOLD_FACTOR;
	m_logic->setDependencyThreshold(threshold);
	m_logic->setMaxFilesToPredict(m_MaxFileToPredictSlider->value());

	m_logic->setURL(m_UrlBox->text().toUtf8().data());
	m_logic->setUser(m_UserBox->text().toUtf8().data());
	m_logic->setPassword(m_PasswordBox->text().toUtf8().data());


	m_logic->save();

	MInfoBanner *infoBanner = new MInfoBanner(MInfoBanner::Event);
	string txt(qtTrId(QT_TRID_NOOP("qtn_socamfs_save_success_message")).toUtf8().data());
	infoBanner->setBodyText(txt.c_str());
	infoBanner->setImageID("icon-l-settings");
	infoBanner->setManagedManually(true);
        infoBanner->setLayoutPosition(M::BottomCenterPosition);
	infoBanner->appear(MSceneWindow::DestroyWhenDone);


}



