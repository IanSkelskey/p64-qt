#pragma once

#include <QByteArray>
#include <QString>

namespace QT_UI {

// Move all enum definitions here
enum CountryCode {
    Country_Unknown = 0,
    Country_USA,
    Country_Germany,
    Country_Japan,
    Country_Europe,
    Country_Italy,
    Country_Spain,
    Country_Australia,
    Country_France,
    Country_Unknown2,
    Country_Unknown3
};

enum CICChip {
    CIC_UNKNOWN = -1,
    CIC_NUS_6101 = 1,
    CIC_NUS_6102,
    CIC_NUS_6103,
    CIC_NUS_6104,
    CIC_NUS_6105,
    CIC_NUS_6106,
    CIC_NUS_5167,
    CIC_NUS_8303,
    CIC_NUS_8401,
    CIC_NUS_5101
};

enum RomFileFormat {
    Format_Uncompressed,
    Format_Zip,
    Format_7zip
};

enum RomByteFormat {
    Format_Unknown,
    Format_Z64,    // Big Endian (native N64)
    Format_N64,    // Byte-swapped (middle-endian)
    Format_V64     // Little Endian
};

class RomParser {
public:
    RomParser();
    ~RomParser();
    
    // Set the ROM data for parsing
    bool setRomData(const QByteArray& headerData);
    
    // Format detection and conversion
    RomByteFormat detectByteFormat() const;
    QByteArray convertToZ64Format(RomByteFormat sourceFormat) const;
    uint32_t byteSwap32(uint32_t value, RomByteFormat sourceFormat) const;
    
    // Header information extraction
    QString extractInternalName() const;
    QString extractCartID() const;
    CountryCode extractCountryCode() const;
    QString extractMediaType() const;
    void calculateCRC(uint32_t& crc1, uint32_t& crc2) const;
    
    // Get raw country code byte for database lookups
    unsigned char getRawCountryByte() const;

private:
    QByteArray m_headerData;
    QByteArray m_headerZ64; // Header data converted to Z64 format for consistent parsing
    RomByteFormat m_detectedFormat;
};

} // namespace QT_UI
