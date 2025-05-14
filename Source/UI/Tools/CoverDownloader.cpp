#include "CoverDownloader.h"
#include "../../Core/SettingsManager.h"
#include "../../Core/RomParser.h"

#include <QSettings>
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <QStandardPaths>
#include <QDebug>
#include <QPixmap>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QRegularExpression>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QApplication>

namespace QT_UI {

CoverDownloader::CoverDownloader(QWidget *parent) :
    QDialog(parent),
    m_networkManager(new QNetworkAccessManager(this)),
    m_currentRom(0),
    m_totalRoms(0),
    m_successCount(0),
    m_failCount(0),
    m_isDownloading(false),
    m_dbManager(nullptr)
{
    setupUi();
    
    connect(m_startButton, &QPushButton::clicked, this, &CoverDownloader::startDownload);
    connect(m_networkManager, &QNetworkAccessManager::finished, this, &CoverDownloader::downloadFinished);
    
    loadSettings();
    
    // Set the default base URL for GitHub repository
    m_baseUrl = "https://raw.githubusercontent.com/IanSkelskey/n64-covers/refs/heads/main/labels/";
    
    // Create database manager instance
    QString dbPath = QDir(QApplication::applicationDirPath()).filePath("database.sqlite");
    m_dbManager = new DatabaseManager(dbPath);
    
    if (!m_dbManager->open()) {
        qWarning() << "Failed to open ROM database";
    }
    
    // Create covers directory if it doesn't exist
    QDir dir;
    dir.mkpath(m_coverDirectory);
    
    m_progressBar->setValue(0);
    m_startButton->setEnabled(true);
}

CoverDownloader::~CoverDownloader()
{
    if (m_isDownloading) {
        saveSettings();
    }
    
    if (m_dbManager) {
        m_dbManager->close();
        delete m_dbManager;
    }
}

void CoverDownloader::setupUi()
{
    setWindowTitle(tr("Cover Downloader"));
    resize(600, 500);
    
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // URL Templates group - RESTORED
    QGroupBox* urlGroup = new QGroupBox(tr("URL Templates"), this);
    QVBoxLayout* urlLayout = new QVBoxLayout(urlGroup);
    
    QLabel* urlLabel = new QLabel(tr("Enter URL templates with ${cartridge_code}, ${internal_name}, or ${rom_name} placeholders:"), this);
    urlLayout->addWidget(urlLabel);
    
    m_urlTextEdit = new QTextEdit(this);
    m_urlTextEdit->setPlaceholderText(tr("https://example.com/covers/${cartridge_code}.png"));
    urlLayout->addWidget(m_urlTextEdit);
    
    mainLayout->addWidget(urlGroup);
    
    // Options group
    QGroupBox* optionsGroup = new QGroupBox(tr("Options"), this);
    QVBoxLayout* optionsLayout = new QVBoxLayout(optionsGroup);
    
    m_useTitleNamesCheckBox = new QCheckBox(tr("Use ROM title names for filenames"), this);
    optionsLayout->addWidget(m_useTitleNamesCheckBox);
    
    m_overwriteExistingCheckBox = new QCheckBox(tr("Overwrite existing covers"), this);
    optionsLayout->addWidget(m_overwriteExistingCheckBox);
    
    mainLayout->addWidget(optionsGroup);
    
    // Status area
    QGroupBox* statusGroup = new QGroupBox(tr("Status"), this);
    QVBoxLayout* statusLayout = new QVBoxLayout(statusGroup);
    
    m_statusLabel = new QLabel(tr("Ready"), this);
    statusLayout->addWidget(m_statusLabel);
    
    m_progressBar = new QProgressBar(this);
    m_progressBar->setMinimum(0);
    statusLayout->addWidget(m_progressBar);
    
    mainLayout->addWidget(statusGroup);
    
    // Buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    
    m_startButton = new QPushButton(tr("Start"), this);
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_startButton);
    
    QPushButton* closeButton = new QPushButton(tr("Close"), this);
    connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);
    buttonLayout->addWidget(closeButton);
    
    mainLayout->addLayout(buttonLayout);
    
    setLayout(mainLayout);
}

void CoverDownloader::saveSettings()
{
    auto& settings = QT_UI::SettingsManager::instance();
    settings.setCoverUrlTemplates(m_urlTextEdit->toPlainText());
    settings.setCoverDownloaderUseTitleNames(m_useTitleNamesCheckBox->isChecked());
    settings.setCoverDownloaderOverwriteExisting(m_overwriteExistingCheckBox->isChecked());
}

