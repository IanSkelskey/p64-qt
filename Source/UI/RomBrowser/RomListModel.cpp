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
    // We won't set any default columns here - we'll load them from settings instead
    // If no settings exist, we'll use defaults after trying to load
    
    // Load country icons from resources
    m_countryIcons["USA"] = QIcon(":/flags/usa.svg");
    m_countryIcons["JP"] = QIcon(":/flags/japan.svg");
    m_countryIcons["EU"] = QIcon(":/flags/europe.svg");
    m_countryIcons["AU"] = QIcon(":/flags/australia.svg");
    m_countryIcons["FR"] = QIcon(":/flags/france.svg");
    m_countryIcons["GE"] = QIcon(":/flags/germany.svg");
    m_countryIcons["IT"] = QIcon(":/flags/italy.svg");
    m_countryIcons["SP"] = QIcon(":/flags/spain.svg");
    m_countryIcons["OTHER"] = QIcon(":/flags/unknown.svg");

    m_defaultIcon = QIcon(":/icons/rom_default.png");
    
    // Load default cover image - fix the resource path
    m_defaultCoverImage = QPixmap(":/assets/images/default-label.png");
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
    
    // Load settings - this will set up the columns
    loadSettings();
    
    qDebug() << "RomListModel initialized with" << m_visibleColumns.size() << "columns";
    for (int i = 0; i < m_visibleColumns.size(); i++) {
        qDebug() << "  Column" << i << ":" << columnNameFromEnum(m_visibleColumns[i]);
    }
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

void RomListModel::clearVisibleColumns()
{
    beginResetModel();
    m_visibleColumns.clear();
    endResetModel();
    
    qDebug() << "Cleared all visible columns";
}

void RomListModel::setVisibleColumns(const QVector<RomColumns>& columns)
{
    if (columns.isEmpty()) {
        qDebug() << "Attempted to set empty columns list, ignoring";
        return;
    }
    
    // Skip if the columns haven't actually changed
    if (m_visibleColumns == columns) {
        qDebug() << "Columns unchanged, not updating";
        return;
    }
    
    qDebug() << "Setting visible columns. Requested:" << columns.size() << "columns";
    
    // Log columns being set
    QString columnList;
    for (int i = 0; i < columns.size(); i++) {
        columnList += QString::number(columns[i]);
        if (i < columns.size() - 1) columnList += ", ";
    }
    qDebug() << "New columns:" << columnList;
    
    QVector<RomColumns> oldColumns = m_visibleColumns;
    
    // Use beginResetModel/endResetModel to ensure views fully update
    beginResetModel();
    m_visibleColumns = columns;
    endResetModel();
    
    // Log for debugging
    debugPrintColumns();
    
    // Emit a custom signal that the columns have changed
    emit columnsChanged();
}

// Add this method to enable better debugging
void RomListModel::debugPrintColumns() const
{
    qDebug() << "========= Current visible columns: =========";
    qDebug() << "Count:" << m_visibleColumns.size();
    for (int i = 0; i < m_visibleColumns.size(); i++) {
        qDebug() << "  Column" << i << ": ID=" << m_visibleColumns[i] 
                 << "Name=" << columnNameFromEnum(m_visibleColumns[i]);
    }
    qDebug() << "===========================================";
}

// Add a helper method to get readable column names for debugging
QString RomListModel::columnNameFromEnum(RomColumns column) const
{
    switch (column) {
    case FileName: return "FileName";
    case GoodName: return "GoodName";
    case InternalName: return "InternalName";
    case Size: return "Size";
    case Country: return "Country";
    case ReleaseDate: return "ReleaseDate";
    case Players: return "Players";
    case Genre: return "Genre";
    case Developer: return "Developer";
    case CRC1: return "CRC1";
    case CRC2: return "CRC2";
    case MD5: return "MD5";
    case FilePath: return "FilePath";
    case CartID: return "CartID";
    case MediaType: return "MediaType";
    default: return "Unknown";
    }
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
                                    
    // CRITICAL FIX: DO NOT set default columns at all here - if settings exist, use them exclusively
    QVariant visibleColumnsVar = settings.value("RomBrowser/VisibleColumns");
    if (!visibleColumnsVar.isNull()) {
        QList<QVariant> visibleColumns = visibleColumnsVar.toList();
        QVector<RomColumns> columns;
        
        qDebug() << "Loading columns from settings. Count:" << visibleColumns.size();
        
        // Only process if we have columns defined
        if (!visibleColumns.isEmpty()) {
            foreach(QVariant column, visibleColumns) {
                int colValue = column.toInt();
                if (colValue >= 0 && colValue < ColumnCount) {
                    columns.append(static_cast<RomColumns>(colValue));
                    qDebug() << "  Loading column:" << colValue << "(" << columnNameFromEnum(static_cast<RomColumns>(colValue)) << ")";
                }
            }
            
            // Only set columns if we actually loaded valid ones
            if (!columns.isEmpty()) {
                m_visibleColumns = columns;
                qDebug() << "Applied" << columns.size() << "columns from settings";
                return; // Important: exit here to avoid setting default columns
            }
        }
        
        qDebug() << "No valid columns defined in settings, using defaults";
    } else {
        qDebug() << "No column settings found, using defaults";
    }
    
    // Only get here if no valid columns were loaded from settings
    setDefaultColumns();
}

