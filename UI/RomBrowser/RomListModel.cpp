#include "RomListModel.h"
#include <QDir>
#include <QFileInfo>
#include <QDateTime>
#include <QSettings>
#include <QIcon>
#include <QApplication>
#include <QMimeDatabase>
#include <QDirIterator>
#include <QThread>
#include <QFuture>
#include <QtConcurrent/QtConcurrentRun>
#include <QPainter>
#include <QFont>
#include <QStyledItemDelegate>
#include <QRegularExpression>
#include "../../Core/RomInfoProvider.h"

namespace QT_UI {

// Constants for cover art
const int DEFAULT_COVER_WIDTH = 160;
const int DEFAULT_COVER_HEIGHT = 224;
const float DEFAULT_COVER_SCALE = 1.0f;

RomListModel::RomListModel(QObject* parent) 
    : QAbstractTableModel(parent)
    , m_coverCache(100) // Cache up to 100 cover images
    , m_coverScale(DEFAULT_COVER_SCALE)
    , m_baseCoverSize(DEFAULT_COVER_WIDTH, DEFAULT_COVER_HEIGHT)
    , m_currentViewMode(DetailView) // Default to detail view
    , m_showTitles(true) // Show titles by default
{
    // Initialize visible columns with default values
    m_visibleColumns = {
        FileName,
        GoodName,
        InternalName,
        Size,
        Country,
        ReleaseDate,
        Developer,
        Genre,
        Players
    };
    
    // Load country icons from resources
    m_countryIcons["USA"] = QIcon(":/flags/Resources/flags/usa.svg");
    m_countryIcons["JP"] = QIcon(":/flags/Resources/flags/japan.svg");
    m_countryIcons["EU"] = QIcon(":/flags/Resources/flags/europe.svg");
    m_countryIcons["AU"] = QIcon(":/flags/Resources/flags/australia.svg");
    m_countryIcons["FR"] = QIcon(":/flags/Resources/flags/france.svg");
    m_countryIcons["GE"] = QIcon(":/flags/Resources/flags/germany.svg");
    m_countryIcons["IT"] = QIcon(":/flags/Resources/flags/italy.svg");
    m_countryIcons["SP"] = QIcon(":/flags/Resources/flags/spain.svg");
    m_countryIcons["OTHER"] = QIcon(":/flags/Resources/flags/unknown.svg");
    
    m_defaultIcon = QIcon(":/icons/Resources/icons/rom_default.png");
    
    // Load default cover image - fix the resource path
    m_defaultCoverImage = QPixmap(":/assets/images/Resources/images/default-label.png");
    if (m_defaultCoverImage.isNull()) {
        qDebug() << "Failed to load default cover image!";
        // Fallback if resource isn't found
        m_defaultCoverImage = QPixmap(DEFAULT_COVER_WIDTH, DEFAULT_COVER_HEIGHT);
        m_defaultCoverImage.fill(Qt::darkGray);
    } else {
        qDebug() << "Successfully loaded default cover image";
    }
    
    // Initialize cover directory
    m_coverDirectory = QApplication::applicationDirPath() + "/covers";
    
    // Load settings
    loadSettings();
}

RomListModel::~RomListModel()
{
    saveSettings();
}

int RomListModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;
    
    return m_romList.size();
}

int RomListModel::columnCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;
    
    return m_visibleColumns.size();
}

