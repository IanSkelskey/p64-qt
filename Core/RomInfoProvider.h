#pragma once

#include <QString>
#include <QByteArray>
#include <QMap>

namespace QT_UI {

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

class RomInfoProvider {
public:
    RomInfoProvider();
    ~RomInfoProvider();
    
    bool openRomFile(const QString& filePath);
    
    // Basic ROM information
    QString getInternalName() const;
    QString getFileName() const;
    QString getGoodName() const;
    QString getCartID() const;
    QString getMediaType() const;
    int getRomSize() const;
    
    // Country and region information
    QString getCountryName() const;
    CountryCode getCountryCode() const;
    
    // Hardware information
    CICChip getCICChip() const;
    uint32_t getCRC1() const;
    uint32_t getCRC2() const;
    QString getManufacturerID() const;
    RomFileFormat getFileFormat() const;
    
    // Game information from RDB/RDX
    QString getDeveloper() const;
    QString getReleaseDate() const;
    QString getGenre() const;
    int getPlayers() const;
    bool getForceFeedback() const;
    QString getProductID() const;
    
private:
    void initializeCountryNames();
    void parseRomHeader();
    void calculateCRC();
    bool loadRomInformation();
    void loadRDBInfo();
    
    // Static helper methods
    static QString countryCodeToName(CountryCode countryCode);
    static CountryCode charToCountryCode(char countryChar);
    
    // ROM information
    QString m_filePath;
    QString m_fileName;
    QString m_internalName;
    QString m_goodName;
    QString m_cartID;
    int m_romSize;
    CountryCode m_country;
    QByteArray m_RomHeader;
    CICChip m_cicChip;
    uint32_t m_crc1;
    uint32_t m_crc2;
    RomFileFormat m_fileFormat;
    
    // Game information
    QString m_developer;
    QString m_releaseDate;
    QString m_genre;
    int m_players;
    bool m_forceFeedback;
    QString m_productID;
    
    // Static country name mapping
    static QMap<CountryCode, QString> m_countryNames;
};

} // namespace QT_UI
