#include "DatabaseManager.h"
#include <QDebug>
#include <QSqlRecord>

DatabaseManager::DatabaseManager(const QString& dbPath) : m_dbPath(dbPath), m_db(QSqlDatabase::addDatabase("QSQLITE")) {
    m_db.setDatabaseName(m_dbPath);
}

DatabaseManager::~DatabaseManager() {
    close();
}

bool DatabaseManager::open() {
    if (!m_db.open()) {
        qWarning() << "Failed to open database:" << m_db.lastError().text();
        return false;
    }
    return true;
}

void DatabaseManager::close() {
    if (m_db.isOpen()) {
        m_db.close();
    }
}

std::vector<std::map<QString, QVariant>> DatabaseManager::getAllGames() {
    return executeQuery("SELECT * FROM games");
}

std::vector<std::map<QString, QVariant>> DatabaseManager::getAllCheats() {
    return executeQuery("SELECT * FROM cheats");
}

std::vector<std::map<QString, QVariant>> DatabaseManager::getAllEnhancements() {
    return executeQuery("SELECT * FROM enhancements");
}

std::vector<std::map<QString, QVariant>> DatabaseManager::executeQuery(const QString& query) {
    std::vector<std::map<QString, QVariant>> results;

    QSqlQuery q(m_db);
    if (!q.exec(query)) {
        qWarning() << "Query failed:" << q.lastError().text();
        return results;
    }

    while (q.next()) {
        std::map<QString, QVariant> row;
        for (int i = 0; i < q.record().count(); ++i) {
            row[q.record().fieldName(i)] = q.value(i);
        }
        results.push_back(row);
    }

    return results;
}

std::map<QString, QVariant> DatabaseManager::getGameByRomId(const QString& romId) {
    auto results = executeQuery(QString("SELECT * FROM games WHERE rom_id = '%1'").arg(romId));
    return results.empty() ? std::map<QString, QVariant>() : results.front();
}

std::vector<std::map<QString, QVariant>> DatabaseManager::getAudioSettingsByGameId(int gameId) {
    return executeQuery(QString("SELECT * FROM audio_settings WHERE game_id = %1").arg(gameId));
}

std::vector<std::map<QString, QVariant>> DatabaseManager::getCoreSettingsByGameId(int gameId) {
    return executeQuery(QString("SELECT * FROM core_settings WHERE game_id = %1").arg(gameId));
}

std::vector<std::map<QString, QVariant>> DatabaseManager::getVideoSettingsByGameId(int gameId) {
    return executeQuery(QString("SELECT * FROM video_settings WHERE game_id = %1").arg(gameId));
}

std::map<QString, QVariant> DatabaseManager::getDeveloperById(int developerId) {
    auto results = executeQuery(QString("SELECT * FROM developers WHERE id = %1").arg(developerId));
    return results.empty() ? std::map<QString, QVariant>() : results.front();
}

std::map<QString, QVariant> DatabaseManager::getGenreById(int genreId) {
    auto results = executeQuery(QString("SELECT * FROM genres WHERE id = %1").arg(genreId));
    return results.empty() ? std::map<QString, QVariant>() : results.front();
}

std::map<QString, QVariant> DatabaseManager::getRegionById(int regionId) {
    auto results = executeQuery(QString("SELECT * FROM regions WHERE id = %1").arg(regionId));
    return results.empty() ? std::map<QString, QVariant>() : results.front();
}

std::map<QString, QVariant> DatabaseManager::getCartridgeColorById(int colorId) {
    auto results = executeQuery(QString("SELECT * FROM cartridge_colors WHERE id = %1").arg(colorId));
    return results.empty() ? std::map<QString, QVariant>() : results.front();
}

std::map<QString, QVariant> DatabaseManager::executeSingleRowQuery(const QString& query) {
    auto results = executeQuery(query);
    return results.empty() ? std::map<QString, QVariant>() : results.front();
}

QString DatabaseManager::createRomIdFromCRC(uint32_t crc1, uint32_t crc2, const QString& countryCode) {
    // Format similar to Project64 RDB format: CRC1-CRC2-C:CC
    // For example: A1B2C3D4-E5F60708-C:45
    return QString("%1-%2-C:%3")
        .arg(QString::number(crc1, 16).toUpper().rightJustified(8, '0'))
        .arg(QString::number(crc2, 16).toUpper().rightJustified(8, '0'))
        .arg(countryCode);
}