void CoverDownloader::loadSettings()
{
    auto& settings = QT_UI::SettingsManager::instance();
    
    // Get ROM and cover directories from SettingsManager
    m_romDirectory = settings.lastRomDirectory();
    m_coverDirectory = settings.coverDirectory();
    
    // Load user preferences
    m_urlTextEdit->setPlainText(settings.coverUrlTemplates());
    m_useTitleNamesCheckBox->setChecked(settings.coverDownloaderUseTitleNames());
    m_overwriteExistingCheckBox->setChecked(settings.coverDownloaderOverwriteExisting());
}

bool CoverDownloader::parseRomHeader(const QString &romPath, QString &cartridgeCode, QString &romName)
{
    // Open the ROM file and read its header
    QFile file(romPath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open ROM file:" << romPath;
        return false;
    }
    
    // Read first 4KB of the ROM (should be enough for header)
    QByteArray headerData = file.read(4096);
    file.close();
    
    // Parse ROM header using RomParser
    RomParser parser;
    if (!parser.setRomData(headerData)) {
        qWarning() << "Failed to parse ROM header:" << romPath;
        return false;
    }
    
    // Get the cartridge ID from ROM header (for database lookup)
    QString cartridgeID = parser.extractCartID();
    if (cartridgeID.isEmpty()) {
        qWarning() << "No cartridge ID found in ROM:" << romPath;
        return false;
    }
    
    // Get the internal name for the ROM
    romName = parser.extractInternalName();
    if (romName.isEmpty()) {
        // Fallback to the file name without extension
        QFileInfo fileInfo(romPath);
        romName = fileInfo.completeBaseName();
    }
    
    // Calculate CRC values for database lookup
    uint32_t crc1 = 0, crc2 = 0;
    parser.calculateCRC(crc1, crc2);
    
    // Get country code byte from ROM header for database lookup
    unsigned char countryByte = parser.getRawCountryByte();
    QString countryHex = QString::number(countryByte, 16).toUpper().rightJustified(2, '0');
    
    // Get full cartridge code from database using ROM ID info
    cartridgeCode = cartridgeID; // Default to ROM header cartridge ID if database lookup fails
    
    if (m_dbManager && m_dbManager->isDatabaseLoaded()) {
        // Try to get cartridge code from database using CRC values first (most accurate)
        std::map<QString, QVariant> romInfo = m_dbManager->getRomBrowserEntryByCRC(crc1, crc2, countryHex);
        
        if (!romInfo.empty() && romInfo.count("cartridge_code") && romInfo["cartridge_code"].isValid()) {
            // Use the official cartridge code from database
            cartridgeCode = romInfo["cartridge_code"].toString();
            qDebug() << "Found cartridge code in database:" << cartridgeCode << "for ROM ID:" << cartridgeID;
            
            // Also update the ROM name if available
            if (romInfo.count("good_name") && romInfo["good_name"].isValid() && !romInfo["good_name"].toString().isEmpty()) {
                romName = romInfo["good_name"].toString();
            }
            
            return true;
        }
        
        // Fallback: try lookup by cartridge ID from ROM header
        romInfo = m_dbManager->getRomInfoByCartridgeCode(cartridgeID);
        if (!romInfo.empty() && romInfo.count("cartridge_code") && romInfo["cartridge_code"].isValid()) {
            cartridgeCode = romInfo["cartridge_code"].toString();
            qDebug() << "Found cartridge code by ID lookup:" << cartridgeCode;
            
            // Also update the ROM name if available
            if (romInfo.count("good_name") && romInfo["good_name"].isValid() && !romInfo["good_name"].toString().isEmpty()) {
                romName = romInfo["good_name"].toString();
            }
        }
    }
    
    qDebug() << "Using cartridge code:" << cartridgeCode << "for" << romName;
    return true;
}

