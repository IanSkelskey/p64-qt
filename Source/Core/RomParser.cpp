#include "RomParser.h"
#include <QDebug>

namespace QT_UI {

RomParser::RomParser() : m_detectedFormat(Format_Unknown)
{
}

RomParser::~RomParser()
{
}

bool RomParser::setRomData(const QByteArray& headerData)
{
    if (headerData.size() < 0x40) {
        qWarning() << "ROM header data too small (needs at least 64 bytes)";
        return false;
    }
    
    m_headerData = headerData;
    m_detectedFormat = detectByteFormat();
    m_headerZ64 = convertToZ64Format(m_detectedFormat);
    
    return true;
}

RomByteFormat RomParser::detectByteFormat() const
{
    if (m_headerData.size() < 4) {
        return Format_Unknown;
    }
    
    // Extract the first 4 bytes as unsigned values
    uint32_t magic = ((static_cast<uint32_t>(static_cast<unsigned char>(m_headerData[0])) << 24) |
                       (static_cast<uint32_t>(static_cast<unsigned char>(m_headerData[1])) << 16) |
                       (static_cast<uint32_t>(static_cast<unsigned char>(m_headerData[2])) << 8) |
                       (static_cast<uint32_t>(static_cast<unsigned char>(m_headerData[3]))));
    
    // Check against known magic values
    if (magic == 0x80371240) {
        return Format_Z64; // Big-endian (native N64)
    } else if (magic == 0x37804012) {
        return Format_N64; // Byte-swapped (middle-endian)
    } else if (magic == 0x40123780) {
        return Format_V64; // Little-endian
    }
    
    // If no match, try a more lenient check based on the first byte
    unsigned char firstByte = static_cast<unsigned char>(m_headerData[0]);
    if (firstByte == 0x80) {
        return Format_Z64;
    } else if (firstByte == 0x37) {
        return Format_N64;
    } else if (firstByte == 0x40) {
        return Format_V64;
    }
    
    // Default to Z64 if we can't determine - only log warning in this case
    if (magic != 0x80371240 && magic != 0x37804012 && magic != 0x40123780) {
        qWarning() << "Could not determine ROM format, defaulting to Z64. Magic:" << QString::number(magic, 16);
    }
    
    return Format_Z64;
}

QByteArray RomParser::convertToZ64Format(RomByteFormat sourceFormat) const
{
    if (sourceFormat == Format_Z64 || sourceFormat == Format_Unknown) {
        return m_headerData; // Already in Z64 format or unknown format
    }
    
    QByteArray converted(m_headerData.size(), 0);
    
    if (sourceFormat == Format_N64) {
        // N64 format: bytes are swapped within 16-bit words (middle-endian)
        for (int i = 0; i < m_headerData.size(); i += 2) {
            if (i + 1 < m_headerData.size()) {
                converted[i] = m_headerData[i + 1];
                converted[i + 1] = m_headerData[i];
            } else if (i < m_headerData.size()) {
                converted[i] = m_headerData[i]; // Handle odd byte at the end
            }
        }
    } else if (sourceFormat == Format_V64) {
        // V64 format: completely reversed byte order (little-endian)
        for (int i = 0; i < m_headerData.size(); i += 4) {
            if (i + 3 < m_headerData.size()) {
                converted[i] = m_headerData[i + 3];
                converted[i + 1] = m_headerData[i + 2];
                converted[i + 2] = m_headerData[i + 1];
                converted[i + 3] = m_headerData[i];
            } else {
                // Handle partial word at the end
                for (int j = 0; j < m_headerData.size() - i; j++) {
                    converted[i + j] = m_headerData[m_headerData.size() - 1 - j];
                }
            }
        }
    }
    
    return converted;
}

uint32_t RomParser::byteSwap32(uint32_t value, RomByteFormat sourceFormat) const
{
    if (sourceFormat == Format_Z64 || sourceFormat == Format_Unknown) {
        return value; // No conversion needed
    } else if (sourceFormat == Format_N64) {
        // N64 format: swap bytes within 16-bit words
        return ((value & 0xFF00FF00) >> 8) | ((value & 0x00FF00FF) << 8);
    } else if (sourceFormat == Format_V64) {
        // V64 format: completely reverse bytes
        return ((value >> 24) & 0xFF) | 
               ((value >> 8) & 0xFF00) | 
               ((value << 8) & 0xFF0000) | 
               ((value << 24) & 0xFF000000);
    }
    return value;
}

QString RomParser::extractInternalName() const
{
    // Extract internal name from header (offset 0x20, length 20 bytes)
    return QString::fromUtf8(m_headerZ64.mid(0x20, 20).constData()).trimmed();
}

QString RomParser::extractCartID() const
{
    // Extract Cartridge ID (offset 0x3C, length 2 bytes)
    return QString::fromUtf8(m_headerZ64.mid(0x3C, 2).constData()).trimmed();
}

CountryCode RomParser::extractCountryCode() const
{
    // Extract country code (offset 0x3E, 1 byte)
    if (m_headerZ64.size() <= 0x3E) {
        return Country_Unknown;
    }
    
    char countryByte = m_headerZ64.at(0x3E);
    
    switch (countryByte) {
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

QString RomParser::extractMediaType() const
{
    if (m_headerZ64.size() < 0x39) {
        return "Unknown";
    }
    
    // Remove excessive debug output
    unsigned char mediaType = (unsigned char)m_headerZ64.at(0x38);
    
    // If the media type byte is 0, try to determine it from other header info
    // Most ROMs should be cartridges (Type 'N')
    if (mediaType == 0) {
        // Most common case is standard N64 cartridge
        return "N64 Cartridge";
    }
    
    // Handle valid media types
    switch (mediaType) {
        case 'N': return "N64 Cartridge";
        case 'D': return "64DD Disk";
        case 'C': return "N64 Cartridge (Disk Compatible)";
        case 'E': return "64DD Expansion";
        case 'Z': return "Aleck64 Cartridge";
        case 0:   return "N64 Cartridge"; // Default to cartridge instead of "None"
        default:  {
            // Handle potentially invalid values that are still printable ASCII
            if (mediaType >= 32 && mediaType <= 126) {
                return QString("Unknown (%1)").arg(QChar(mediaType));
            } else {
                return "N64 Cartridge"; // Default for non-printable values
            }
        }
    }
}

void RomParser::calculateCRC(uint32_t& crc1, uint32_t& crc2) const
{
    if (m_headerZ64.size() >= 0x18) {
        // Extract CRCs from the Z64-formatted header
        crc1 = ((static_cast<uint32_t>(static_cast<unsigned char>(m_headerZ64[0x10])) << 24) |
                (static_cast<uint32_t>(static_cast<unsigned char>(m_headerZ64[0x11])) << 16) |
                (static_cast<uint32_t>(static_cast<unsigned char>(m_headerZ64[0x12])) << 8) |
                (static_cast<uint32_t>(static_cast<unsigned char>(m_headerZ64[0x13]))));
        
        crc2 = ((static_cast<uint32_t>(static_cast<unsigned char>(m_headerZ64[0x14])) << 24) |
                (static_cast<uint32_t>(static_cast<unsigned char>(m_headerZ64[0x15])) << 16) |
                (static_cast<uint32_t>(static_cast<unsigned char>(m_headerZ64[0x16])) << 8) |
                (static_cast<uint32_t>(static_cast<unsigned char>(m_headerZ64[0x17]))));
        
        // Only log problematic CRCs
        if (crc1 == 0 || crc2 == 0) {
            qWarning() << "ROM has unusual CRCs:" << QString::number(crc1, 16).toUpper()
                    << QString::number(crc2, 16).toUpper();
        }
    }
}

unsigned char RomParser::getRawCountryByte() const
{
    if (m_headerZ64.size() <= 0x3E) {
        return 0;
    }
    return static_cast<unsigned char>(m_headerZ64.at(0x3E));
}

} // namespace QT_UI