// Add a new helper method to set default columns
void RomListModel::setDefaultColumns()
{
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
    qDebug() << "Set default columns. Count:" << m_visibleColumns.size();
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

// Helper method implementations
QString RomGridDelegate::cleanTitle(const QString& title, QString& countryCode, QString& version) const
{
    QString cleanedTitle = title;
    
    // Extract country code - common patterns like (U), (E), (J), (USA), etc.
    QRegularExpression countryRegex("\\([A-Z!]\\)|\\((USA|Europe|Japan|Germany|Italy|France|Spain|Australia)\\)");
    QRegularExpressionMatch countryMatch = countryRegex.match(cleanedTitle);
    if (countryMatch.hasMatch()) {
        QString match = countryMatch.captured(0);
        countryCode = match;
        cleanedTitle.remove(match);
    }
    
    // Extract version - patterns like (V1.0), (V1.1), etc.
    QRegularExpression versionRegex("\\(V\\d+\\.\\d+\\)");
    QRegularExpressionMatch versionMatch = versionRegex.match(cleanedTitle);
    if (versionMatch.hasMatch()) {
        QString match = versionMatch.captured(0);
        version = match;
        cleanedTitle.remove(match);
    }
    
    // Clean up any remaining parentheses and extra spaces
    cleanedTitle = cleanedTitle.replace(QRegularExpression("\\([^)]*\\)"), "")
                               .replace(QRegularExpression("\\s+"), " ")
                               .trimmed();
                               
    return cleanedTitle;
}

QIcon RomGridDelegate::getCountryIcon(const QString& countryCode) const
{
    // Map parenthetical country codes to flag icons
    if (countryCode.contains("U") || countryCode.contains("USA"))
        return QIcon(":/flags/usa.svg");
    else if (countryCode.contains("E") || countryCode.contains("Europe"))
        return QIcon(":/flags/europe.svg");
    else if (countryCode.contains("J") || countryCode.contains("Japan"))
        return QIcon(":/flags/japan.svg");
    else if (countryCode.contains("G") || countryCode.contains("Germany"))
        return QIcon(":/flags/germany.svg");
    else if (countryCode.contains("F") || countryCode.contains("France"))
        return QIcon(":/flags/france.svg");
    else if (countryCode.contains("I") || countryCode.contains("Italy"))
        return QIcon(":/flags/italy.svg");
    else if (countryCode.contains("S") || countryCode.contains("Spain"))
        return QIcon(":/flags/spain.svg");
    else if (countryCode.contains("A") || countryCode.contains("Australia"))
        return QIcon(":/flags/australia.svg");
    else
        return QIcon(":/flags/unknown.svg");
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
    QString originalTitle = index.data(Qt::UserRole + 2).toString();
    
    // Extract country code and version from title
    QString countryCode;
    QString version;
    QString cleanedTitle = cleanTitle(originalTitle, countryCode, version);
    
    // Determine if we need to display metadata
    bool hasCountry = !countryCode.isEmpty();
    bool hasVersion = !version.isEmpty();
    bool hasMetadata = hasCountry || hasVersion;
    
    // Ensure the title isn't empty
    if (cleanedTitle.isEmpty()) {
        cleanedTitle = tr("Unknown");
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
    
    // Fixed calculations for cover placement - minimal padding
    QSize coverSize = m_model->coverSize();
    
    // Calculate actual display size maintaining aspect ratio
    QSize actualSize;
    if (cover.isNull()) {
        actualSize = QSize(coverSize.width(), coverSize.height());
    } else {
        actualSize = cover.size().scaled(coverSize.width(), coverSize.height(), Qt::KeepAspectRatio);
    }
    
    // Center the cover in the available space with minimal top padding
    QRect coverRect(
        option.rect.left() + (option.rect.width() - actualSize.width()) / 2,
        option.rect.top() + 3, // Minimal top padding
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
        // Create text rect immediately below the cover
        int textTop = coverRect.bottom() + 2; // Minimal space between cover and title
        
        QRect textRect = QRect(
            option.rect.left() + 5,
            textTop,
            option.rect.width() - 10,
            18  // Compact height for title
        );
        
        // Create a contrasting background for the text area
        QColor bgColor = option.palette.window().color();
        QColor textColor = option.palette.text().color();
        
        if (option.state & QStyle::State_Selected) {
            bgColor = option.palette.highlight().color();
            bgColor.setAlpha(60);
            textColor = option.palette.highlightedText().color();
        }
        
        // Calculate precise background height based on content
        int bgHeight = 18; // Default title height
        if (hasMetadata) {
            bgHeight += 14; // Add just enough for metadata
        }
        
        // Draw background for text area
        QRect textBgRect = textRect.adjusted(-2, -2, 2, 2);
        textBgRect.setHeight(bgHeight);
        painter->fillRect(textBgRect, bgColor);
        
        // Set font for the main title
        QFont titleFont = option.font;
        titleFont.setBold(true);
        painter->setFont(titleFont);
        painter->setPen(textColor);
        
        // Elide text properly for display
        QFontMetrics fm(titleFont);
        QString elidedText = fm.elidedText(cleanedTitle, Qt::ElideRight, textRect.width() - 10);
        
        // Draw the title text centered
        painter->drawText(textRect, 
                         Qt::AlignHCenter | Qt::AlignVCenter | Qt::TextSingleLine,
                         elidedText);
        
        // Only draw metadata if we have any
        if (hasMetadata) {
            // Position metadata immediately below the title
            QRect metadataRect = QRect(
                textRect.left(),
                textRect.bottom() - 1, // Overlap slightly for compactness
                textRect.width(),
                14  // Minimal height for metadata
            );
            
            // Set a smaller font for metadata
            QFont metadataFont = option.font;
            metadataFont.setPointSize(qMax(metadataFont.pointSize() - 1, 8)); // Ensure a minimum reasonable size
            painter->setFont(metadataFont);
            
            // Determine positions based on what metadata is available
            if (hasCountry && hasVersion) {
                // Draw both country flag and version
                QRect flagRect = QRect(
                    metadataRect.left() + (metadataRect.width() / 2) - 25,
                    metadataRect.top(),
                    14,
                    14
                );
                
                QIcon countryIcon = getCountryIcon(countryCode);
                if (!countryIcon.isNull()) {
                    countryIcon.paint(painter, flagRect);
                }
                
                QRect versionRect = QRect(
                    metadataRect.left() + (metadataRect.width() / 2) + 5,
                    metadataRect.top(),
                    40,
                    14
                );
                
                // Clean up the version string
                version.remove("(").remove(")");
                painter->drawText(versionRect, 
                                 Qt::AlignLeft | Qt::AlignVCenter,
                                 version);
            } else if (hasCountry) {
                // Only country flag - center it
                QRect flagRect = QRect(
                    metadataRect.left() + (metadataRect.width() - 14) / 2,
                    metadataRect.top(),
                    14,
                    14
                );
                
                QIcon countryIcon = getCountryIcon(countryCode);
                if (!countryIcon.isNull()) {
                    countryIcon.paint(painter, flagRect);
                }
            } else if (hasVersion) {
                // Only version - center it
                QRect versionRect = QRect(
                    metadataRect.left(),
                    metadataRect.top(),
                    metadataRect.width(),
                    14
                );
                
                // Clean up the version string
                version.remove("(").remove(")");
                painter->drawText(versionRect, 
                                 Qt::AlignCenter | Qt::AlignVCenter,
                                 version);
            }
        }
    }
    
    painter->restore();
}

QSize RomGridDelegate::sizeHint(const QStyleOptionViewItem& option, 
                              const QModelIndex& index) const
{
    if (!m_model)
        return QSize(200, 200);
    
    // Get cover size from the model
    QSize coverSize = m_model->coverSize();
    
    // Base width with reasonable padding
    int width = coverSize.width() + 20;
    
    // Get metadata to determine if we need extra height
    QString originalTitle = index.data(Qt::UserRole + 2).toString();
    QString countryCode, version;
    cleanTitle(originalTitle, countryCode, version);
    bool hasMetadata = !countryCode.isEmpty() || !version.isEmpty();
    
    // Calculate the height needed based on the content
    int height = coverSize.height() + 15; // Base height with padding
    
    if (m_model->showTitles()) {
        // Add height based on if we have metadata
        height += hasMetadata ? 35 : 25; // Reasonable height for title (and metadata if present)
    }
    
    return QSize(width, height);
}

QVector<RomListModel::RomColumns> RomListModel::visibleColumns() const
{
    return m_visibleColumns;
}

} // namespace QT_UI
