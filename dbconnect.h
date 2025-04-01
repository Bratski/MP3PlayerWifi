#ifndef DBCONNECT_H
#define DBCONNECT_H
#include <QApplication>
#include <QDebug>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

bool createConnection(const QString& playlistName, const int& playlistID);
#endif // DBCONNECT_H