void CoverDownloader::scanRoms()
{
    // Clear previous data
    m_downloadQueue.clear();
    m_cartridgeCodeToRomName.clear();
    m_cartridgeCodeToRomPath.clear();
    
    if (m_romDirectory.isEmpty() || !QDir(m_romDirectory).exists()) {
        updateStatus(tr("Invalid ROM directory. Please set a valid ROM directory."));
        return;
    }
    
    updateStatus(tr("Scanning for ROMs in %1...").arg(m_romDirectory));
    
    // Scan ROM directory for N64 ROMs
    QStringList romExtensions = {"*.z64", "*.v64", "*.n64", "*.zip"};
    QDir romDir(m_romDirectory);
    QStringList romFiles = romDir.entryList(romExtensions, QDir::Files);
    
    auto& settings = QT_UI::SettingsManager::instance();
    bool recursive = settings.recursiveScan();
    
    // Process files found in the main directory
    for (const QString& romFileName : romFiles) {
        QString romPath = romDir.filePath(romFileName);
        
        QString cartridgeCode, romName;
        if (parseRomHeader(romPath, cartridgeCode, romName)) {
            // The cartridge code should now come from the database if available
            
            // Add to our maps
            m_cartridgeCodeToRomName[cartridgeCode] = romName;
            m_cartridgeCodeToRomPath[cartridgeCode] = romPath;
            
            // Check if cover exists
            QString coverPath = getCoverFilePath(cartridgeCode, romName);
            
            // Add to download queue if needed
            bool needsDownload = !QFile::exists(coverPath) || m_overwriteExistingCheckBox->isChecked();
            if (needsDownload) {
                m_downloadQueue.enqueue(cartridgeCode);
            }
        }
    }
    
    // Also check recursively if enabled
    if (recursive) {
        QDirIterator it(m_romDirectory, romExtensions, QDir::Files, QDirIterator::Subdirectories);
        while (it.hasNext()) {
            QString romPath = it.next();
            
            // Skip files we've already processed from the main directory
            if (m_cartridgeCodeToRomPath.values().contains(romPath)) {
                continue;
            }
            
            QString cartridgeCode, romName;
            if (parseRomHeader(romPath, cartridgeCode, romName)) {
                // Get additional info from database if available
                if (m_dbManager && m_dbManager->isDatabaseLoaded()) {
                    std::map<QString, QVariant> romInfo = m_dbManager->getRomInfoByCartridgeCode(cartridgeCode);
                    if (!romInfo.empty() && romInfo.count("good_name") && !romInfo["good_name"].toString().isEmpty()) {
                        romName = romInfo["good_name"].toString();
                    }
                }
                
                // Add to our maps
                m_cartridgeCodeToRomName[cartridgeCode] = romName;
                m_cartridgeCodeToRomPath[cartridgeCode] = romPath;
                
                // Check if cover exists
                QString coverPath = getCoverFilePath(cartridgeCode, romName);
                
                // Add to download queue if needed
                bool needsDownload = !QFile::exists(coverPath) || m_overwriteExistingCheckBox->isChecked();
                if (needsDownload) {
                    m_downloadQueue.enqueue(cartridgeCode);
                }
            }
        }
    }
    
    updateStatus(tr("Found %1 ROMs. %2 covers need to be downloaded.")
                .arg(m_cartridgeCodeToRomName.size())
                .arg(m_downloadQueue.size()));
}

void CoverDownloader::startDownload()
{
    if (m_isDownloading) {
        // Cancel current download process
        m_downloadQueue.clear();
        m_isDownloading = false;
        m_startButton->setText(tr("Start"));
        updateStatus(tr("Download cancelled."));
        return;
    }
    
    // Get URL templates
    QString urlTemplatesText = m_urlTextEdit->toPlainText();
    if (urlTemplatesText.isEmpty()) {
        QMessageBox::warning(this, tr("Error"), tr("Please enter at least one URL template."));
        return;
    }
    
    m_urlTemplates = urlTemplatesText.split('\n', Qt::SkipEmptyParts);
    if (m_urlTemplates.isEmpty()) {
        QMessageBox::warning(this, tr("Error"), tr("Please enter at least one URL template."));
        return;
    }
    
    saveSettings();
    
    // Start the download process
    m_isDownloading = true;
    m_startButton->setText(tr("Cancel"));
    m_successCount = 0;
    m_failCount = 0;
    
    // Scan ROMs and build download queue
    scanRoms();
    
    if (m_downloadQueue.isEmpty()) {
        updateStatus(tr("No covers to download."));
        m_isDownloading = false;
        m_startButton->setText(tr("Start"));
        return;
    }
    
    m_currentRom = 0;
    m_totalRoms = m_downloadQueue.size();
    m_progressBar->setMaximum(m_totalRoms);
    
    updateStatus(tr("Starting download of %1 covers...").arg(m_totalRoms));
    
    // Start the download process
    downloadNext();
}