std::map<QString, QVariant> DatabaseManager::getRomInfoByCRC(uint32_t crc1, uint32_t crc2, const QString& countryCode) {
    QString romId = createRomIdFromCRC(crc1, crc2, countryCode);
    
    // Try exact match first
    auto result = getGameByRomId(romId);
    
    // If not found, try alternate formats
    if (result.empty()) {
        // Try with brackets
        QString bracketRomId = QString("[%1]").arg(romId);
        result = getGameByRomId(bracketRomId);
        
        // Try without country code
        if (result.empty()) {
            result = getRomInfoByCRCWithoutCountry(crc1, crc2);
        }
    }
    
    return result;
}

std::map<QString, QVariant> DatabaseManager::getRomInfoByCRCWithoutCountry(uint32_t crc1, uint32_t crc2) {
    // Try to find a ROM by CRC values only, ignoring country code
    QString crcOnly = QString("%1-%2")
        .arg(QString::number(crc1, 16).toUpper().rightJustified(8, '0'))
        .arg(QString::number(crc2, 16).toUpper().rightJustified(8, '0'));
    
    // Try without brackets first
    auto result = executeSingleRowQuery(
        QString("SELECT * FROM games WHERE rom_id LIKE '%1%'").arg(crcOnly));
    
    // Try with brackets
    if (result.empty()) {
        QString bracketCrcOnly = QString("[%1]").arg(crcOnly);
        result = executeSingleRowQuery(
            QString("SELECT * FROM games WHERE rom_id LIKE '%1%'").arg(bracketCrcOnly));
    }
    
    return result;
}

std::map<QString, QVariant> DatabaseManager::getRomCompleteInfo(uint32_t crc1, uint32_t crc2, const QString& countryCode) {
    // Get the basic ROM info
    auto gameInfo = getRomInfoByCRC(crc1, crc2, countryCode);
    
    if (gameInfo.empty()) {
        return gameInfo; // ROM not found
    }
    
    // Get the game ID
    int gameId = gameInfo["id"].toInt();
    
    // Get developer info
    if (gameInfo.count("developer_id") && gameInfo["developer_id"].isValid()) {
        auto developerInfo = getDeveloperById(gameInfo["developer_id"].toInt());
        if (!developerInfo.empty()) {
            gameInfo["developer_name"] = developerInfo["name"];
        }
    }
    
    // Get genre info
    if (gameInfo.count("genre_id") && gameInfo["genre_id"].isValid()) {
        auto genreInfo = getGenreById(gameInfo["genre_id"].toInt());
        if (!genreInfo.empty()) {
            gameInfo["genre_name"] = genreInfo["name"];
        }
    }
    
    // Get region info
    if (gameInfo.count("region_id") && gameInfo["region_id"].isValid()) {
        auto regionInfo = getRegionById(gameInfo["region_id"].toInt());
        if (!regionInfo.empty()) {
            gameInfo["region_name"] = regionInfo["name"];
            gameInfo["region_code"] = regionInfo["code"];
        }
    }
    
    // Get cartridge color info
    if (gameInfo.count("cartridge_color_id") && gameInfo["cartridge_color_id"].isValid()) {
        auto colorInfo = getCartridgeColorById(gameInfo["cartridge_color_id"].toInt());
        if (!colorInfo.empty()) {
            gameInfo["cartridge_color_name"] = colorInfo["name"];
        }
    }
    
    return gameInfo;
}

QString DatabaseManager::getRomSetting(int gameId, const QString& settingName, const QString& category) {
    QString query;
    
    if (category == "core") {
        query = QString("SELECT setting_value FROM core_settings WHERE game_id = %1 AND setting_name = '%2'")
                .arg(gameId).arg(settingName);
    } else if (category == "video") {
        query = QString("SELECT setting_value FROM video_settings WHERE game_id = %1 AND setting_name = '%2'")
                .arg(gameId).arg(settingName);
    } else if (category == "audio") {
        query = QString("SELECT setting_value FROM audio_settings WHERE game_id = %1 AND setting_name = '%2'")
                .arg(gameId).arg(settingName);
    } else {
        return QString(); // Unknown category
    }
    
    auto result = executeSingleRowQuery(query);
    return result.empty() ? QString() : result["setting_value"].toString();
}

QString DatabaseManager::getCoreSetting(int gameId, const QString& settingName) {
    return getRomSetting(gameId, settingName, "core");
}

QString DatabaseManager::getVideoSetting(int gameId, const QString& settingName) {
    return getRomSetting(gameId, settingName, "video");
}

QString DatabaseManager::getAudioSetting(int gameId, const QString& settingName) {
    return getRomSetting(gameId, settingName, "audio");
}