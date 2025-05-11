#include "RomInfoProvider.h"
#include <QDir>
#include <QFileInfo>
#include <QSettings>
#include <QCoreApplication>
#include <QDebug>
#include <QFile>

namespace QT_UI {

// Initialize static country names map
QMap<CountryCode, QString> RomInfoProvider::m_countryNames;

RomInfoProvider::RomInfoProvider() : 
    m_romSize(0),
    m_country(Country_Unknown),
    m_cicChip(CIC_UNKNOWN),
    m_crc1(0),
    m_crc2(0),
    m_fileFormat(Format_Uncompressed),
    m_developer(""),
    m_releaseDate(""),
    m_genre(""),
    m_players(1),
    m_forceFeedback(false),
    m_productID("")
{
    if (m_countryNames.isEmpty()) {
        initializeCountryNames();
    }
}

RomInfoProvider::~RomInfoProvider()
{
}

void RomInfoProvider::initializeCountryNames()
{
    m_countryNames[Country_USA] = "USA";
    m_countryNames[Country_Germany] = "Germany";
    m_countryNames[Country_Japan] = "Japan";
    m_countryNames[Country_Europe] = "Europe";
    m_countryNames[Country_Italy] = "Italy";
    m_countryNames[Country_Spain] = "Spain";
    m_countryNames[Country_Australia] = "Australia";
    m_countryNames[Country_France] = "France";
    m_countryNames[Country_Unknown2] = "PAL";
    m_countryNames[Country_Unknown3] = "PAL";
    m_countryNames[Country_Unknown] = "Unknown";
}

bool RomInfoProvider::openRomFile(const QString& filePath)
{
    m_filePath = filePath;
    
    QFileInfo fileInfo(filePath);
    m_fileName = fileInfo.fileName();
    
    // Determine file format
    QString extension = fileInfo.suffix().toLower();
    if (extension == "zip") {
        m_fileFormat = Format_Zip;
        // TODO: Extract ROM data from zip
        return false; // Not implemented for this example
    }
    else if (extension == "7z") {
        m_fileFormat = Format_7zip;
        // TODO: Extract ROM data from 7z
        return false; // Not implemented for this example
    }
    else {
        m_fileFormat = Format_Uncompressed;
    }
    
    // Read ROM header - first 64 bytes
    QFile romFile(filePath);
    if (!romFile.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open ROM file:" << filePath;
        return false;
    }
    
    m_RomHeader = romFile.read(0x40); // Read first 64 bytes header
    if (m_RomHeader.size() < 0x40) {
        qWarning() << "ROM file too small:" << filePath;
        return false;
    }
    
    // Get ROM size
    m_romSize = romFile.size();
    romFile.close();
    
    // Parse header information
    parseRomHeader();
    
    // Calculate CRCs
    calculateCRC();
    
    // Load ROM database information
    loadRomInformation();
    
    return true;
}

void RomInfoProvider::parseRomHeader()
{
    // Extract internal name from header (offset 0x20, length 20 bytes)
    m_internalName = QString::fromUtf8(m_RomHeader.mid(0x20, 20).constData()).trimmed();
    
    // Extract Cartridge ID (offset 0x3C, length 2 bytes)
    m_cartID = QString::fromUtf8(m_RomHeader.mid(0x3C, 2).constData()).trimmed();
    
    // Extract country code (offset 0x3E, 1 byte)
    char countryByte = m_RomHeader.at(0x3E);
    m_country = charToCountryCode(countryByte);
}

void RomInfoProvider::calculateCRC()
{
    // For a basic implementation, we'll just extract CRCs from the ROM header
    // In real implementation, you would calculate proper CRCs across the entire ROM
    
    if (m_RomHeader.size() >= 0x18) {
        // We need to read bytes as unsigned values to avoid sign extension issues
        m_crc1 = ((static_cast<uint32_t>(static_cast<unsigned char>(m_RomHeader[0x10])) << 24) |
                 (static_cast<uint32_t>(static_cast<unsigned char>(m_RomHeader[0x11])) << 16) |
                 (static_cast<uint32_t>(static_cast<unsigned char>(m_RomHeader[0x12])) << 8) |
                 (static_cast<uint32_t>(static_cast<unsigned char>(m_RomHeader[0x13]))));
        
        m_crc2 = ((static_cast<uint32_t>(static_cast<unsigned char>(m_RomHeader[0x14])) << 24) |
                 (static_cast<uint32_t>(static_cast<unsigned char>(m_RomHeader[0x15])) << 16) |
                 (static_cast<uint32_t>(static_cast<unsigned char>(m_RomHeader[0x16])) << 8) |
                 (static_cast<uint32_t>(static_cast<unsigned char>(m_RomHeader[0x17]))));
        
        // Debug the actual bytes we're reading
        qDebug() << "Raw CRC1 bytes:" 
                 << QString::number(static_cast<unsigned char>(m_RomHeader[0x10]), 16).toUpper().rightJustified(2, '0')
                 << QString::number(static_cast<unsigned char>(m_RomHeader[0x11]), 16).toUpper().rightJustified(2, '0')
                 << QString::number(static_cast<unsigned char>(m_RomHeader[0x12]), 16).toUpper().rightJustified(2, '0')
                 << QString::number(static_cast<unsigned char>(m_RomHeader[0x13]), 16).toUpper().rightJustified(2, '0');
                 
        qDebug() << "Raw CRC2 bytes:" 
                 << QString::number(static_cast<unsigned char>(m_RomHeader[0x14]), 16).toUpper().rightJustified(2, '0')
                 << QString::number(static_cast<unsigned char>(m_RomHeader[0x15]), 16).toUpper().rightJustified(2, '0')
                 << QString::number(static_cast<unsigned char>(m_RomHeader[0x16]), 16).toUpper().rightJustified(2, '0')
                 << QString::number(static_cast<unsigned char>(m_RomHeader[0x17]), 16).toUpper().rightJustified(2, '0');
                 
        qDebug() << "Calculated CRC1:" << QString::number(m_crc1, 16).toUpper().rightJustified(8, '0')
                 << "CRC2:" << QString::number(m_crc2, 16).toUpper().rightJustified(8, '0');
    }
}

bool RomInfoProvider::loadRomInformation()
{
    // Parse the ROM header first
    parseRomHeader();
    
    // Calculate CRC if not already done
    calculateCRC();
    
    // Try to load extra information from RDB/RDX
    loadRDBInfo();
    
    return true;
}

void RomInfoProvider::loadRDBInfo()
{
    // Get the country code byte (offset 0x3E, 1 byte)
    unsigned char countryByte = 0;
    if (m_RomHeader.size() >= 0x3F) {
        countryByte = static_cast<unsigned char>(m_RomHeader[0x3E]);
    }
    
    // Format country code consistently with database format (hex value without sign extension)
    QString countryHex = QString::number(countryByte, 16).toUpper().rightJustified(2, '0');
    qDebug() << "Raw Country Byte:" << QString::number(countryByte) << "Hex:" << countryHex;
    
    // Create both key formats to try - with and without brackets
    QString crcKey = QString("%1-%2-C:%3")
                         .arg(QString::number(m_crc1, 16).toUpper().rightJustified(8, '0'))
                         .arg(QString::number(m_crc2, 16).toUpper().rightJustified(8, '0'))
                         .arg(countryHex);
    
    QString bracketCrcKey = QString("[%1-%2-C:%3]")
                         .arg(QString::number(m_crc1, 16).toUpper().rightJustified(8, '0'))
                         .arg(QString::number(m_crc2, 16).toUpper().rightJustified(8, '0'))
                         .arg(countryHex);

    qDebug() << "Looking for ROM with CRC keys:" << crcKey << bracketCrcKey;

    // Check resources first, then fall back to file system
    QString rdbPath, rdxPath;
    bool useResources = false;

    // Check if resources exist using QFile::exists
    if (QFile::exists(":/database/Resources/data/Project64.rdb") && 
        QFile::exists(":/database/Resources/data/Project64.rdx")) {
        rdbPath = ":/database/Resources/data/Project64.rdb";
        rdxPath = ":/database/Resources/data/Project64.rdx";
        useResources = true;
        qDebug() << "Using RDB/RDX from Qt resources";
    } else {
        // Fall back to file system
        rdbPath = QDir(QCoreApplication::applicationDirPath()).filePath("Config/Project64.rdb");
        rdxPath = QDir(QCoreApplication::applicationDirPath()).filePath("Config/Project64.rdx");
        qDebug() << "Using RDB/RDX from file system:" << rdbPath << rdxPath;
    }

    // Load and check database files 
    QSettings rdbSettings(rdbPath, QSettings::IniFormat);
    QSettings rdxSettings(rdxPath, QSettings::IniFormat);
    
    QStringList rdbGroups = rdbSettings.childGroups();
    QStringList rdxGroups = rdxSettings.childGroups();
    
    qDebug() << "RDB contains" << rdbGroups.size() << "entries";
    qDebug() << "RDX contains" << rdxGroups.size() << "entries";
    
    // Debug the database keys to help diagnose the issue
    if (rdbGroups.size() < 50) { // Only dump if there aren't too many
        qDebug() << "RDB contains keys:" << rdbGroups.join(", ");
    } else {
        // Show a sample of keys
        qDebug() << "RDB sample keys:" << rdbGroups.mid(0, 5).join(", ");
    }
    
    // Generate all possible key formats for more flexibility
    QStringList possibleKeys = {bracketCrcKey, crcKey};
    
    // Try without country code as a fallback
    possibleKeys.append(QString("%1-%2")
                        .arg(QString::number(m_crc1, 16).toUpper().rightJustified(8, '0'))
                        .arg(QString::number(m_crc2, 16).toUpper().rightJustified(8, '0')));
    
    possibleKeys.append(QString("[%1-%2]")
                        .arg(QString::number(m_crc1, 16).toUpper().rightJustified(8, '0'))
                        .arg(QString::number(m_crc2, 16).toUpper().rightJustified(8, '0')));
    
    // Try common country codes used in the database
    QStringList countryCodes = {"45", "44", "4A", "50", "49", "53", "55", "46", "58", "59", "40", "00"};
    for (const QString& cc : countryCodes) {
        possibleKeys.append(QString("[%1-%2-C:%3]")
                             .arg(QString::number(m_crc1, 16).toUpper().rightJustified(8, '0'))
                             .arg(QString::number(m_crc2, 16).toUpper().rightJustified(8, '0'))
                             .arg(cc));
    }
    
    //------------------------------------------------------
    // FIRST: Search RDB database for ROM information
    //------------------------------------------------------
    bool foundInRdb = false;
    QString foundKey;
    for (const QString& tryKey : possibleKeys) {
        if (rdbGroups.contains(tryKey)) {
            qDebug() << "Found ROM in RDB with key:" << tryKey;
            rdbSettings.beginGroup(tryKey);
            foundInRdb = true;
            foundKey = tryKey;
            break;
        }
    }
    
    // If found in RDB, process the information
    if (foundInRdb) {
        QStringList keys = rdbSettings.childKeys();
        qDebug() << "RDB entry contains keys:" << keys.join(", ");
        
        // Load available information
        if (rdbSettings.contains("Good Name")) {
            m_goodName = rdbSettings.value("Good Name").toString();
            qDebug() << "Loaded Good Name from RDB:" << m_goodName;
        }
        
        if (rdbSettings.contains("Internal Name")) {
            // If internal name is blank or missing from ROM header, use from RDB
            if (m_internalName.isEmpty()) {
                m_internalName = rdbSettings.value("Internal Name").toString();
                qDebug() << "Loaded Internal Name from RDB:" << m_internalName;
            }
        }
        
        // Some RDB files might contain extended information directly
        if (rdbSettings.contains("Developer")) {
            m_developer = rdbSettings.value("Developer").toString();
            qDebug() << "Loaded Developer from RDB:" << m_developer;
        }
        
        if (rdbSettings.contains("Release Date")) {
            m_releaseDate = rdbSettings.value("Release Date").toString();
            qDebug() << "Loaded Release Date from RDB:" << m_releaseDate;
        }
        
        rdbSettings.endGroup();
    } else {
        qDebug() << "ROM not found in RDB";
    }
    
    //------------------------------------------------------
    // SECOND: Search RDX database for extended information
    //------------------------------------------------------
    bool foundInRdx = false;
    for (const QString& tryKey : possibleKeys) {
        if (rdxGroups.contains(tryKey)) {
            qDebug() << "Found ROM in RDX with key:" << tryKey;
            rdxSettings.beginGroup(tryKey);
            foundInRdx = true;
            break;
        }
    }
    
    // If found in RDX, load the extended information
    if (foundInRdx) {
        QStringList keys = rdxSettings.childKeys();
        qDebug() << "RDX entry contains keys:" << keys.join(", ");
        
        // Good Name might be in the RDX too
        if (rdxSettings.contains("Good Name") && m_goodName.isEmpty()) {
            m_goodName = rdxSettings.value("Good Name").toString();
            qDebug() << "Loaded Good Name from RDX:" << m_goodName;
        }
        
        if (rdxSettings.contains("Developer")) {
            m_developer = rdxSettings.value("Developer").toString();
            qDebug() << "Loaded Developer from RDX:" << m_developer;
        }
        
        if (rdxSettings.contains("ReleaseDate")) {
            m_releaseDate = rdxSettings.value("ReleaseDate").toString();
            qDebug() << "Loaded ReleaseDate from RDX:" << m_releaseDate;
        } else if (rdxSettings.contains("Release Date")) {
            // Try alternative key name
            m_releaseDate = rdxSettings.value("Release Date").toString();
            qDebug() << "Loaded Release Date from RDX:" << m_releaseDate;
        }
        
        if (rdxSettings.contains("Genre")) {
            m_genre = rdxSettings.value("Genre").toString();
            qDebug() << "Loaded Genre from RDX:" << m_genre;
        }
        
        if (rdxSettings.contains("Players")) {
            bool ok;
            int players = rdxSettings.value("Players").toInt(&ok);
            if (ok) {
                m_players = players;
                qDebug() << "Loaded Players from RDX:" << m_players;
            }
        }
        
        if (rdxSettings.contains("ForceFeedback")) {
            QString ffValue = rdxSettings.value("ForceFeedback").toString();
            m_forceFeedback = (ffValue.compare("Yes", Qt::CaseInsensitive) == 0) || 
                              (ffValue.compare("true", Qt::CaseInsensitive) == 0) ||
                              (ffValue.toInt() != 0);
            qDebug() << "Loaded ForceFeedback from RDX:" << m_forceFeedback;
        }
        
        // Load ProductID if available
        if (rdxSettings.contains("ProductID")) {
            m_productID = rdxSettings.value("ProductID").toString();
            qDebug() << "Loaded ProductID from RDX:" << m_productID;
        }
        
        rdxSettings.endGroup();
    } else {
        qDebug() << "ROM not found in RDX";
    }

    // Debug output to verify data loading
    qDebug() << "ROM info loaded - Good Name:" << m_goodName
             << "Developer:" << m_developer 
             << "Release Date:" << m_releaseDate
             << "Genre:" << m_genre 
             << "Players:" << m_players
             << "ProductID:" << m_productID;
}

QString RomInfoProvider::getInternalName() const
{
    return m_internalName;
}

QString RomInfoProvider::getFileName() const
{
    return m_fileName;
}

QString RomInfoProvider::getGoodName() const
{
    return m_goodName;
}

QString RomInfoProvider::getCartID() const
{
    return m_cartID;
}

QString RomInfoProvider::getMediaType() const
{
    if (m_RomHeader.size() < 0x3F) {
        return "Unknown";
    }
    
    char mediaType = m_RomHeader.at(0x3F);
    switch (mediaType) {
        case 'C': return "N64 Cartridge (Disk Compatible)";
        case 'D': return "64DD Disk";
        case 'E': return "64DD Disk (Expansion)";
        case 'M': return "N64 Development Cartridge";
        case 'N': return "N64 Cartridge";
        case 'Z': return "Aleck64";
        case 0:   return "None";
        default:  return QString("Unknown (%1)").arg(QChar(mediaType));
    }
}

int RomInfoProvider::getRomSize() const
{
    return m_romSize;
}

QString RomInfoProvider::getCountryName() const
{
    return countryCodeToName(m_country);
}

CountryCode RomInfoProvider::getCountryCode() const
{
    return m_country;
}

CICChip RomInfoProvider::getCICChip() const
{
    return m_cicChip;
}

uint32_t RomInfoProvider::getCRC1() const
{
    return m_crc1;
}

uint32_t RomInfoProvider::getCRC2() const
{
    return m_crc2;
}

QString RomInfoProvider::getManufacturerID() const
{
    return ""; // Not implemented for this example
}

RomFileFormat RomInfoProvider::getFileFormat() const
{
    return m_fileFormat;
}

QString RomInfoProvider::getDeveloper() const
{
    return m_developer;
}

QString RomInfoProvider::getReleaseDate() const
{
    return m_releaseDate;
}

QString RomInfoProvider::getGenre() const
{
    return m_genre;
}

int RomInfoProvider::getPlayers() const
{
    return m_players;
}

bool RomInfoProvider::getForceFeedback() const
{
    return m_forceFeedback;
}

QString RomInfoProvider::getProductID() const
{
    return m_productID;
}

QString RomInfoProvider::countryCodeToName(CountryCode countryCode)
{
    return m_countryNames.value(countryCode, "Unknown");
}

CountryCode RomInfoProvider::charToCountryCode(char countryChar)
{
    switch (countryChar) {
        case 0x45: return Country_USA;
        case 0x44: return Country_Germany;
        case 0x4A: return Country_Japan;
        case 0x50: return Country_Europe;
        case 0x49: return Country_Italy;
        case 0x53: return Country_Spain;
        case 0x55: return Country_Australia;
        case 0x46: return Country_France;
        case 0x58: return Country_Unknown2; // X - PAL
        case 0x59: return Country_Unknown3; // Y - PAL
        default:   return Country_Unknown;
    }
}

} // namespace QT_UI
