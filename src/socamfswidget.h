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

#ifndef SOCAMFSWIDGET_H
#define SOCAMFSWIDGET_H

#define ROW_SPACING_SHORT 5
#define ROW_SPACING 15
#define SLIDER_VALUE_SEPARATOR  ": "

#include <dcpwidget.h>
#include <MTextEdit>
#include <MLabel>
#include <string>
#include <MBanner>
#include <MInfoBanner>
#include <MSceneWindow>
#include <QTimer>

using namespace std;

class MButton;
class MSlider;
class MLabel;
class MComboBox;
class MGConfItem;
class SocamFSBusinessLogic;
class MLinearLayoutPolicy;

class SocamFSWidget : public DcpWidget
{
    Q_OBJECT

public:
    SocamFSWidget (QGraphicsWidget *parent = 0);
    virtual ~SocamFSWidget ();
    void retranslateUi ();

protected:
    void initWidget ();

private slots:
    void saveButtonPressed ();
    
    void catalogPeriodSliderChanged(int value);
    void cleanIntervalSliderChanged(int value);
    void fileSystemSizeSliderChanged(int value);
    void minFreeDiskSliderChanged(int value);
    void dependencyThresholdSliderChanged(int value);
    void maxFileToPredictSliderChanged(int value);

private:

    int rows;
    
    enum SizeUnit { KB, MB, GB }; 
    void addLogLevelsCBox ();
    void addProfilesCBox ();
    void addMaxFilesToPredictCBox ();
    void addCleanIntervalCBox ();
    void addCatalogUpdatePeriodsCBox ();	
    void addTextBox(MLabel* &label, MTextEdit* &textBox, const char* name, const char* text);
    void addSlider(MLabel* &label, MSlider* &slider, const char* name, unsigned long long min, unsigned long long max, int value);

    string translateTime(int minutes);
    string sizeToText(int size, SizeUnit unit);

private:
    SocamFSBusinessLogic   *m_logic;

    MLinearLayoutPolicy    *m_MainLayout;


    //Labels
    //----------------------------------------------------
    MLabel    *m_TitleLabel;
    MLabel    *m_SubTitleLabel;
    MLabel    *m_UrlLabel;
    MLabel    *m_UserLabel;
    MLabel    *m_PasswordLabel;
    MLabel    *mProfileLabel;
    string    m_CatalogPeriodLabelText;
    MLabel    *m_CatalogPeriodLabel;
    string    m_MinFreeDiskLabelText;
    MLabel    *m_MinFreeDiskLabel;
    string    m_CleanIntervalLabelText;
    MLabel    *m_CleanIntervalLabel;
    string    m_DependencyThresholdLabelText;
    MLabel    *m_DependencyThresholdLabel;
    string    m_MaxFileToPredictLabelText;
    MLabel    *m_MaxFileToPredictLabel;
    string    m_FileSystemSizeLabelText;
    MLabel    *m_FileSystemSizeLabel;
    //----------------------------------------------------


    //Components
    //---------------------------------------------------- 
    MComboBox              *m_logLevelsCBox;
    MComboBox              *m_profilesCBox;

    MSlider                *m_CatalogPeriodSlider;
    MSlider                *m_CleanIntervalSlider;
    MSlider                *m_FileSystemSizeSlider;
    MSlider                *m_MinFreeDiskSlider;
    MSlider                *m_DependencyThresholdSlider;
    MSlider                *m_MaxFileToPredictSlider;

    MLinearLayoutPolicy    *m_urlLayout;
    MTextEdit              *m_UrlBox;
    MTextEdit              *m_UserBox;
    MTextEdit              *m_PasswordBox;

    bool                   m_topCloseChangeable;
    MButton                *m_CloseSwitch;
    MButton                *m_SaveButton;

    //texts
    string txtDay;
    string txtHour;
    string txtMinute;
     
    //---------------------------------------------------- 

#ifdef UNIT_TEST
    friend class Ut_SocamFSWidget;
#endif
};


#endif // SOCAMFSWIDGET_H

