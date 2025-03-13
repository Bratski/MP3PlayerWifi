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

  // Create the Artist table
  if (!query.exec("CREATE TABLE IF NOT EXISTS Artist ("
                  "ArtID INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, "
                  "ArtName TEXT NOT NULL UNIQUE, "
                  "ArtGenre TEXT NOT NULL)")) {
    QMessageBox::critical(nullptr, "Database Error",
                          "Failed to create Artist table: " +
                              query.lastError().text());
    return false;
  }

  // Create the Album table
  if (!query.exec("CREATE TABLE IF NOT EXISTS Album ("
                  "AlbID INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, "
                  "AlbName TEXT NOT NULL, "
                  "AlbYear TEXT NOT NULL, "
                  "AlbArtFK INTEGER NOT NULL, "
                  "FOREIGN KEY(AlbArtFK) REFERENCES Artist(ArtID), "
                  "UNIQUE (AlbName, AlbArtFK))")) {
    QMessageBox::critical(nullptr, "Database Error",
                          "Failed to create Album table: " +
                              query.lastError().text());
    return false;
  }

  // Create the Track table
  if (!query.exec("CREATE TABLE IF NOT EXISTS Track ("
                  "TraID INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, "
                  "TraName TEXT NOT NULL, "
                  "TraNumber INTEGER NOT NULL, "
                  "TraDuration INTEGER NOT NULL, "
                  "TraBitrate INTEGER NOT NULL, "
                  "TraSamplerate INTEGER NOT NULL, "
                  "TraChannels INTEGER NOT NULL, "
                  "TraFileLocation TEXT NOT NULL, "
                  "TraAlbFK INTEGER NOT NULL, "
                  "FOREIGN KEY(TraAlbFK) REFERENCES Album(AlbID), "
                  "UNIQUE (TraName, TraAlbFK))")) {
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
  if (!query.exec(
          "CREATE TABLE IF NOT EXISTS TrackPlaylist ("
          "TraFK INTEGER NOT NULL, "
          "PllFK INTEGER NOT NULL, "
          "FOREIGN KEY(PllFK) REFERENCES Playlist(PllID) ON DELETE CASCADE, "
          "FOREIGN KEY(TraFK) REFERENCES Track(TraID) ON DELETE CASCADE, "
          "UNIQUE (TraFK, PllFK))")) {
    QMessageBox::critical(nullptr, "Database Error",
                          "Failed to create TrackPlaylist table: " +
                              query.lastError().text());
    return false;
  }

  // Create the default playlist table, at least one empty playlist is available
  // Step 1: Check if the playlist with the given ID exists
  QString playlistName = "default Playlist";
  int playlistID = 1;

  query.prepare("SELECT PllID FROM Playlist WHERE PllID = :PllID");
  query.bindValue(":PllID", playlistID);

  if (!query.exec()) {
    QMessageBox::critical(nullptr, "Database Error",
                          "Failed to execute query: " +
                              query.lastError().text());
    return false;
  }

  if (query.next()) {
    // Playlist already exists
    qDebug() << "Playlist with ID" << playlistID << "already exists.";
    return true;
  }

  // Step 2: Insert a new playlist with the specified ID
  query.prepare(
      "INSERT INTO Playlist (PllID, PllName) VALUES (:PllID, :PllName)");
  query.bindValue(":PllID", playlistID); // Explicitly set the PllID
  query.bindValue(":PllName", playlistName);

  if (!query.exec()) {
    QMessageBox::critical(nullptr, "Database Error",
                          "Failed to insert playlist: " +
                              query.lastError().text());
    return false;
  }

  qDebug() << "Database and tables created successfully!";
  return true;
}

#endif // DBCONNECT_H
