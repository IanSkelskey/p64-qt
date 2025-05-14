#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QString>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <vector>
#include <map>

class DatabaseManager {
public:
    DatabaseManager(const QString& dbPath);
    ~DatabaseManager();

    bool open();
    void close();

    // Functions to fetch data
    std::vector<std::map<QString, QVariant>> getAllGames();
    std::vector<std::map<QString, QVariant>> getAllCheats();
    std::vector<std::map<QString, QVariant>> getAllEnhancements();

    // New functions to fetch additional ROM data
    std::map<QString, QVariant> getGameByRomId(const QString& romId);
    std::vector<std::map<QString, QVariant>> getAudioSettingsByGameId(int gameId);
    std::vector<std::map<QString, QVariant>> getCoreSettingsByGameId(int gameId);
    std::vector<std::map<QString, QVariant>> getVideoSettingsByGameId(int gameId);
    std::map<QString, QVariant> getDeveloperById(int developerId);
    std::map<QString, QVariant> getGenreById(int genreId);
    std::map<QString, QVariant> getRegionById(int regionId);
    std::map<QString, QVariant> getCartridgeColorById(int colorId);

    // New ROM lookup functions
    static QString createRomIdFromCRC(uint32_t crc1, uint32_t crc2, const QString& countryCode);
    std::map<QString, QVariant> getRomInfoByCRC(uint32_t crc1, uint32_t crc2, const QString& countryCode);
    std::map<QString, QVariant> getRomInfoByCRCWithoutCountry(uint32_t crc1, uint32_t crc2);
    std::map<QString, QVariant> getRomCompleteInfo(uint32_t crc1, uint32_t crc2, const QString& countryCode);

    // Settings retrieval
    QString getRomSetting(int gameId, const QString& settingName, const QString& category);
    QString getCoreSetting(int gameId, const QString& settingName);
    QString getVideoSetting(int gameId, const QString& settingName);
    QString getAudioSetting(int gameId, const QString& settingName);

private:
    QString m_dbPath;
    QSqlDatabase m_db;

    std::vector<std::map<QString, QVariant>> executeQuery(const QString& query);
    std::map<QString, QVariant> executeSingleRowQuery(const QString& query);
};

#endif // DATABASEMANAGER_H