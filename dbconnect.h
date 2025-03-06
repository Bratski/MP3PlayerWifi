#ifndef DBCONNECT_H
#define DBCONNECT_H
#include <QApplication>
#include <QDebug>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

bool createConnection() {
  // Check for available database drivers
  qDebug() << "Available database drivers:" << QSqlDatabase::drivers();

  // Create a SQLite database connection
  QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
  db.setDatabaseName("playlistDataBase.db"); // Database file name

  // Open the database
  if (!db.open()) {
    QMessageBox::critical(nullptr, "Database Error",
                          "Failed to open database: " + db.lastError().text());
    return false;
  }

  // Create tables if they don't exist already
  QSqlQuery query;

  // Create the Track table
  if (!query.exec("CREATE TABLE IF NOT EXISTS Track ("
                  "TraID INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, "
                  "TraName TEXT NOT NULL, "
                  "TraArtist TEXT NOT NULL, "
                  "TraAlbum TEXT NOT NULL, "
                  "TraYear INTEGER NOT NULL, "
                  "TraNumber INTEGER NOT NULL, "
                  "TraGenre TEXT NOT NULL, "
                  "TraDuration INTEGER NOT NULL, "
                  "TraBitrate INTEGER NOT NULL, "
                  "TraSamplerate INTEGER NOT NULL, "
                  "TraChannels INTEGER NOT NULL, "
                  "TraFileLocation	TEXT NOT NULL)")) {
    QMessageBox::critical(nullptr, "Database Error",
                          "Failed to create Track table: " +
                              query.lastError().text());
    return false;
  }

  // Create the Playlist table
  if (!query.exec("CREATE TABLE IF NOT EXISTS Playlist ("
                  "PllID INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, "
                  "PllName TEXT NOT NULL)")) {
    QMessageBox::critical(nullptr, "Database Error",
                          "Failed to create Playlist table: " +
                              query.lastError().text());
    return false;
  }

  // Create the TrackPlaylist table
  if (!query.exec("CREATE TABLE IF NOT EXISTS TrackPlaylist ("
                  "TraFK INTEGER NOT NULL, "
                  "PllFK INTEGER NOT NULL, "
                  "FOREIGN KEY(PllFK) REFERENCES Playlist(PllID), "
                  "FOREIGN KEY(TraFK) REFERENCES Track(TraID))")) {
    QMessageBox::critical(nullptr, "Database Error",
                          "Failed to create TrackPlaylist table: " +
                              query.lastError().text());
    return false;
  }

  qDebug() << "Database and tables created successfully!";
  return true;
}

#endif // DBCONNECT_H
