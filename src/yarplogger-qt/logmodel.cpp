/*
 * Copyright (C) 2012-2014  iCub Facility, Istituto Italiano di Tecnologia
 * Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include "logmodel.h"

#include <QBrush>

inline static QString logLevelToString(yarp::yarpLogger::LogLevelEnum level)
{
    switch(level) {
    case yarp::yarpLogger::LOGLEVEL_UNDEFINED:
        return QString();
    case yarp::yarpLogger::LOGLEVEL_TRACE:
        return QString(TRACE_STRING.c_str());
    case yarp::yarpLogger::LOGLEVEL_DEBUG:
        return QString(DEBUG_STRING.c_str());
    case yarp::yarpLogger::LOGLEVEL_INFO:
        return QString(INFO_STRING.c_str());
    case yarp::yarpLogger::LOGLEVEL_WARNING:
        return QString(WARNING_STRING.c_str());
    case yarp::yarpLogger::LOGLEVEL_ERROR:
        return QString(ERROR_STRING.c_str());
    case yarp::yarpLogger::LOGLEVEL_FATAL:
        return QString(FATAL_STRING.c_str());
    default:
        return QString();
    }
}


LogModel::LogModel(QObject *parent) :
        QAbstractTableModel(parent),
#if defined Q_OS_WIN
        font("Consolas"),
#elif defined Q_OS_OSX
        font("Monaco"),
#else
        font("Monospace"),
#endif
        color(true)
{
    font.setStyleHint(QFont::Monospace);
}

LogModel::~LogModel()
{
}

int LogModel::rowCount(const QModelIndex &parent) const
{
    // If the index is the root item, then return the row count,
    // otherwise return 0
    if (parent == QModelIndex()) {
       return messages.size();
    }
    return 0;
}

int LogModel::columnCount(const QModelIndex &parent) const
{
    // If the index is the root item, then return the column count,
    // otherwise return 0
    if (parent == QModelIndex()) {
       return TEXT_COLUMN + 1;
    }
    return 0;
}

QVariant LogModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Vertical) {
        return QVariant();
    }

    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    switch (section) {
    case YARPRUNTIMESTAMP_COLUMN:
        return tr("yarprun timestamp");
    case LOCALTIMESTAMP_COLUMN:
        return tr("local timestamp");
    case LOGLEVEL_COLUMN:
        return tr("level");
    case FILENAME_COLUMN:
        return tr("filename");
    case LINE_COLUMN:
        return tr("line");
    case FUNCTION_COLUMN:
        return tr("function");
    case THREADID_COLUMN:
        return tr("thread id");
    case COMPONENT_COLUMN:
        return tr("component");
    case TEXT_COLUMN:
        return tr("message");
    default:
        return QVariant();
    }
}

QVariant LogModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
        case YARPRUNTIMESTAMP_COLUMN:
            return QString(messages.at(index.row()).yarprun_timestamp.c_str());
        case LOCALTIMESTAMP_COLUMN:
            return QString(messages.at(index.row()).local_timestamp.c_str());
        case LOGLEVEL_COLUMN:
            return logLevelToString(messages.at(index.row()).level);
        case FILENAME_COLUMN:
            return QString(messages.at(index.row()).filename.c_str());
        case LINE_COLUMN:
            return QString(messages.at(index.row()).line.c_str());
        case FUNCTION_COLUMN:
            return QString(messages.at(index.row()).function.c_str());
        case THREADID_COLUMN:
            return QString(messages.at(index.row()).thread_id.c_str());
        case COMPONENT_COLUMN:
            return QString(messages.at(index.row()).component.c_str());
        case TEXT_COLUMN:
            return QString(messages.at(index.row()).text.c_str());
        default:
            return QString();
        }
    }

    if (role == Qt::BackgroundRole) {
        if(!color) {
            return QBrush();
        }
        switch (messages.at(index.row()).level) {
        case yarp::yarpLogger::LOGLEVEL_UNDEFINED:
            return QBrush(Qt::white);
        case yarp::yarpLogger::LOGLEVEL_TRACE:
            return QBrush(QColor("#E9E9E9"));
        case yarp::yarpLogger::LOGLEVEL_DEBUG:
            return QBrush(QColor("#7070FF")); // FIXME Debug should be green
        case yarp::yarpLogger::LOGLEVEL_INFO:
            return QBrush(QColor("#70FF70")); // FIXME Info should be blue
        case yarp::yarpLogger::LOGLEVEL_WARNING:
            return QBrush(QColor("#FFFF70"));
        case yarp::yarpLogger::LOGLEVEL_ERROR:
            return QBrush(QColor("#FF7070"));
        case yarp::yarpLogger::LOGLEVEL_FATAL:
            return QBrush(Qt::black);
        default:
            return QBrush(Qt::white);
        }
    }

    if (role == Qt::ForegroundRole) {
        if(!color) {
            return QBrush();
        }
        if (messages.at(index.row()).level == yarp::yarpLogger::LOGLEVEL_FATAL) {
            return QBrush(Qt::white);
        }
        return QBrush(Qt::black);
    }

    if (role == Qt::FontRole) {
        return font;
    }

    if (role == Qt::TextAlignmentRole) {
        switch (index.column()) {
        case YARPRUNTIMESTAMP_COLUMN:
        case LOCALTIMESTAMP_COLUMN:
        case LINE_COLUMN:
        case THREADID_COLUMN:
            return Qt::AlignRight;
        case LOGLEVEL_COLUMN:
             return Qt::AlignHCenter;
       case FILENAME_COLUMN:
        case FUNCTION_COLUMN:
        case COMPONENT_COLUMN:
        case TEXT_COLUMN:
        default:
            return Qt::AlignLeft;
        }
    }

    if (role == YarprunTimestampRole) {
        return QString(messages.at(index.row()).yarprun_timestamp.c_str());
    }

    if (role == LocalTimestampRole) {
        return QString(messages.at(index.row()).local_timestamp.c_str());
    }

    if (role == LogLevelRole) {
        return messages.at(index.row()).level;
    }

    if (role == LogLevelStringRole) {
        return logLevelToString(messages.at(index.row()).level);
    }

    if (role == FilenameRole) {
        return QString(messages.at(index.row()).filename.c_str());
    }

    if (role == LineRole) {
        return QString(messages.at(index.row()).line.c_str()).toInt();
    }

    if (role == LineStringRole) {
        return QString(messages.at(index.row()).line.c_str());
    }

    if (role == FunctionRole) {
        return QString(messages.at(index.row()).function.c_str());
    }

    if (role == ThreadIdRole) {
        return QString(messages.at(index.row()).thread_id.c_str());
    }

    if (role == ComponentRole) {
        return QString(messages.at(index.row()).component.c_str());
    }

    if (role == TextRole) {
        return QString(messages.at(index.row()).text.c_str());
    }

    return QVariant();
}

void LogModel::addMessages(const std::list<yarp::yarpLogger::MessageEntry> &messages)
{
    beginInsertRows(QModelIndex(),
                    rowCount(),
                    rowCount() + messages.size() - 1);
    this->messages.append(QList<yarp::yarpLogger::MessageEntry>::fromStdList(messages));
    endInsertRows();
}

void LogModel::setColor(bool enabled)
{
    if (color != enabled) {
        color = enabled;
        Q_EMIT dataChanged(index(0, 0),
                           index(rowCount()-1,columnCount()-1),
                           QVector<int>() << Qt::BackgroundRole
                                          << Qt::ForegroundRole);
    }
}

void LogModel::clear()
{
    beginResetModel();
    this->messages.clear();
    endInsertRows();
}
