#pragma once

#include <QObject>
#include <QMap>
#include <QString>
#include <QVariant>
#include <vector>
#include <map>
#include "DatabaseManager.h"

namespace QT_UI {

class RomBrowserPopulator : public QObject {
    Q_OBJECT
    
public:
    explicit RomBrowserPopulator(DatabaseManager* dbManager, QObject* parent = nullptr);
    ~RomBrowserPopulator();
    
    // Pre-fetch all ROM entries for faster lookups
    void preloadRomEntries();
    
    // Get ROM info by CRC
    std::map<QString, QVariant> getRomInfoByCRC(uint32_t crc1, uint32_t crc2, const QString& countryCode);
    
    // Search for ROMs
    std::vector<std::map<QString, QVariant>> searchRoms(const QString& searchTerm);
    
    // Get all loaded ROMs
    std::vector<std::map<QString, QVariant>> getAllRomEntries();
    
    // Check if the ROM database is loaded and contains entries
    bool isRomDatabaseLoaded() const;
    
signals:
    void preloadComplete(int count);
    
private:
    DatabaseManager* m_dbManager;
    std::vector<std::map<QString, QVariant>> m_romEntries;
    QMap<QString, int> m_romCrcIndex; // Maps CRC strings to indices in m_romEntries
    bool m_isPreloaded;
    
    // Helper function to create a CRC key
    QString createCrcKey(uint32_t crc1, uint32_t crc2, const QString& countryCode);
};

} // namespace QT_UI