QVariant RomListModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() >= m_romList.size())
        return QVariant();
    
    const RomInfo& romInfo = m_romList.at(index.row());
    int column = m_visibleColumns.at(index.column());
    
    switch (role) {
    case Qt::DisplayRole:
        switch (column) {
        case FileName: return romInfo.fileName.isEmpty() ? tr("Unknown") : romInfo.fileName;
        case GoodName: return romInfo.goodName.isEmpty() ? tr("Unknown") : romInfo.goodName;
        case InternalName: return romInfo.internalName.isEmpty() ? tr("Unknown") : romInfo.internalName;
        case Size: return romInfo.romSize.isEmpty() ? tr("Unknown") : romInfo.romSize;
        case Country: return romInfo.country.isEmpty() ? tr("Unknown") : romInfo.country;
        case ReleaseDate: return romInfo.releaseDate.isEmpty() ? tr("Unknown") : romInfo.releaseDate;
        case Players: return romInfo.players.isEmpty() ? tr("Unknown") : romInfo.players;
        case Genre: return romInfo.genre.isEmpty() ? tr("Unknown") : romInfo.genre;
        case Developer: return romInfo.developer.isEmpty() ? tr("Unknown") : romInfo.developer;
        case CRC1: return romInfo.crc1.isEmpty() ? tr("Unknown") : romInfo.crc1;
        case CRC2: return romInfo.crc2.isEmpty() ? tr("Unknown") : romInfo.crc2;
        case MD5: return romInfo.md5.isEmpty() ? tr("Unknown") : romInfo.md5;
        case FilePath: return romInfo.filePath.isEmpty() ? tr("Unknown") : romInfo.filePath;
        case CartID: return romInfo.cartID.isEmpty() ? tr("Unknown") : romInfo.cartID;
        case MediaType: return romInfo.mediaType.isEmpty() ? tr("Unknown") : romInfo.mediaType;
        default: return QVariant();
        }
    case Qt::DecorationRole:
        if (column == FileName) {
            // Return cover art image for grid view
            if (m_currentViewMode == GridView && romInfo.hasCover) {
                return getCoverImage(romInfo.filePath);
            }
            // Return default icon if the ROM doesn't have an icon
            return romInfo.icon.isNull() ? m_defaultIcon : romInfo.icon;
        }
        else if (column == Country) {
            // Get country code for icon
            QString countryCode;
            if (romInfo.country.startsWith("USA") || romInfo.country.startsWith("America")) {
                countryCode = "USA";
            } else if (romInfo.country.startsWith("Japan")) {
                countryCode = "JP";
            } else if (romInfo.country.startsWith("Europe")) {
                countryCode = "EU";
            } else if (romInfo.country.startsWith("Australia")) {
                countryCode = "AU";
            } else if (romInfo.country.startsWith("France")) {
                countryCode = "FR";
            } else if (romInfo.country.startsWith("Germany")) {
                countryCode = "GE";
            } else if (romInfo.country.startsWith("Italy")) {
                countryCode = "IT";
            } else if (romInfo.country.startsWith("Spain")) {
                countryCode = "SP";
            } else {
                countryCode = "OTHER";
            }
            return m_countryIcons.value(countryCode, m_countryIcons.value("OTHER"));
        }
        return QVariant();
    // Custom role for grid view to access cover art
    case Qt::UserRole + 1:
        return getCoverImage(romInfo.filePath);
    case Qt::UserRole + 2:
        return romInfo.goodName.isEmpty() ? romInfo.fileName : romInfo.goodName;
    case Qt::ToolTipRole:
        return QString("%1\n%2\n%3\nPath: %4")
            .arg(romInfo.fileName)
            .arg(romInfo.goodName)
            .arg(romInfo.romSize)
            .arg(romInfo.filePath);
    case Qt::UserRole:
        return romInfo.filePath;
    default:
        return QVariant();
    }
}

QVariant RomListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return QVariant();
    
    if (section >= m_visibleColumns.size())
        return QVariant();
    
    switch (m_visibleColumns.at(section)) {
    case FileName: return tr("File Name");
    case GoodName: return tr("Good Name");
    case InternalName: return tr("Internal Name");
    case Size: return tr("Size");
    case Country: return tr("Country");
    case ReleaseDate: return tr("Release Date");
    case Players: return tr("Players");
    case Genre: return tr("Genre");
    case Developer: return tr("Developer");
    case CRC1: return tr("CRC1");
    case CRC2: return tr("CRC2");
    case MD5: return tr("MD5");
    case FilePath: return tr("File Path");
    case CartID: return tr("Cartridge ID");
    case MediaType: return tr("Media");
    default: return QVariant();
    }
}

Qt::ItemFlags RomListModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;
    
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

bool RomListModel::addRom(const QString& filePath)
{
    QFileInfo fileInfo(filePath);
    if (!fileInfo.exists() || !fileInfo.isReadable())
        return false;
    
    // Check if rom already exists in the list
    if (m_pathToIndex.contains(filePath))
        return false;
    
    RomInfo info;
    if (loadRomInfo(filePath, info)) {
        beginInsertRows(QModelIndex(), m_romList.size(), m_romList.size());
        m_pathToIndex[filePath] = m_romList.size();
        m_romList.append(info);
        endInsertRows();
        
        emit romAdded(filePath);
        return true;
    }
    
    return false;
}