void CoverDownloader::downloadNext()
{
    if (m_downloadQueue.isEmpty() || !m_isDownloading) {
        // All downloads completed or cancelled
        updateStatus(tr("Download complete. Success: %1, Failed: %2").arg(m_successCount).arg(m_failCount));
        m_isDownloading = false;
        m_startButton->setText(tr("Start"));
        return;
    }
    
    m_currentCartridgeCode = m_downloadQueue.dequeue();
    m_currentRom++;
    
    // Display progress
    m_progressBar->setValue(m_currentRom);
    
    // Get ROM name
    QString romName = m_cartridgeCodeToRomName.value(m_currentCartridgeCode, "Unknown");
    
    // Try each URL template
    bool requestSent = false;
    for (const QString &urlTemplate : m_urlTemplates) {
        QString url = replacePlaceholders(urlTemplate, m_currentCartridgeCode, romName);
        if (!url.isEmpty()) {
            updateStatus(tr("Downloading cover for %1 (%2) - %3/%4")
                        .arg(romName)
                        .arg(m_currentCartridgeCode)
                        .arg(m_currentRom)
                        .arg(m_totalRoms));
            
            // Create network request
            QNetworkRequest request{QUrl(url)};
            QNetworkReply *reply = m_networkManager->get(request);
            
            connect(reply, &QNetworkReply::downloadProgress, this, &CoverDownloader::onDownloadProgress);
            
            requestSent = true;
            break;
        }
    }
    
    // If no template worked, try the default GitHub URL as fallback
    if (!requestSent && !m_baseUrl.isEmpty()) {
        QString url = m_baseUrl + m_currentCartridgeCode;
        updateStatus(tr("Trying default URL for %1 (%2) - %3/%4")
                    .arg(romName)
                    .arg(m_currentCartridgeCode)
                    .arg(m_currentRom)
                    .arg(m_totalRoms));
                    
        QNetworkRequest request{QUrl(url)};
        QNetworkReply *reply = m_networkManager->get(request);
        connect(reply, &QNetworkReply::downloadProgress, this, &CoverDownloader::onDownloadProgress);
        requestSent = true;
    }
    
    if (!requestSent) {
        // Failed to create a valid URL, skip this ROM
        m_failCount++;
        downloadNext();
    }
}

QString CoverDownloader::replacePlaceholders(const QString &urlTemplate, const QString &cartridgeCode, const QString &romName)
{
    QString url = urlTemplate;
    url.replace("${cartridge_code}", cartridgeCode);
    url.replace("${internal_name}", romName); // Using the ROM name as internal name
    url.replace("${rom_name}", romName);
    
    // Legacy support for older placeholder formats
    url.replace("${rom_id}", cartridgeCode);
    url.replace("${product_id}", cartridgeCode);
    
    return url;
}

void CoverDownloader::downloadFinished(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray data = reply->readAll();
        processCoverDownload(data, m_currentCartridgeCode);
        m_successCount++;
    } else {
        qDebug() << "Download error for" << m_currentCartridgeCode << ":" << reply->errorString();
        m_failCount++;
    }
    
    reply->deleteLater();
    
    // Continue with next download
    downloadNext();
}

void CoverDownloader::processCoverDownload(const QByteArray &data, const QString &cartridgeCode)
{
    QPixmap pixmap;
    QString romName = m_cartridgeCodeToRomName.value(cartridgeCode, "Unknown");
    
    if (pixmap.loadFromData(data)) {
        // Save the image
        QString filePath = getCoverFilePath(cartridgeCode, romName);
        
        // Ensure the directory exists
        QFileInfo fileInfo(filePath);
        QDir dir;
        dir.mkpath(fileInfo.absolutePath());
        
        if (pixmap.save(filePath)) {
            qDebug() << "Cover saved to" << filePath;
        } else {
            qDebug() << "Failed to save cover to" << filePath;
            m_failCount++;
            m_successCount--; // Revert the increment from downloadFinished
        }
    } else {
        qDebug() << "Invalid image data received for" << cartridgeCode;
        m_failCount++;
        m_successCount--; // Revert the increment from downloadFinished
    }
}

void CoverDownloader::onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    if (bytesTotal > 0) {
        int percent = static_cast<int>((bytesReceived * 100) / bytesTotal);
        QString romName = m_cartridgeCodeToRomName.value(m_currentCartridgeCode, "Unknown");
        
        updateStatus(tr("Downloading cover for %1 (%2) - %3/%4 - %5%")
                    .arg(romName)
                    .arg(m_currentCartridgeCode)
                    .arg(m_currentRom)
                    .arg(m_totalRoms)
                    .arg(percent));
    }
}

QString CoverDownloader::getCoverFilePath(const QString &cartridgeCode, const QString &romName)
{
    QString filename;
    
    // Use either the ROM name or cartridge code based on user preference
    if (m_useTitleNamesCheckBox->isChecked() && !romName.isEmpty()) {
        filename = romName;
    } else {
        filename = cartridgeCode;
    }
    
    // Ensure filename is valid for the filesystem
    filename = filename.replace(QRegularExpression("[\\\\/:*?\"<>|]"), "_");
    
    // Make sure cover directory ends with a slash
    QString coverDir = m_coverDirectory;
    if (!coverDir.endsWith('/') && !coverDir.endsWith('\\')) {
        coverDir += '/';
    }
    
    return coverDir + filename + ".png";
}

void CoverDownloader::updateStatus(const QString &message)
{
    m_statusLabel->setText(message);
    qDebug() << message; // Log status messages
}

} // namespace QT_UI
