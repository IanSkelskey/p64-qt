#pragma once

#include <QAbstractTableModel>
#include <QVector>
#include <QIcon>
#include <QString>
#include <QMap>
#include <QSize>
#include <QDateTime>
#include <QPixmap>
#include <QCache>
#include <QtWidgets/QStyledItemDelegate>
#include "../../Core/RomInfoProvider.h"

namespace QT_UI {

/**
 * @brief Structure to hold ROM information
 */
struct RomInfo {
    QString fileName;
    QString goodName;
    QString internalName;
    QString romSize;
    QString country;
    QString releaseDate;
    QString players;
    QString genre;
    QString developer;
    QString crc1;
    QString crc2;
    QString md5;
    QString filePath;
    QString cartID;
    QString mediaType;
    QString productID;  // Added ProductID field
    QIcon icon;
    bool isGoodDump;
    
    // Additional flags for sorting/filtering
    bool hasBeenPlayed;
    QDateTime lastPlayed;
    int playCount;
    
    // Cover art information
    QString coverPath;
    bool hasCover;
};

/**
 * @brief Model for the ROM browser list/table view
 */
class RomListModel : public QAbstractTableModel
{
    Q_OBJECT
    
public:
    enum RomColumns {
        FileName = 0,
        GoodName,
        InternalName,
        Size,
        Country,
        ReleaseDate,
        Players,
        Genre,
        Developer,
        CRC1,
        CRC2,
        MD5,
        FilePath,
        CartID,
        MediaType,
        ColumnCount
    };
    
    // Simplified view modes inspired by PCSX2
    enum ViewMode {
        DetailView,  // Table-like view with enhancements
        GridView     // Grid of cover images
    };

    explicit RomListModel(QObject* parent = nullptr);
    ~RomListModel();
    
    // Model interface implementation
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    
    // ROM management methods
    bool addRom(const QString& filePath);
    bool removeRom(const QString& filePath);
    void clear();
    void refresh();
    
    // Directory scanning
    void scanDirectory(const QString& path, bool recursive = false);
    
    // Access methods
    RomInfo getRomInfo(int index) const;
    RomInfo getRomInfo(const QString& filePath) const;
    QString getRomPath(int index) const;
    
    // Column visibility and order
    void setVisibleColumns(const QVector<RomColumns>& columns);
    QVector<RomColumns> visibleColumns() const;
    
    // View mode handling
    void setViewMode(ViewMode mode);
    ViewMode viewMode() const;
    
    // Grid view options
    void setShowTitles(bool show);
    bool showTitles() const;
    void setCoverScale(float scale);
    float coverScale() const;
    QSize coverSize() const;
    
    // Cover art management
    void setCoverDirectory(const QString& directory);
    QString coverDirectory() const;
    QPixmap getCoverImage(const QString& romPath, bool loadIfNeeded = true) const;
    void refreshCovers();
    
public slots:
    void setRomDirectory(const QString& directory);
    void refreshRomList();
    
signals:
    void scanStarted();
    void scanProgress(int current, int total);
    void scanFinished();
    void romAdded(const QString& filePath);
    void romRemoved(const QString& filePath);
    void coverLoaded(const QString& romPath);
    
private:
    // Helper methods
    QIcon getCountryIcon(const QString& countryCode) const;
    QString sizeToString(qint64 size) const;
    bool findAndLoadCoverArt(const QString& romPath, RomInfo& info);
    QPixmap createPlaceholderCover(const RomInfo& info) const;
    void loadSettings();
    void saveSettings();
    
    // ROM information loading
    bool loadRomInfo(const QString& filePath, RomInfo& info);
    
    // Data storage
    QVector<RomInfo> m_romList;
    QMap<QString, int> m_pathToIndex;
    QVector<RomColumns> m_visibleColumns;
    QString m_currentDirectory;
    
    // Icon cache
    QMap<QString, QIcon> m_countryIcons;
    QIcon m_defaultIcon;
    QPixmap m_defaultCoverImage;
    
    // Cover art and view options
    QString m_coverDirectory;
    mutable QCache<QString, QPixmap> m_coverCache;
    ViewMode m_currentViewMode;
    float m_coverScale;
    QSize m_baseCoverSize;
    bool m_showTitles;
};

// Grid view ItemDelegate for ROM display
class RomGridDelegate : public QStyledItemDelegate
{
    Q_OBJECT
    
public:
    RomGridDelegate(RomListModel* model = nullptr, QObject* parent = nullptr);
    
    void paint(QPainter* painter, const QStyleOptionViewItem& option, 
               const QModelIndex& index) const override;
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    
private:
    RomListModel* m_model;
};

} // namespace QT_UI
