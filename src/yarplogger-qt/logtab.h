/* 
 * Copyright (C)2014  iCub Facility - Istituto Italiano di Tecnologia
 * Author: Marco Randazzo
 * email:  marco.randazzo@iit.it
 * website: www.robotcub.org
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license can be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
*/

#ifndef LOGTAB_H
#define LOGTAB_H

#include <QMutex>
#include <QFrame>
#include <QTimer>
#include <QSortFilterProxyModel>
#include <QClipboard>
#include <yarp/logger/YarpLogger.h>
#include "messageWidget.h"
#include "logtabSorting.h"
#include "logmodel.h"


namespace Ui {
class LogTab;
}

class LogTab : public QFrame
{
    Q_OBJECT
    Q_DISABLE_COPY(LogTab)

public:
    explicit LogTab(yarp::yarpLogger::LoggerEngine*  _theLogger,
                    MessageWidget* _system_message,
                    std::string _portName,
                    QWidget *parent = 0,
                    int refreshRate = 100);
    ~LogTab();

private:
    Ui::LogTab*                     ui;
    std::string                     portName;
    yarp::yarpLogger::LoggerEngine* theLogger;
    MessageWidget*                  system_message;
    QMutex                          mutex;
    bool                            displayYarprunTimestamp_enabled;
    bool                            displayLocalTimestamp_enabled;
    bool                            displayLogLevel_enabled;
    bool                            displayFilename_enabled;
    bool                            displayLine_enabled;
    bool                            displayFunction_enabled;
    bool                            displayThreadId_enabled;
    bool                            displayComponent_enabled;
    bool                            displayColors_enabled;
    bool                            displayGrid_enabled;

private slots:
    void updateLog(bool from_beginning=false);
    void ctxMenu(const QPoint &pos);
    void on_copy_to_clipboard_action();

public:
    QTimer                  *logTimer;
    LogModel                *logModel;
#define USE_FILTERS 1
#if USE_FILTERS
    LogSortFilterProxyModel *proxyModelButtons;
    LogSortFilterProxyModel *proxyModelSearch;
#endif
    QClipboard              *clipboard;
    QMenu                   *contextMenu;

    void displayYarprunTimestamp(bool enabled);
    void displayLocalTimestamp  (bool enabled);
    void displayLogLevel        (bool enabled);
    void displayFilename        (bool enabled);
    void displayLine            (bool enabled);
    void displayFunction        (bool enabled);
    void displayThreadId        (bool enabled);
    void displayComponent       (bool enabled);
    void displayColors          (bool enabled);
    void displayGrid            (bool enabled);

    void clearLogModel();
};

#endif // LOGTAB_H