bool RomListModel::removeRom(const QString& filePath)
{
    if (!m_pathToIndex.contains(filePath))
        return false;
    
    int index = m_pathToIndex[filePath];
    beginRemoveRows(QModelIndex(), index, index);
    m_romList.removeAt(index);
    m_pathToIndex.remove(filePath);
    
    // Update indices for remaining items
    for (auto it = m_pathToIndex.begin(); it != m_pathToIndex.end(); ++it) {
        if (it.value() > index)
            it.value()--;
    }
    
    endRemoveRows();
    emit romRemoved(filePath);
    return true;
}

void RomListModel::clear()
{
    if (m_romList.isEmpty())
        return;
    
    beginResetModel();
    m_romList.clear();
    m_pathToIndex.clear();
    endResetModel();
}

void RomListModel::refresh()
{
    // Rescan the current directory
    if (!m_currentDirectory.isEmpty())
        scanDirectory(m_currentDirectory);
}

void RomListModel::scanDirectory(const QString& path, bool recursive)
{
    if (path.isEmpty())
        return;
    
    emit scanStarted();
    
    // Clear existing data
    clear();
    
    QDir dir(path);
    if (!dir.exists()) {
        emit scanFinished();
        return;
    }
    
    // Get list of ROM file extensions
    QStringList filters;
    filters << "*.z64" << "*.v64" << "*.n64" << "*.zip";
    
    // Count total files for progress reporting
    int totalFiles = 0;
    if (recursive) {
        QDirIterator it(path, filters, QDir::Files, QDirIterator::Subdirectories);
        while (it.hasNext()) {
            it.next();
            totalFiles++;
        }
    } else {
        dir.setNameFilters(filters);
        totalFiles = dir.entryList(QDir::Files).count();
    }
    
    // Scan directory
    QDirIterator it(path, filters, QDir::Files, 
                    recursive ? QDirIterator::Subdirectories : QDirIterator::NoIteratorFlags);
    
    int processedFiles = 0;
    while (it.hasNext()) {
        QString filePath = it.next();
        addRom(filePath);
        
        processedFiles++;
        emit scanProgress(processedFiles, totalFiles);
        
        // Process events to keep UI responsive
        QApplication::processEvents();
    }
    
    emit scanFinished();
}

RomInfo RomListModel::getRomInfo(int index) const
{
    if (index >= 0 && index < m_romList.size())
        return m_romList.at(index);
    
    return RomInfo();
}

RomInfo RomListModel::getRomInfo(const QString& filePath) const
{
    auto it = m_pathToIndex.find(filePath);
    if (it != m_pathToIndex.end())
        return m_romList.at(it.value());
    
    return RomInfo();
}

QString RomListModel::getRomPath(int index) const
{
    if (index >= 0 && index < m_romList.size())
        return m_romList.at(index).filePath;
    
    return QString();
}

void RomListModel::setVisibleColumns(const QVector<RomColumns>& columns)
{
    if (columns.isEmpty())
        return;
    
    beginResetModel();
    m_visibleColumns = columns;
    endResetModel();
}

QVector<RomListModel::RomColumns> RomListModel::visibleColumns() const
{
    return m_visibleColumns;
}

void RomListModel::setRomDirectory(const QString& directory)
{
    if (m_currentDirectory != directory) {
        m_currentDirectory = directory;
        scanDirectory(directory);
    }
}

void RomListModel::refreshRomList()
{
    refresh();
}

