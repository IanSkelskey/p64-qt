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
    m_status("Unknown"),
    m_romParser(new RomParser())
{
    if (m_countryNames.isEmpty()) {
        initializeCountryNames();
    }
    
    // Initialize DatabaseManager with path to the database
    QString dbPath = QDir(QCoreApplication::applicationDirPath()).filePath("database.sqlite");
    m_dbManager = new DatabaseManager(dbPath);
    
    // Open database connection
    if (!m_dbManager->open()) {
        qWarning() << "Failed to open database at:" << dbPath;
    }
}

RomInfoProvider::~RomInfoProvider()
{
    delete m_romParser;
    delete m_dbManager; // Clean up database manager
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
    // Only log format if it's unusual
    if (m_byteFormat == Format_Unknown) {
        qWarning() << "Unknown ROM format detected for:" << filePath;
    }
    
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
    
    // Check if database is usable
    if (!m_dbManager || !m_dbManager->isDatabaseLoaded()) {
        qWarning() << "Database is not loaded, ROM info cannot be retrieved";
        return;
    }
    
    // Use the optimized rom_browser_view to get ROM information in a single query
    auto gameInfo = m_dbManager->getRomBrowserEntryByCRC(m_crc1, m_crc2, countryHex);
    
    if (!gameInfo.empty()) {
        // Process ROM info silently - only log if needed for diagnostics
        if (gameInfo.count("good_name") && gameInfo["good_name"].isValid()) {
            m_goodName = gameInfo["good_name"].toString();
        }
        
        if (gameInfo.count("status") && gameInfo["status"].isValid()) {
            m_status = gameInfo["status"].toString();
        }
        
        if (gameInfo.count("internal_name") && gameInfo["internal_name"].isValid() && m_internalName.isEmpty()) {
            m_internalName = gameInfo["internal_name"].toString();
        }
        
        if (gameInfo.count("developer_name") && gameInfo["developer_name"].isValid()) {
            m_developer = gameInfo["developer_name"].toString();
        }
        
        if (gameInfo.count("release_date") && gameInfo["release_date"].isValid()) {
            m_releaseDate = gameInfo["release_date"].toString();
        }
        
        if (gameInfo.count("genre_name") && gameInfo["genre_name"].isValid()) {
            m_genre = gameInfo["genre_name"].toString();
        }
        
        if (gameInfo.count("players") && gameInfo["players"].isValid()) {
            m_players = gameInfo["players"].toInt();
        }
        
        if (gameInfo.count("force_feedback") && gameInfo["force_feedback"].isValid()) {
            m_forceFeedback = gameInfo["force_feedback"].toBool();
        }
        
        // Map cartridge_code to productID as mentioned in the requirements
        if (gameInfo.count("cartridge_code") && gameInfo["cartridge_code"].isValid()) {
            m_productID = gameInfo["cartridge_code"].toString();
        }
        
        // Single summary debug message instead of multiple per-property messages
        qDebug() << "ROM info loaded:" << m_goodName << "(" << m_status << ")";
    } else {
        // Only log when no match is found - this is potentially a problem
        qDebug() << "ROM not found in database for CRC:" 
                 << QString::number(m_crc1, 16).toUpper() << "-" 
                 << QString::number(m_crc2, 16).toUpper();
        
        // Try fallback lookups if not found by CRC
        auto gameInfoByCartridge = m_dbManager->getRomInfoByCartridgeCode(m_cartID);
        if (!gameInfoByCartridge.empty()) {
            // Successful fallback - no need to log details
            if (gameInfoByCartridge.count("good_name") && gameInfoByCartridge["good_name"].isValid()) {
                m_goodName = gameInfoByCartridge["good_name"].toString();
            }
            
            if (gameInfoByCartridge.count("cartridge_code") && gameInfoByCartridge["cartridge_code"].isValid()) {
                m_productID = gameInfoByCartridge["cartridge_code"].toString();
            }
        } else if (!m_internalName.isEmpty()) {
            // Try lookup by internal name if available
            auto gameInfoByName = m_dbManager->getRomInfoByInternalName(m_internalName);
            if (!gameInfoByName.empty()) {
                if (gameInfoByName.count("good_name") && gameInfoByName["good_name"].isValid()) {
                    m_goodName = gameInfoByName["good_name"].toString();
                }
            }
        }
        
        if (m_goodName.isEmpty()) {
            // Only log complete failure to find ROM info
            qWarning() << "No ROM info found in database for:" << m_internalName << "Cart ID:" << m_cartID;
        }
    }
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
    // Only log unusual media types
    if (mediaType != "N64 Cartridge" && !mediaType.isEmpty()) {
        qDebug() << "Non-standard media type detected:" << mediaType;
    }
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
    
    // Only log unusual CIC chips
    if (m_cicChip != CIC_NUS_6102 && m_cicChip != CIC_NUS_6103 && m_cicChip != CIC_UNKNOWN) {
        qDebug() << "Non-standard CIC chip detected:" << m_cicChip;
    }
}

} // namespace QT_UI
