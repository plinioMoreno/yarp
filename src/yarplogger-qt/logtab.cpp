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

#include "logtab.h"
#include "ui_logtab.h"


LogTab::LogTab(yarp::yarpLogger::LoggerEngine* _theLogger,
               MessageWidget* _system_message,
               std::string _portName,
               QWidget *parent,
               int refreshRate) :
        QFrame(parent),
        ui(new Ui::LogTab),
        portName(_portName),
        theLogger(_theLogger),
        system_message(_system_message),
        displayYarprunTimestamp_enabled(true),
        displayLocalTimestamp_enabled(true),
        displayLogLevel_enabled(true),
        displayFilename_enabled(false),
        displayLine_enabled(false),
        displayFunction_enabled(false),
        displayThreadId_enabled(false),
        displayComponent_enabled(true),
        displayColors_enabled(true),
        displayGrid_enabled(true),
        logTimer(new QTimer(this)),
        logModel(new LogModel()),
#if USE_FILTERS
        proxyModelButtons(new LogSortFilterProxyModel(this)),
        proxyModelSearch(new LogSortFilterProxyModel(this)),
#endif
        clipboard(QApplication::clipboard())
{
    ui->setupUi(this);


#if USE_FILTERS
    proxyModelButtons->setSourceModel(logModel);
    proxyModelSearch->setSourceModel(proxyModelButtons);
    proxyModelSearch->setFilterKeyColumn(-1);
    ui->listView->setModel(proxyModelSearch);
#else
    ui->listView->setModel(logModel);
#endif

    connect(logTimer, SIGNAL(timeout()), this, SLOT(updateLog()));
    logTimer->start(refreshRate);

    connect(ui->listView, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(ctxMenu(const QPoint &)));

    updateLog(true);
}

void LogTab::ctxMenu(const QPoint &pos)
{
    QMenu *menu = new QMenu;
    menu->addAction(tr("Copy to clipboard"), this, SLOT(on_copy_to_clipboard_action()));
    menu->exec(ui->listView->mapToGlobal(pos));
}

void LogTab::on_copy_to_clipboard_action()
{
    QString selected_test;
    QString separator("\t\t");
    foreach(const QModelIndex &index, ui->listView->selectionModel()->selectedRows())
    {
        QStringList list;
#if USE_FILTERS
        QModelIndex prox_index_pre = proxyModelSearch->mapToSource(index);
        if (!prox_index_pre.isValid())
        {
            system_message->addMessage(QString("Invalid prox_index_pre in copy_to_clipboard"));
            return;
        }
        QModelIndex prox_index     = proxyModelButtons->mapToSource(prox_index_pre);
        if (!prox_index.isValid())
        {
            system_message->addMessage(QString("Invalid prox_index in copy_to_clipboard"));
            return;
        }
#else
        QModelIndex prox_index = index;
#endif
        if (displayYarprunTimestamp_enabled) list.append(logModel->data(prox_index, LogModel::YarprunTimestampRole).toString());
        if (displayLocalTimestamp_enabled)   list.append(logModel->data(prox_index, LogModel::LocalTimestampRole).toString());
        if (displayLogLevel_enabled)         list.append(logModel->data(prox_index, LogModel::LogLevelStringRole).toString());
        if (displayFilename_enabled)         list.append(logModel->data(prox_index, LogModel::FilenameRole).toString());
        if (displayLine_enabled)             list.append(logModel->data(prox_index, LogModel::LineStringRole).toString());
        if (displayFunction_enabled)         list.append(logModel->data(prox_index, LogModel::FunctionRole).toString());
        if (displayThreadId_enabled)         list.append(logModel->data(prox_index, LogModel::ThreadIdRole).toString());
        if (displayComponent_enabled)        list.append(logModel->data(prox_index, LogModel::ComponentRole).toString());
        list.append(logModel->data(prox_index, LogModel::TextRole).toString());
        selected_test += list.join(separator);
    }
    clipboard->setText(selected_test);
}

LogTab::~LogTab()
{
    delete logTimer;
    delete logModel;
#if USE_FILTERS
    delete proxyModelButtons;
    delete proxyModelSearch;
#endif
    delete ui;
}

