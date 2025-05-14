#pragma once

#include <QString>
#include <QByteArray>
#include <QMap>
#include "RomParser.h" // Include parser to get enum types

namespace QT_UI {

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
    QString getStatus() const;  // New method to get compatibility status
    
    // Add a method to get the detected ROM format
    RomByteFormat getByteFormat() const;
    
private:
    void initializeCountryNames();
    void parseRomHeader();
    void calculateCRC();
    bool loadRomInformation();
    void loadRDBInfo();
    void detectCICChip();  // Add this declaration
    
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
    QString m_status;  // New field for compatibility status
    
    // Static country name mapping
    static QMap<CountryCode, QString> m_countryNames;

    // ROM byte format
    RomByteFormat m_byteFormat;
    
    // ROM parser
    RomParser* m_romParser;
};

} // namespace QT_UI
