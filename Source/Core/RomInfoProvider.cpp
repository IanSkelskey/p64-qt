#include "RomInfoProvider.h"
#include "RomParser.h"
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
    m_byteFormat(Format_Unknown),
    m_developer(""),
    m_releaseDate(""),
    m_genre(""),
    m_players(1),
    m_forceFeedback(false),
    m_productID(""),
    m_status("Unknown"),  // Initialize status
    m_romParser(new RomParser())
{
    if (m_countryNames.isEmpty()) {
        initializeCountryNames();
    }
}

RomInfoProvider::~RomInfoProvider()
{
    delete m_romParser;
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
    
    // Set the header data in the parser
    m_romParser->setRomData(m_RomHeader);
    
    // Detect ROM byte format (Z64, N64, V64)
    m_byteFormat = m_romParser->detectByteFormat();
    qDebug() << "Detected ROM format:" << 
        (m_byteFormat == Format_Z64 ? "Z64 (Big Endian)" : 
         m_byteFormat == Format_N64 ? "N64 (Byte-swapped)" : 
         m_byteFormat == Format_V64 ? "V64 (Little Endian)" : "Unknown");
    
    // Get ROM size
    m_romSize = romFile.size();
    romFile.close();
    
    // Parse header information
    parseRomHeader();
    
    // Calculate CRCs
    calculateCRC();
    
    // Detect CIC chip based on ROM header information
    detectCICChip();
    
    // Load ROM database information
    loadRomInformation();
    
    return true;
}

void RomInfoProvider::parseRomHeader()
{
    // Use the parser to extract information from the header
    m_internalName = m_romParser->extractInternalName();
    m_cartID = m_romParser->extractCartID();
    m_country = m_romParser->extractCountryCode();
}

void RomInfoProvider::calculateCRC()
{
    // Use the parser to calculate CRCs
    m_romParser->calculateCRC(m_crc1, m_crc2);
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
    // Get the country code byte from the parser
    unsigned char countryByte = m_romParser->getRawCountryByte();
    
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
    if (QFile::exists(":/database/Project64.rdb") && 
        QFile::exists(":/database/Project64.rdx")) {
        rdbPath = ":/database/Project64.rdb";
        rdxPath = ":/database/Project64.rdx";
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
        
        // Add check for Status field
        if (rdbSettings.contains("Status")) {
            m_status = rdbSettings.value("Status").toString();
            qDebug() << "Loaded Status from RDB:" << m_status;
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
        
        // Load Status if available and not already set
        if (m_status == "Unknown" && rdxSettings.contains("Status")) {
            m_status = rdxSettings.value("Status").toString();
            qDebug() << "Loaded Status from RDX:" << m_status;
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
             << "Status:" << m_status
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
    QString mediaType = m_romParser->extractMediaType();
    qDebug() << "Extracted media type:" << mediaType;
    return mediaType;
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

// Add getter for byte format
RomByteFormat RomInfoProvider::getByteFormat() const
{
    return m_byteFormat;
}

// Add getter for status
QString RomInfoProvider::getStatus() const
{
    return m_status;
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

// Update CICChip detection in openRomFile or parseRomHeader method
void RomInfoProvider::detectCICChip() 
{
    // The CIC chip can often be detected by examining the ROM data at specific offsets
    // Here's a simplified detection method based on common CIC patterns

    // Default to unknown
    m_cicChip = CIC_UNKNOWN;
    
    // Check if we have enough data
    if (m_RomHeader.size() < 0x40)
        return;
        
    // Detect based on the ROM header checksum at 0x10-0x13 and other characteristics
    uint32_t crc1 = m_crc1;
    
    // Common CIC chip detection patterns (simplified approach)
    if (crc1 == 0x587BD543 || crc1 == 0x90D68C10 || crc1 == 0xA5CC4F95) {
        m_cicChip = CIC_NUS_6101;  // Also used by StarFox 64
    } else if (crc1 == 0xE24DD796 || crc1 == 0x900DDE47) {
        m_cicChip = CIC_NUS_6102;  // Common NTSC CIC
    } else if (crc1 == 0xB8B51D35 || crc1 == 0x49207DCE || crc1 == 0x9BBDF07D) {
        m_cicChip = CIC_NUS_6103;  // Common PAL CIC
    } else if (crc1 == 0x0B050EE0 || crc1 == 0xC8379994 || crc1 == 0x0E711C63) {
        m_cicChip = CIC_NUS_6105;  // Used by Zelda, F-Zero X, etc.
    } else if (crc1 == 0xD5BE5580 || crc1 == 0xB98A9A45) {
        m_cicChip = CIC_NUS_6106;  // Rare PAL CIC
    } else if (m_cartID == "NA" && getMediaType().contains("ALECK64")) {
        m_cicChip = CIC_NUS_5167;  // Aleck64 arcade board
    } else if (getMediaType().contains("64DD")) {
        m_cicChip = CIC_NUS_8303;  // 64DD
    } else {
        // Default to most common CIC chip if we can't detect
        if (m_country == Country_USA || m_country == Country_Japan) {
            m_cicChip = CIC_NUS_6102;  // NTSC default
        } else {
            m_cicChip = CIC_NUS_6103;  // PAL default
        }
    }
    
    qDebug() << "Detected CIC chip:" << m_cicChip;
}

} // namespace QT_UI