bool RomListModel::loadRomInfo(const QString& filePath, RomInfo& info)
{
    QFileInfo fileInfo(filePath);
    if (!fileInfo.exists() || !fileInfo.isReadable())
        return false;
    
    // Create ROM info provider
    RomInfoProvider provider;
    
    // Try to open and parse the ROM file
    if (!provider.openRomFile(filePath)) {
        // Basic file information if we couldn't parse the ROM
        info.fileName = fileInfo.fileName();
        info.filePath = filePath;
        info.romSize = sizeToString(fileInfo.size());
        info.icon = m_defaultIcon;
        info.goodName = info.fileName.section('.', 0, -2); // Remove extension
        info.isGoodDump = false;
        info.hasBeenPlayed = false;
        info.hasCover = false;
        return true;
    }
    
    // Fill in the ROM info from our provider
    info.fileName = fileInfo.fileName();
    info.filePath = filePath;
    info.romSize = sizeToString(fileInfo.size());
    info.icon = m_defaultIcon;
    
    // Enhanced ROM information
    info.internalName = provider.getInternalName();
    info.goodName = provider.getGoodName();
    if (info.goodName.isEmpty()) {
        info.goodName = info.fileName.section('.', 0, -2); // Use filename without extension as fallback
    }
    
    info.country = provider.getCountryName();
    info.crc1 = QString("0x%1").arg(provider.getCRC1(), 8, 16, QChar('0')).toUpper();
    info.crc2 = QString("0x%1").arg(provider.getCRC2(), 8, 16, QChar('0')).toUpper();
    info.cartID = provider.getCartID();
    info.mediaType = provider.getMediaType();
    
    // Additional information from ROM database
    info.developer = provider.getDeveloper();
    info.releaseDate = provider.getReleaseDate();
    info.genre = provider.getGenre();
    info.players = QString::number(provider.getPlayers());
    info.productID = provider.getProductID();  // Get ProductID
    
    // Format the release date nicely if possible (assuming YYYY-MM-DD format)
    if (!info.releaseDate.isEmpty()) {
        QStringList parts = info.releaseDate.split("-");
        if (parts.size() == 3) {
            // Format as Month Day, Year if it appears to be a valid date
            bool yearOk = false, monthOk = false, dayOk = false;
            int year = parts[0].toInt(&yearOk);
            int month = parts[1].toInt(&monthOk);
            int day = parts[2].toInt(&dayOk);
            
            if (yearOk && monthOk && dayOk && month > 0 && month <= 12 && day > 0 && day <= 31) {
                QDate date(year, month, day);
                if (date.isValid()) {
                    info.releaseDate = date.toString("MMMM d, yyyy");
                }
            }
        }
    }
    
    // Status information
    info.isGoodDump = true;
    info.hasBeenPlayed = false;
    info.lastPlayed = QDateTime();
    info.playCount = 0;
    
    // Check for cover art
    info.hasCover = findAndLoadCoverArt(filePath, info);
    
    return true;
}

void RomListModel::setCoverDirectory(const QString& directory)
{
    if (m_coverDirectory != directory) {
        m_coverDirectory = directory;
        
        // Save the new cover directory in settings
        QSettings settings("Project64", "QtUI");
        settings.setValue("RomBrowser/CoverDirectory", m_coverDirectory);
        
        // Refresh covers
        refreshCovers();
    }
}

QString RomListModel::coverDirectory() const
{
    return m_coverDirectory;
}

QPixmap RomListModel::getCoverImage(const QString& romPath, bool loadIfNeeded) const
{
    // Try to get from cache first
    QPixmap* cachedPixmap = m_coverCache.object(romPath);
    if (cachedPixmap && !cachedPixmap->isNull()) {
        return *cachedPixmap;
    }
    
    // If not in cache and we should load it
    if (loadIfNeeded) {
        int index = m_pathToIndex.value(romPath, -1);
        if (index >= 0) {
            const RomInfo& info = m_romList.at(index);
            
            QPixmap coverPixmap;
            if (info.hasCover && !info.coverPath.isEmpty()) {
                // Load the cover image
                coverPixmap = QPixmap(info.coverPath);
                if (coverPixmap.isNull()) {
                    // If loading fails, use default
                    coverPixmap = m_defaultCoverImage;
                }
            } else {
                // Use default cover image when no custom cover exists
                coverPixmap = m_defaultCoverImage;
            }
            
            // This pixmap will be used directly - no scaling here
            // Let the delegate handle the proper scaled drawing with aspect ratio maintained
            
            // Store original image in cache
            m_coverCache.insert(romPath, new QPixmap(coverPixmap));
            
            return coverPixmap;
        }
    }
    
    // Return the default cover pixmap
    return m_defaultCoverImage;
}

void RomListModel::refreshCovers()
{
    // Clear the cover cache
    m_coverCache.clear();
    
    // Re-scan covers for all ROMs
    for (int i = 0; i < m_romList.size(); ++i) {
        RomInfo& info = m_romList[i];
        info.hasCover = findAndLoadCoverArt(info.filePath, info);
    }
    
    // Notify views of data change
    emit dataChanged(index(0, 0), index(m_romList.size() - 1, columnCount() - 1));
}

