#ifndef DBCONNECT_H
#define DBCONNECT_H

// this should disable or enable qdebug output
#define QT_NO_DEBUG_OUTPUT

#include <QApplication>
#include <QDebug>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

bool createConnection(const QString& playlistName, const int& playlistID);
#endif // DBCONNECT_H
