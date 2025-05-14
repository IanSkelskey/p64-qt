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

    // ROM lookup functions
    static QString createRomIdFromCRC(uint32_t crc1, uint32_t crc2, const QString& countryCode);
    std::map<QString, QVariant> getRomInfoByCRC(uint32_t crc1, uint32_t crc2, const QString& countryCode);
    std::map<QString, QVariant> getRomInfoByCRCWithoutCountry(uint32_t crc1, uint32_t crc2);
    std::map<QString, QVariant> getRomInfoByCartridgeCode(const QString& cartridgeCode);
    std::map<QString, QVariant> getRomInfoByInternalName(const QString& internalName);
    std::map<QString, QVariant> getRomCompleteInfo(uint32_t crc1, uint32_t crc2, const QString& countryCode);
    
    // Enhanced ROM info getters
    std::map<QString, QVariant> getGameByRomId(const QString& romId);
    QString getGameGoodName(uint32_t crc1, uint32_t crc2, const QString& countryCode);
    QString getGameInternalName(uint32_t crc1, uint32_t crc2, const QString& countryCode);
    QString getGameCartridgeCode(uint32_t crc1, uint32_t crc2, const QString& countryCode);
    QString getGameDeveloper(uint32_t crc1, uint32_t crc2, const QString& countryCode);
    QString getGameGenre(uint32_t crc1, uint32_t crc2, const QString& countryCode);
    QString getGameReleaseDate(uint32_t crc1, uint32_t crc2, const QString& countryCode);
    int getGamePlayers(uint32_t crc1, uint32_t crc2, const QString& countryCode);
    bool getGameForceFeedback(uint32_t crc1, uint32_t crc2, const QString& countryCode);
    QString getGameStatus(uint32_t crc1, uint32_t crc2, const QString& countryCode);
    QString getGameRegion(uint32_t crc1, uint32_t crc2, const QString& countryCode);
    QString getGameCartridgeColor(uint32_t crc1, uint32_t crc2, const QString& countryCode);

    // Related entity getters
    std::vector<std::map<QString, QVariant>> getAudioSettingsByGameId(int gameId);
    std::vector<std::map<QString, QVariant>> getCoreSettingsByGameId(int gameId);
    std::vector<std::map<QString, QVariant>> getVideoSettingsByGameId(int gameId);
    std::map<QString, QVariant> getDeveloperById(int developerId);
    std::map<QString, QVariant> getGenreById(int genreId);
    std::map<QString, QVariant> getRegionById(int regionId);
    std::map<QString, QVariant> getCartridgeColorById(int colorId);
    
    // Game settings
    QString getRomSetting(int gameId, const QString& settingName, const QString& category);
    QString getCoreSetting(int gameId, const QString& settingName);
    QString getVideoSetting(int gameId, const QString& settingName);
    QString getAudioSetting(int gameId, const QString& settingName);
    
    // Game settings by CRC
    QString getCoreSettingByCRC(uint32_t crc1, uint32_t crc2, const QString& countryCode, const QString& settingName);
    QString getVideoSettingByCRC(uint32_t crc1, uint32_t crc2, const QString& countryCode, const QString& settingName);
    QString getAudioSettingByCRC(uint32_t crc1, uint32_t crc2, const QString& countryCode, const QString& settingName);
    
    // Schema inspection methods
    QStringList getTables() const;
    QStringList getColumns(const QString& tableName) const;
    void logDatabaseSchema() const;
    void logBasicDatabaseInfo() const;  // Add this method
    QString getDatabaseInfo() const;

    // Additional helper functions
    static QString formatCRC(uint32_t crc);
    static QString normalizeCountryCode(const QString& countryCode);
    bool isDatabaseLoaded() const;
    QString getDatabaseVersion() const;

private:
    QString m_dbPath;
    QSqlDatabase m_db;
    
    // Query execution helpers
    std::vector<std::map<QString, QVariant>> executeQuery(const QString& query) const;
    std::map<QString, QVariant> executeSingleRowQuery(const QString& query) const;
    QVariant executeSingleValueQuery(const QString& query) const;
    
    // Helper to get game ID from CRC
    int getGameIdFromCRC(uint32_t crc1, uint32_t crc2, const QString& countryCode);
    
    // Helper to generate multiple ROM ID formats for flexible lookup
    QStringList generatePossibleRomIds(uint32_t crc1, uint32_t crc2, const QString& countryCode);
};

#endif // DATABASEMANAGER_H