void RomListModel::setViewMode(ViewMode mode)
{
    if (m_currentViewMode != mode) {
        m_currentViewMode = mode;
        // Notify views that data presentation may have changed
        emit dataChanged(index(0, 0), index(rowCount() - 1, columnCount() - 1));
        
        // Save the setting
        QSettings settings("Project64", "QtUI");
        settings.setValue("RomBrowser/ViewMode", static_cast<int>(m_currentViewMode));
    }
}

RomListModel::ViewMode RomListModel::viewMode() const
{
    return m_currentViewMode;
}

void RomListModel::setShowTitles(bool show)
{
    if (m_showTitles != show) {
        m_showTitles = show;
        
        // Only trigger update if in grid view mode
        if (m_currentViewMode == GridView) {
            emit dataChanged(index(0, 0), index(rowCount() - 1, columnCount() - 1));
        }
        
        // Save setting
        QSettings settings("Project64", "QtUI");
        settings.setValue("RomBrowser/ShowTitles", m_showTitles);
    }
}

bool RomListModel::showTitles() const
{
    return m_showTitles;
}

void RomListModel::setCoverScale(float scale)
{
    if (m_coverScale != scale) {
        m_coverScale = qBound(0.5f, scale, 2.0f);  // Limit zoom between 50% and 200%
        
        // Notify views of change if in grid view
        if (m_currentViewMode == GridView) {
            emit dataChanged(index(0, 0), index(rowCount() - 1, columnCount() - 1));
        }
        
        // Save setting
        QSettings settings("Project64", "QtUI");
        settings.setValue("RomBrowser/CoverScale", m_coverScale);
    }
}

float RomListModel::coverScale() const
{
    return m_coverScale;
}

QSize RomListModel::coverSize() const
{
    return QSize(m_baseCoverSize.width() * m_coverScale, 
                 m_baseCoverSize.height() * m_coverScale);
}

bool RomListModel::findAndLoadCoverArt(const QString& romPath, RomInfo& info)
{
    if (m_coverDirectory.isEmpty() || !QDir(m_coverDirectory).exists()) {
        return false;
    }
    
    QFileInfo fileInfo(romPath);
    QString baseName = fileInfo.completeBaseName();
    QString goodName = info.goodName;
    QString internalName = info.internalName;
    
    // Generate possible filenames for the cover
    QStringList possibleNames;
    
    // 1. Try ProductID if available (highest priority)
    if (!info.productID.isEmpty()) {
        // Remove any spaces or hyphens from the ProductID
        QString cleanProductID = info.productID;
        cleanProductID.remove(' ').remove('-');
        possibleNames << cleanProductID;
        possibleNames << info.productID; // Also try with original format
    }
    
    // 2. Try Internal Name
    if (!internalName.isEmpty()) {
        // Clean up internal name (remove spaces and special characters)
        QString cleanInternalName = internalName;
        cleanInternalName = cleanInternalName.replace(QRegularExpression("[^a-zA-Z0-9]"), "");
        possibleNames << cleanInternalName;
    }
    
    // 3. Try by CRC values if available
    if (!info.crc1.isEmpty() && !info.crc2.isEmpty()) {
        possibleNames << QString("%1-%2").arg(info.crc1).arg(info.crc2);
        possibleNames << QString("%1%2").arg(info.crc1.mid(2)).arg(info.crc2.mid(2)); // Without 0x prefix
    }
    
    // 4. Try by file name
    possibleNames << baseName;
    
    // 5. Try by good name
    if (!goodName.isEmpty()) {
        // Try with different variants of the good name
        QString cleanGoodName = goodName;
        cleanGoodName = cleanGoodName.replace(QRegularExpression("[^a-zA-Z0-9]"), ""); // Remove special chars
        possibleNames << cleanGoodName;
        possibleNames << goodName;
    }
    
    // Try to find the cover with various extensions
    QStringList extensions = {"png", "jpg", "jpeg"};
    
    qDebug() << "Looking for cover art for ROM:" << info.fileName;
    qDebug() << "ProductID:" << info.productID << "Internal Name:" << internalName;
    qDebug() << "Possible names to try:" << possibleNames;
    
    for (const QString& name : possibleNames) {
        for (const QString& ext : extensions) {
            QString coverPath = QString("%1/%2.%3").arg(m_coverDirectory, name, ext);
            QFileInfo coverInfo(coverPath);
            
            if (coverInfo.exists() && coverInfo.isReadable()) {
                qDebug() << "Found cover art at:" << coverPath;
                info.coverPath = coverPath;
                return true;
            }
        }
    }
    
    qDebug() << "No cover found for ROM:" << info.fileName;
    
    // No cover found
    info.coverPath.clear();
    return false;
}

