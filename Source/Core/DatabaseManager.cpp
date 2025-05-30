#include "DatabaseManager.h"
#include <QDebug>
#include <QSqlRecord>
#include <QRegularExpression>
#include <QSqlDriver>
#include <QFile>  // Add this include for QFile class

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
    
    // Only log schema info if tables are missing
    QStringList tables = getTables();
    if (tables.isEmpty()) {
        qWarning() << "Database opened but contains no tables:" << m_dbPath;
        logDatabaseSchema();
    }
    
    // Check if rom_browser_view exists
    if (!viewExists("rom_browser_view")) {
        qWarning() << "rom_browser_view does not exist in the database, some operations may be slower";
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

std::vector<std::map<QString, QVariant>> DatabaseManager::executeQuery(const QString& query) const {
    std::vector<std::map<QString, QVariant>> results;

    QSqlQuery q(m_db);
    if (!q.exec(query)) {
        qWarning() << "Query failed:" << q.lastError().text() << "Query:" << query;
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

std::map<QString, QVariant> DatabaseManager::executeSingleRowQuery(const QString& query) const {
    auto results = executeQuery(query);
    return results.empty() ? std::map<QString, QVariant>() : results.front();
}

QVariant DatabaseManager::executeSingleValueQuery(const QString& query) const {
    QSqlQuery q(m_db);
    if (!q.exec(query) || !q.next()) {
        return QVariant();
    }
    return q.value(0);
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

QString DatabaseManager::createRomIdFromCRC(uint32_t crc1, uint32_t crc2, const QString& countryCode) {
    // Format similar to Project64 RDB format: CRC1-CRC2-C:CC
    return QString("%1-%2-C:%3")
        .arg(formatCRC(crc1))
        .arg(formatCRC(crc2))
        .arg(normalizeCountryCode(countryCode));
}

QString DatabaseManager::formatCRC(uint32_t crc) {
    return QString::number(crc, 16).toUpper().rightJustified(8, '0');
}

QString DatabaseManager::normalizeCountryCode(const QString& countryCode) {
    // Ensure country code is properly formatted (2 hex digits)
    QString normalized = countryCode;
    
    // Remove any non-hex characters
    normalized.remove(QRegularExpression("[^0-9A-Fa-f]"));
    
    // If the country code is a single character, try to convert to hex
    if (normalized.length() == 1) {
        bool ok;
        char c = normalized.at(0).toLatin1();
        if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) {
            normalized = QString::number(static_cast<int>(c), 16).toUpper();
        }
    }
    
    // Ensure we have 2 hex digits
    normalized = normalized.rightJustified(2, '0');
    
    return normalized;
}

QStringList DatabaseManager::generatePossibleRomIds(uint32_t crc1, uint32_t crc2, const QString& countryCode) {
    QStringList results;
    
    QString formattedCRC1 = formatCRC(crc1);
    QString formattedCRC2 = formatCRC(crc2);
    QString normalizedCountry = normalizeCountryCode(countryCode);
    
    // Generate various formats that might be in the database
    
    // 1. Standard format
    QString standard = QString("%1-%2-C:%3")
        .arg(formattedCRC1)
        .arg(formattedCRC2)
        .arg(normalizedCountry);
    results << standard;
    
    // 2. With brackets
    results << QString("[%1]").arg(standard);
    
    // 3. Without country code
    QString withoutCountry = QString("%1-%2")
        .arg(formattedCRC1)
        .arg(formattedCRC2);
    results << withoutCountry;
    
    // 4. Without country code with brackets
    results << QString("[%1]").arg(withoutCountry);
    
    // 5. Try common country code alternatives (45=USA, 4A=Japan, 50=Europe, etc.)
    QStringList commonCountries = {"45", "4A", "50", "49", "53", "55", "46", "58", "59"};
    for (const QString& cc : commonCountries) {
        if (cc != normalizedCountry) {
            results << QString("%1-%2-C:%3")
                .arg(formattedCRC1)
                .arg(formattedCRC2)
                .arg(cc);
                
            results << QString("[%1-%2-C:%3]")
                .arg(formattedCRC1)
                .arg(formattedCRC2)
                .arg(cc);
        }
    }
    
    return results;
}

int DatabaseManager::getGameIdFromCRC(uint32_t crc1, uint32_t crc2, const QString& countryCode) {
    QStringList possibleIds = generatePossibleRomIds(crc1, crc2, countryCode);
    
    // Build a query with all possible ROM IDs
    QStringList conditions;
    for (const QString& id : possibleIds) {
        conditions << QString("rom_id = '%1'").arg(id);
    }
    
    QString query = QString("SELECT id FROM games WHERE %1 LIMIT 1").arg(conditions.join(" OR "));
    
    QVariant result = executeSingleValueQuery(query);
    if (result.isValid()) {
        return result.toInt();
    }
    
    return -1; // Game not found
}

std::map<QString, QVariant> DatabaseManager::getRomInfoByCRC(uint32_t crc1, uint32_t crc2, const QString& countryCode) {
    QStringList possibleIds = generatePossibleRomIds(crc1, crc2, countryCode);
    
    // Build a query with all possible ROM IDs
    QStringList conditions;
    for (const QString& id : possibleIds) {
        conditions << QString("rom_id = '%1'").arg(id);
    }
    
    QString query = QString("SELECT * FROM games WHERE %1 LIMIT 1").arg(conditions.join(" OR "));
    
    return executeSingleRowQuery(query);
}

std::map<QString, QVariant> DatabaseManager::getRomInfoByCRCWithoutCountry(uint32_t crc1, uint32_t crc2) {
    // Try to find a ROM by CRC values only, ignoring country code
    QString crcOnly = QString("%1-%2")
        .arg(formatCRC(crc1))
        .arg(formatCRC(crc2));
    
    // Try both with and without brackets, using LIKE to match any country code
    QString query = QString("SELECT * FROM games WHERE rom_id LIKE '%1%' OR rom_id LIKE '[%1]%' LIMIT 1")
        .arg(crcOnly);
    
    return executeSingleRowQuery(query);
}

std::map<QString, QVariant> DatabaseManager::getRomInfoByCartridgeCode(const QString& cartridgeCode) {
    if (cartridgeCode.isEmpty()) {
        return std::map<QString, QVariant>();
    }
    
    // Find games with matching cartridge code (exact or partial match)
    QString query = QString("SELECT * FROM games WHERE cartridge_code = '%1' OR cartridge_code LIKE '%1%' LIMIT 1")
        .arg(cartridgeCode);
    
    return executeSingleRowQuery(query);
}

std::map<QString, QVariant> DatabaseManager::getRomInfoByInternalName(const QString& internalName) {
    if (internalName.isEmpty()) {
        return std::map<QString, QVariant>();
    }
    
    // Find games with matching internal name (exact or cleaned version)
    QString cleanedName = internalName;
    cleanedName.remove(QRegularExpression("[^a-zA-Z0-9]")); // Remove special characters
    
    QString query = QString("SELECT * FROM games WHERE internal_name = '%1' OR internal_name = '%2' LIMIT 1")
        .arg(internalName)
        .arg(cleanedName);
    
    return executeSingleRowQuery(query);
}

bool DatabaseManager::viewExists(const QString& viewName) const {
    if (!m_db.isOpen()) {
        return false;
    }
    
    // Check if view exists in sqlite_master
    QSqlQuery query(m_db);
    query.prepare("SELECT name FROM sqlite_master WHERE type='view' AND name=:viewName");
    query.bindValue(":viewName", viewName);
    
    if (!query.exec() || !query.next()) {
        return false;
    }
    
    return true;
}

std::vector<std::map<QString, QVariant>> DatabaseManager::getAllRomBrowserEntries() {
    if (!viewExists("rom_browser_view")) {
        qWarning() << "rom_browser_view not found, falling back to slower method";
        return getAllGames();
    }
    
    return executeQuery("SELECT * FROM rom_browser_view");
}

std::map<QString, QVariant> DatabaseManager::getRomBrowserEntryByCRC(uint32_t crc1, uint32_t crc2, const QString& countryCode) {
    if (!viewExists("rom_browser_view")) {
        return getRomCompleteInfo(crc1, crc2, countryCode);
    }
    
    QStringList possibleIds = generatePossibleRomIds(crc1, crc2, countryCode);
    
    // Build a query with all possible ROM IDs
    QStringList conditions;
    for (const QString& id : possibleIds) {
        conditions << QString("rom_id = '%1'").arg(id);
    }
    
    QString query = QString("SELECT * FROM rom_browser_view WHERE %1 LIMIT 1").arg(conditions.join(" OR "));
    
    return executeSingleRowQuery(query);
}

std::map<QString, QVariant> DatabaseManager::getRomBrowserEntryByRomId(const QString& romId) {
    if (!viewExists("rom_browser_view")) {
        return getGameByRomId(romId);
    }
    
    QString query = QString("SELECT * FROM rom_browser_view WHERE rom_id = '%1' LIMIT 1").arg(romId);
    return executeSingleRowQuery(query);
}

std::vector<std::map<QString, QVariant>> DatabaseManager::searchRomBrowserEntries(const QString& searchTerm, int limit) {
    if (!viewExists("rom_browser_view")) {
        // Fallback to a more complex join query if the view doesn't exist
        QString query = QString(
            "SELECT g.*, r.code AS region_code, r.name AS region_name, "
            "d.name AS developer_name, gen.name AS genre_name, cc.name AS cartridge_color_name "
            "FROM games g "
            "LEFT JOIN regions r ON g.region_id = r.id "
            "LEFT JOIN developers d ON g.developer_id = d.id "
            "LEFT JOIN genres gen ON g.genre_id = gen.id "
            "LEFT JOIN cartridge_colors cc ON g.cartridge_color_id = cc.id "
            "WHERE g.good_name LIKE '%%%1%%' OR g.internal_name LIKE '%%%1%%' OR g.cartridge_code LIKE '%%%1%%' "
            "LIMIT %2").arg(searchTerm).arg(limit);
        
        return executeQuery(query);
    }
    
    QString query = QString(
        "SELECT * FROM rom_browser_view "
        "WHERE good_name LIKE '%%%1%%' OR internal_name LIKE '%%%1%%' OR cartridge_code LIKE '%%%1%%' "
        "LIMIT %2").arg(searchTerm).arg(limit);
    
    return executeQuery(query);
}

// Optimize existing method to use the view when possible
std::map<QString, QVariant> DatabaseManager::getRomCompleteInfo(uint32_t crc1, uint32_t crc2, const QString& countryCode) {
    // Try getting info from the view first if it exists
    if (viewExists("rom_browser_view")) {
        auto gameInfo = getRomBrowserEntryByCRC(crc1, crc2, countryCode);
        if (!gameInfo.empty()) {
            return gameInfo;
        }
    }
    
    // Otherwise use the original implementation as fallback
    // Get the basic ROM info
    auto gameInfo = getRomInfoByCRC(crc1, crc2, countryCode);
    
    if (gameInfo.empty()) {
        // Try fallbacks
        gameInfo = getRomInfoByCRCWithoutCountry(crc1, crc2);
        
        if (gameInfo.empty()) {
            return gameInfo; // ROM not found
        }
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

QString DatabaseManager::getGameGoodName(uint32_t crc1, uint32_t crc2, const QString& countryCode) {
    auto info = getRomInfoByCRC(crc1, crc2, countryCode);
    return info.count("good_name") ? info["good_name"].toString() : QString();
}

QString DatabaseManager::getGameInternalName(uint32_t crc1, uint32_t crc2, const QString& countryCode) {
    auto info = getRomInfoByCRC(crc1, crc2, countryCode);
    return info.count("internal_name") ? info["internal_name"].toString() : QString();
}

QString DatabaseManager::getGameCartridgeCode(uint32_t crc1, uint32_t crc2, const QString& countryCode) {
    auto info = getRomInfoByCRC(crc1, crc2, countryCode);
    return info.count("cartridge_code") ? info["cartridge_code"].toString() : QString();
}

QString DatabaseManager::getGameDeveloper(uint32_t crc1, uint32_t crc2, const QString& countryCode) {
    auto info = getRomCompleteInfo(crc1, crc2, countryCode);
    return info.count("developer_name") ? info["developer_name"].toString() : QString();
}

QString DatabaseManager::getGameGenre(uint32_t crc1, uint32_t crc2, const QString& countryCode) {
    auto info = getRomCompleteInfo(crc1, crc2, countryCode);
    return info.count("genre_name") ? info["genre_name"].toString() : QString();
}

QString DatabaseManager::getGameReleaseDate(uint32_t crc1, uint32_t crc2, const QString& countryCode) {
    auto info = getRomInfoByCRC(crc1, crc2, countryCode);
    return info.count("release_date") ? info["release_date"].toString() : QString();
}

int DatabaseManager::getGamePlayers(uint32_t crc1, uint32_t crc2, const QString& countryCode) {
    auto info = getRomInfoByCRC(crc1, crc2, countryCode);
    return info.count("players") ? info["players"].toInt() : 1;
}

bool DatabaseManager::getGameForceFeedback(uint32_t crc1, uint32_t crc2, const QString& countryCode) {
    auto info = getRomInfoByCRC(crc1, crc2, countryCode);
    return info.count("force_feedback") ? info["force_feedback"].toBool() : false;
}

QString DatabaseManager::getGameStatus(uint32_t crc1, uint32_t crc2, const QString& countryCode) {
    auto info = getRomInfoByCRC(crc1, crc2, countryCode);
    return info.count("status") ? info["status"].toString() : QString("Unknown");
}

QString DatabaseManager::getGameRegion(uint32_t crc1, uint32_t crc2, const QString& countryCode) {
    auto info = getRomCompleteInfo(crc1, crc2, countryCode);
    return info.count("region_name") ? info["region_name"].toString() : QString();
}

QString DatabaseManager::getGameCartridgeColor(uint32_t crc1, uint32_t crc2, const QString& countryCode) {
    auto info = getRomCompleteInfo(crc1, crc2, countryCode);
    return info.count("cartridge_color_name") ? info["cartridge_color_name"].toString() : QString();
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
    
    QVariant result = executeSingleValueQuery(query);
    return result.isValid() ? result.toString() : QString();
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

QString DatabaseManager::getCoreSettingByCRC(uint32_t crc1, uint32_t crc2, const QString& countryCode, const QString& settingName) {
    int gameId = getGameIdFromCRC(crc1, crc2, countryCode);
    if (gameId > 0) {
        return getCoreSetting(gameId, settingName);
    }
    return QString();
}

QString DatabaseManager::getVideoSettingByCRC(uint32_t crc1, uint32_t crc2, const QString& countryCode, const QString& settingName) {
    int gameId = getGameIdFromCRC(crc1, crc2, countryCode);
    if (gameId > 0) {
        return getVideoSetting(gameId, settingName);
    }
    return QString();
}

QString DatabaseManager::getAudioSettingByCRC(uint32_t crc1, uint32_t crc2, const QString& countryCode, const QString& settingName) {
    int gameId = getGameIdFromCRC(crc1, crc2, countryCode);
    if (gameId > 0) {
        return getAudioSetting(gameId, settingName);
    }
    return QString();
}

bool DatabaseManager::isDatabaseLoaded() const {
    return m_db.isOpen();
}

QString DatabaseManager::getDatabaseVersion() const {
    if (!m_db.isOpen()) {
        return QString();
    }
    
    // Try to get database version from metadata table
    QVariant version = executeSingleValueQuery("SELECT value FROM metadata WHERE key = 'version' LIMIT 1");
    return version.isValid() ? version.toString() : QString("Unknown");
}

QStringList DatabaseManager::getTables() const {
    if (!m_db.isOpen()) {
        return QStringList();
    }
    
    return m_db.tables();
}

QStringList DatabaseManager::getColumns(const QString& tableName) const {
    QStringList columns;
    
    if (!m_db.isOpen()) {
        return columns;
    }
    
    QSqlQuery query(m_db);
    QString sql = QString("PRAGMA table_info(%1)").arg(tableName);
    
    if (!query.exec(sql)) {
        qWarning() << "Failed to get columns for" << tableName << ":" << query.lastError().text();
        return columns;
    }
    
    while (query.next()) {
        QString columnName = query.value("name").toString();
        QString type = query.value("type").toString();
        bool notNull = query.value("notnull").toBool();
        QVariant defaultValue = query.value("dflt_value");
        bool isPk = query.value("pk").toBool();
        
        QString columnInfo = columnName + " (" + type;
        if (isPk) columnInfo += ", PRIMARY KEY";
        if (notNull) columnInfo += ", NOT NULL";
        if (defaultValue.isValid()) columnInfo += ", DEFAULT=" + defaultValue.toString();
        columnInfo += ")";
        
        columns << columnInfo;
    }
    
    return columns;
}

void DatabaseManager::logDatabaseSchema() const {
    if (!m_db.isOpen()) {
        qWarning() << "Cannot log schema: Database not open";
        return;
    }
    
    qDebug() << "=== DATABASE SCHEMA ===";
    
    // Just log basic database info and size
    QFile dbFile(m_dbPath);
    if (dbFile.exists()) {
        qDebug() << "Size:" << QString::number(dbFile.size() / 1024) << "KB";
    }
    
    QStringList tables = getTables();
    qDebug() << "Tables (" << tables.size() << "):" << tables.join(", ");
    
    // Skip detailed column logging unless needed for debugging
    // Uncomment the following code for detailed schema logging if needed
    /*
    if (!tables.isEmpty()) {
        for (const QString& table : tables) {
            QStringList columns = getColumns(table);
            qDebug() << "Table:" << table << "has" << columns.size() << "columns:";
            for (const QString& column : columns) {
                qDebug() << "  -" << column;
            }
        }
    }
    */
    
    qDebug() << "=== END DATABASE SCHEMA ===";
}

void DatabaseManager::logBasicDatabaseInfo() const {
    if (!m_db.isOpen()) {
        return;
    }
    
    QStringList tables = getTables();
    if (tables.size() < 10) {
        // Only log tables if there are few of them - might indicate a problem
        qDebug() << "Database contains only" << tables.size() << "tables:" << tables.join(", ");
    }
    
    // Log version only on debug builds or if it's missing
    QVariant version = executeSingleValueQuery("SELECT value FROM metadata WHERE key = 'version' LIMIT 1");
    if (!version.isValid()) {
        qWarning() << "Database has no version information";
    }
}

QString DatabaseManager::getDatabaseInfo() const {
    QString info;
    
    if (!m_db.isOpen()) {
        return "Database not open";
    }
    
    // Database type
    info += "Type: " + m_db.driverName() + "\n";
    
    // Connection name
    info += "Connection name: " + m_db.connectionName() + "\n";
    
    // Database path
    info += "Path: " + m_dbPath + "\n";
    
    // Database size
    QFile dbFile(m_dbPath);
    if (dbFile.exists()) {
        info += "Size: " + QString::number(dbFile.size() / 1024) + " KB\n";
    }
    
    // SQLite version
    QVariant versionVar = executeSingleValueQuery("SELECT sqlite_version()");
    if (versionVar.isValid()) {
        info += "SQLite version: " + versionVar.toString() + "\n";
    }
    
    return info;
}