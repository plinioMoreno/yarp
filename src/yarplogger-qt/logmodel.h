/*
 * Copyright (C) 2012-2014  iCub Facility, Istituto Italiano di Tecnologia
 * Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#ifndef LOGMODEL_H
#define LOGMODEL_H

#include <QAbstractTableModel>
#include <yarp/logger/YarpLogger.h>
#include <QFont>


const std::string TRACE_STRING   = "TRACE";
const std::string DEBUG_STRING   = "DEBUG";
const std::string INFO_STRING    = "INFO";
const std::string WARNING_STRING = "WARNING";
const std::string ERROR_STRING   = "ERROR";
const std::string FATAL_STRING   = "FATAL";

const int YARPRUNTIMESTAMP_COLUMN = 0;
const int LOCALTIMESTAMP_COLUMN   = 1;
const int LOGLEVEL_COLUMN         = 2;
const int FILENAME_COLUMN         = 3;
const int LINE_COLUMN             = 4;
const int FUNCTION_COLUMN         = 5;
const int THREADID_COLUMN         = 6;
const int COMPONENT_COLUMN        = 7;
const int TEXT_COLUMN             = 8;

class LogModel : public QAbstractTableModel
{
    Q_OBJECT
    Q_DISABLE_COPY(LogModel);

public:
    enum Roles {
        YarprunTimestampRole = Qt::UserRole,
        LocalTimestampRole,
        LogLevelRole,
        LogLevelStringRole,
        FilenameRole,
        LineRole,
        LineStringRole,
        FunctionRole,
        ThreadIdRole,
        ComponentRole,
        TextRole
    };

    explicit LogModel(QObject *parent = 0);
    virtual ~LogModel();

    virtual int rowCount(const QModelIndex & parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;

    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    void addMessages(const std::list<yarp::yarpLogger::MessageEntry> &messages);
    void setColor(bool enabled);
    void clear();

private:
    QFont font;
    bool color;
    QList<yarp::yarpLogger::MessageEntry> messages;
};


#endif // LOGMODEL_H