QPixmap RomListModel::createPlaceholderCover(const RomInfo& info) const
{
    // Use the default image resource
    QPixmap placeholder = m_defaultCoverImage;
    
    // If the default cover image is not available, create a basic one
    if (placeholder.isNull()) {
        // Create a placeholder cover with the ROM name
        placeholder = QPixmap(m_baseCoverSize);
        placeholder.fill(Qt::darkGray);
        
        QPainter painter(&placeholder);
        painter.setPen(Qt::white);
        
        // Draw a border
        painter.drawRect(0, 0, placeholder.width() - 1, placeholder.height() - 1);
        
        // Get the ROM name to display
        QString displayName = info.goodName;
        if (displayName.isEmpty()) {
            displayName = info.fileName;
        }
        
        // Draw the name
        QFont font = painter.font();
        font.setBold(true);
        font.setPixelSize(14);
        painter.setFont(font);
        
        QRect textRect = placeholder.rect().adjusted(10, 10, -10, -10);
        painter.drawText(textRect, Qt::AlignCenter | Qt::TextWordWrap, displayName);
    }
    
    return placeholder;
}

void RomListModel::loadSettings()
{
    QSettings settings("Project64", "QtUI");
    
    // Load view mode setting
    int viewMode = settings.value("RomBrowser/ViewMode", static_cast<int>(DetailView)).toInt();
    m_currentViewMode = static_cast<ViewMode>(viewMode);
    
    // Load grid view settings
    m_showTitles = settings.value("RomBrowser/ShowTitles", true).toBool();
    m_coverScale = settings.value("RomBrowser/CoverScale", DEFAULT_COVER_SCALE).toFloat();
    
    // Load cover directory
    m_coverDirectory = settings.value("RomBrowser/CoverDirectory", 
                                    QApplication::applicationDirPath() + "/covers").toString();
}

void RomListModel::saveSettings()
{
    QSettings settings("Project64", "QtUI");
    
    // Save view mode
    settings.setValue("RomBrowser/ViewMode", static_cast<int>(m_currentViewMode));
    
    // Save grid view settings
    settings.setValue("RomBrowser/ShowTitles", m_showTitles);
    settings.setValue("RomBrowser/CoverScale", m_coverScale);
    
    // Save cover directory
    settings.setValue("RomBrowser/CoverDirectory", m_coverDirectory);
}

QIcon QT_UI::RomListModel::getCountryIcon(const QString &countryCode) const
{
    // Try to get the country icon from our map
    auto it = m_countryIcons.find(countryCode);
    if (it != m_countryIcons.end()) {
        return it.value();
    }
    
    // Return the default icon if no match
    return m_countryIcons.value("OTHER", QIcon());
}

QString QT_UI::RomListModel::sizeToString(long long size) const
{
    const double KB = 1024.0;
    const double MB = 1024.0 * KB;
    const double GB = 1024.0 * MB;
    
    if (size < 0) {
        return "Unknown";
    } else if (size < KB) {
        return QString("%1 bytes").arg(size);
    } else if (size < MB) {
        return QString("%1 KB").arg(size / KB, 0, 'f', 2);
    } else if (size < GB) {
        return QString("%1 MB").arg(size / MB, 0, 'f', 2);
    } else {
        return QString("%1 GB").arg(size / GB, 0, 'f', 2);
    }
}

// Grid view delegate implementation
RomGridDelegate::RomGridDelegate(RomListModel* model, QObject* parent)
    : QStyledItemDelegate(parent)
    , m_model(model)
{
}

void RomGridDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, 
                            const QModelIndex& index) const
{
    if (!m_model)
        return;

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setRenderHint(QPainter::SmoothPixmapTransform);
    
    // Get cover image and title
    QPixmap cover = qvariant_cast<QPixmap>(index.data(Qt::UserRole + 1));
    QString title = index.data(Qt::UserRole + 2).toString();
    
    // Ensure the title isn't empty
    if (title.isEmpty()) {
        title = tr("Unknown");
    }
    
    // Draw selection background if selected
    if (option.state & QStyle::State_Selected) {
        QColor highlight = option.palette.highlight().color();
        highlight.setAlpha(180);
        painter->fillRect(option.rect, highlight);
    } else if (option.state & QStyle::State_MouseOver) {
        // Highlight on hover with a softer color
        QColor hoverColor = option.palette.highlight().color().lighter(150);
        hoverColor.setAlpha(120);
        painter->fillRect(option.rect, hoverColor);
    }
    
    // Fixed calculations for cover placement
    QSize coverSize = m_model->coverSize();
    
    // Calculate the exact vertical space we need
    int titleSpace = m_model->showTitles() ? 30 : 10;  // Reduced space for title (was 50)
    int coverHeight = qMin(coverSize.height(), option.rect.height() - titleSpace);
    int coverWidth = qMin(coverSize.width(), option.rect.width() - 10);
    
    // Calculate actual display size maintaining aspect ratio
    QSize actualSize;
    if (cover.isNull()) {
        // Use fixed size for null pixmaps
        actualSize = QSize(coverWidth, coverHeight);
    } else {
        // Use scale for non-null pixmaps to maintain aspect ratio
        actualSize = cover.size().scaled(coverWidth, coverHeight, Qt::KeepAspectRatio);
    }
    
    // Center the cover in the available space
    QRect coverRect(
        option.rect.left() + (option.rect.width() - actualSize.width()) / 2,
        option.rect.top() + 10, // Consistent top margin
        actualSize.width(),
        actualSize.height()
    );
    
    // Draw a subtle border and shadow for the cover
    painter->setPen(QPen(QColor(80, 80, 80, 120), 1));
    QRect shadowRect = coverRect.adjusted(2, 2, 2, 2);
    painter->fillRect(shadowRect, QColor(0, 0, 0, 30)); // Subtle shadow
    
    // Draw the cover with a 1px border
    painter->drawPixmap(coverRect, cover);
    painter->drawRect(coverRect);
    
    // Draw title if titles are enabled
    if (m_model->showTitles()) {
        // Create text rect exactly below the cover with fixed position
        int textTop = coverRect.bottom() + 5;  // Reduced distance from cover bottom (was 10)
        int textHeight = option.rect.bottom() - textTop - 5;  // Space between bottom of text and edge of item
        
        QRect textRect = QRect(
            option.rect.left() + 5,
            textTop,
            option.rect.width() - 10,
            textHeight
        );
        
        // Create a contrasting background for the text to ensure visibility
        QColor bgColor = option.palette.window().color();
        QColor textColor = option.palette.text().color();
        
        if (option.state & QStyle::State_Selected) {
            // Use selection colors for better visibility when selected
            bgColor = option.palette.highlight().color();
            bgColor.setAlpha(60);  // More transparent for better readability
            textColor = option.palette.highlightedText().color();
        }
        
        // Draw background panel for text - make it slightly wider than the text
        QRect textBgRect = textRect.adjusted(-3, -2, 3, 2); // Smaller vertical padding (was -3, 3)
        painter->fillRect(textBgRect, bgColor);
        painter->setPen(textColor);
        
        // Use a more compact font for the title
        QFont titleFont = option.font;
        titleFont.setBold(true);
        titleFont.setPointSize(option.font.pointSize()); // Don't increase size (was +1)
        painter->setFont(titleFont);
        
        // Elide text properly for display
        // Draw text as a single line with ellipsis if needed
        QFontMetrics fm(titleFont);
        QString elidedText = fm.elidedText(title, Qt::ElideRight, textRect.width() - 10);
        
        // Draw the title text centered in the text area
        painter->drawText(textRect, 
                         Qt::AlignHCenter | Qt::AlignVCenter | Qt::TextSingleLine, // Use single line (was TextWordWrap)
                         elidedText);
    }
    
    painter->restore();
}

QSize RomGridDelegate::sizeHint(const QStyleOptionViewItem& option, 
                              const QModelIndex& index) const
{
    if (!m_model)
        return QSize(200, 200);
    
    // Get the cover size
    QSize coverSize = m_model->coverSize();
    
    // Base width with some extra padding
    int width = coverSize.width() + 30;  // Increased horizontal padding
    
    // Add space for the title if titles are shown
    int height = coverSize.height() + 20; // Base height with margins
    if (m_model->showTitles()) {
        height += 30; // Reduced space for title text (was 50)
    }
    
    return QSize(width, height);
}

// ...rest of the existing code ...

} // namespace QT_UI