void LogTab::clearLogModel()
{
    mutex.lock();
    if (logModel) {
        logModel->clear();
    }
    mutex.unlock();
}

void LogTab::updateLog(bool from_beginning)
{
    mutex.lock();
    std::list<yarp::yarpLogger::MessageEntry> messages;
    theLogger->get_messages_by_port_complete(portName, messages, from_beginning);
    logModel->addMessages(messages);

    ui->listView->setColumnHidden(YARPRUNTIMESTAMP_COLUMN, !displayYarprunTimestamp_enabled);
    ui->listView->setColumnHidden(LOCALTIMESTAMP_COLUMN,   !displayLocalTimestamp_enabled);
    ui->listView->setColumnHidden(LOGLEVEL_COLUMN,         !displayLogLevel_enabled);
    ui->listView->setColumnHidden(FILENAME_COLUMN,         !displayFilename_enabled);
    ui->listView->setColumnHidden(LINE_COLUMN,             !displayLine_enabled);
    ui->listView->setColumnHidden(FUNCTION_COLUMN,         !displayFunction_enabled);
    ui->listView->setColumnHidden(THREADID_COLUMN,         !displayThreadId_enabled);
    ui->listView->setColumnHidden(COMPONENT_COLUMN,        !displayComponent_enabled);
    ui->listView->setShowGrid(displayGrid_enabled);
    mutex.unlock();
}

void LogTab::displayYarprunTimestamp(bool enabled)
{
    displayYarprunTimestamp_enabled = enabled;
    ui->listView->setColumnHidden(YARPRUNTIMESTAMP_COLUMN, !displayYarprunTimestamp_enabled);
    if (enabled) {
        ui->listView->setColumnWidth(YARPRUNTIMESTAMP_COLUMN, 115);
    }
}

void LogTab::displayLocalTimestamp(bool enabled)
{
    displayLocalTimestamp_enabled = enabled;
    ui->listView->setColumnHidden(LOCALTIMESTAMP_COLUMN, !displayLocalTimestamp_enabled);
    if (enabled) {
        ui->listView->setColumnWidth(LOCALTIMESTAMP_COLUMN, 115);
    }
}

void LogTab::displayLogLevel(bool enabled)
{
    displayLogLevel_enabled = enabled;
    ui->listView->setColumnHidden(LOGLEVEL_COLUMN, !displayLogLevel_enabled);
    if (enabled) {
        ui->listView->setColumnWidth(LOGLEVEL_COLUMN, 60);
    }
}

void LogTab::displayFilename(bool enabled)
{
    displayFilename_enabled = enabled;
    ui->listView->setColumnHidden(FILENAME_COLUMN, !displayFilename_enabled);
    if (enabled) {
        ui->listView->setColumnWidth(FILENAME_COLUMN, 120);
    }
}

void LogTab::displayLine(bool enabled)
{
    displayLine_enabled = enabled;
    ui->listView->setColumnHidden(LINE_COLUMN, !displayLine_enabled);
    if (enabled) {
        ui->listView->setColumnWidth(LINE_COLUMN, 40);
    }
}

void LogTab::displayFunction(bool enabled)
{
    displayFunction_enabled = enabled;
    ui->listView->setColumnHidden(FUNCTION_COLUMN, !displayFunction_enabled);
    if (enabled) {
        ui->listView->setColumnWidth(FUNCTION_COLUMN, 120);
    }
}

void LogTab::displayThreadId(bool enabled)
{
    displayThreadId_enabled = enabled;
    ui->listView->setColumnHidden(THREADID_COLUMN, !displayThreadId_enabled);
    if (enabled) {
        ui->listView->setColumnWidth(THREADID_COLUMN, 80);
    }
}

void LogTab::displayComponent(bool enabled)
{
    displayComponent_enabled = enabled;
    ui->listView->setColumnHidden(COMPONENT_COLUMN, !displayComponent_enabled);
    if (enabled) {
        ui->listView->setColumnWidth(COMPONENT_COLUMN, 110);
    }
}

void LogTab::displayColors(bool enabled)
{
    displayColors_enabled = enabled;
    logModel->setColor(enabled);
}

void LogTab::displayGrid(bool enabled)
{
    displayGrid_enabled = enabled;
    ui->listView->setShowGrid(displayGrid_enabled);
}
