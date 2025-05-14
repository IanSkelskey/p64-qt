#include "RomBrowserPopulator.h"
#include <QDebug>

namespace QT_UI {

RomBrowserPopulator::RomBrowserPopulator(DatabaseManager* dbManager, QObject* parent)
    : QObject(parent), 
      m_dbManager(dbManager), 
      m_isPreloaded(false)
{
    if (!m_dbManager) {
        qWarning() << "RomBrowserPopulator initialized with null DatabaseManager!";
    }
}

RomBrowserPopulator::~RomBrowserPopulator()
{
    // Clear cached data
    m_romEntries.clear();
    m_romCrcIndex.clear();
}

void RomBrowserPopulator::preloadRomEntries()
{
    if (!m_dbManager || !m_dbManager->isDatabaseLoaded()) {
        qWarning() << "Cannot preload ROM entries: database not loaded";
        return;
    }
    
    // Clear any existing data
    m_romEntries.clear();
    m_romCrcIndex.clear();
    
    // Load all entries from the rom_browser_view
    m_romEntries = m_dbManager->getAllRomBrowserEntries();
    
    // Build indices for fast lookups
    for (size_t i = 0; i < m_romEntries.size(); ++i) {
        const auto& entry = m_romEntries[i];
        
        // If we have a rom_id, create an index for it
        if (entry.count("rom_id") && entry.at("rom_id").isValid()) {
            QString romId = entry.at("rom_id").toString();
            
            // Parse the ROM ID to get CRCs if possible
            QStringList parts = romId.split("-");
            if (parts.size() >= 2) {
                QString crc1Str = parts[0];
                QString crc2Str = parts[1];
                
                // Clean up any brackets
                crc1Str.remove('[').remove(']');
                crc2Str.remove('[').remove(']');
                
                // Create index entries for different possible country codes
                for (int countryCode = 0; countryCode < 256; ++countryCode) {
                    QString countryHex = QString::number(countryCode, 16).toUpper().rightJustified(2, '0');
                    QString crcKey = crc1Str + "-" + crc2Str + "-" + countryHex;
                    m_romCrcIndex[crcKey] = static_cast<int>(i);
                }
            }
        }
    }
    
    m_isPreloaded = true;
    emit preloadComplete(static_cast<int>(m_romEntries.size()));
    
    qDebug() << "ROM browser preloaded" << m_romEntries.size() << "entries with" 
             << m_romCrcIndex.size() << "index entries";
}

std::map<QString, QVariant> RomBrowserPopulator::getRomInfoByCRC(uint32_t crc1, uint32_t crc2, const QString& countryCode)
{
    // If we have preloaded data, use the indices for fast lookup
    if (m_isPreloaded) {
        QString crcKey = createCrcKey(crc1, crc2, countryCode);
        
        if (m_romCrcIndex.contains(crcKey)) {
            int index = m_romCrcIndex[crcKey];
            if (index >= 0 && index < static_cast<int>(m_romEntries.size())) {
                return m_romEntries[index];
            }
        }
        
        // Try without country code
        crcKey = DatabaseManager::formatCRC(crc1) + "-" + DatabaseManager::formatCRC(crc2);
        
        // Search through all entries (slower fallback)
        for (const auto& entry : m_romEntries) {
            if (entry.count("rom_id") && entry.at("rom_id").isValid()) {
                QString romId = entry.at("rom_id").toString();
                if (romId.startsWith(crcKey, Qt::CaseInsensitive)) {
                    return entry;
                }
            }
        }
        
        return std::map<QString, QVariant>();
    } else {
        // If not preloaded, use database directly
        return m_dbManager->getRomBrowserEntryByCRC(crc1, crc2, countryCode);
    }
}

std::vector<std::map<QString, QVariant>> RomBrowserPopulator::searchRoms(const QString& searchTerm)
{
    if (searchTerm.isEmpty()) {
        return m_isPreloaded ? m_romEntries : m_dbManager->getAllRomBrowserEntries();
    }
    
    std::vector<std::map<QString, QVariant>> results;
    
    // If preloaded, search in memory
    if (m_isPreloaded) {
        QString lowerSearchTerm = searchTerm.toLower();
        
        for (const auto& entry : m_romEntries) {
            bool match = false;
            
            // Check good_name
            if (entry.count("good_name") && entry.at("good_name").isValid()) {
                if (entry.at("good_name").toString().toLower().contains(lowerSearchTerm)) {
                    match = true;
                }
            }
            
            // Check internal_name
            if (!match && entry.count("internal_name") && entry.at("internal_name").isValid()) {
                if (entry.at("internal_name").toString().toLower().contains(lowerSearchTerm)) {
                    match = true;
                }
            }
            
            // Check cartridge_code
            if (!match && entry.count("cartridge_code") && entry.at("cartridge_code").isValid()) {
                if (entry.at("cartridge_code").toString().toLower().contains(lowerSearchTerm)) {
                    match = true;
                }
            }
            
            if (match) {
                results.push_back(entry);
            }
        }
    } else {
        // Otherwise, query the database
        results = m_dbManager->searchRomBrowserEntries(searchTerm);
    }
    
    return results;
}

std::vector<std::map<QString, QVariant>> RomBrowserPopulator::getAllRomEntries()
{
    return m_isPreloaded ? m_romEntries : m_dbManager->getAllRomBrowserEntries();
}

bool RomBrowserPopulator::isRomDatabaseLoaded() const
{
    return m_isPreloaded || (m_dbManager && m_dbManager->isDatabaseLoaded());
}

QString RomBrowserPopulator::createCrcKey(uint32_t crc1, uint32_t crc2, const QString& countryCode)
{
    return DatabaseManager::formatCRC(crc1) + "-" + 
           DatabaseManager::formatCRC(crc2) + "-" + 
           DatabaseManager::normalizeCountryCode(countryCode);
}

} // namespace QT_UI
