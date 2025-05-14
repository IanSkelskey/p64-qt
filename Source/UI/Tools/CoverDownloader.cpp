#include "CoverDownloader.h"
#include "../../Core/SettingsManager.h"

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

namespace QT_UI {

CoverDownloader::CoverDownloader(QWidget *parent) :
    QDialog(parent),
    m_networkManager(new QNetworkAccessManager(this)),
    m_currentRom(0),
    m_totalRoms(0),
    m_successCount(0),
    m_failCount(0),
    m_isDownloading(false)
{
    setupUi();
    
    connect(m_startButton, &QPushButton::clicked, this, &CoverDownloader::startDownload);
    connect(m_networkManager, &QNetworkAccessManager::finished, this, &CoverDownloader::downloadFinished);
    
    loadSettings();
    
    // Create covers directory if it doesn't exist
    QDir dir;
    dir.mkpath(getCoversDirectory());
    
    m_progressBar->setValue(0);
    m_startButton->setEnabled(true);
}

void CoverDownloader::setupUi()
{
    setWindowTitle(tr("Cover Downloader"));
    resize(600, 500);
    
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // URL Templates group
    QGroupBox* urlGroup = new QGroupBox(tr("URL Templates"), this);
    QVBoxLayout* urlLayout = new QVBoxLayout(urlGroup);
    
    QLabel* urlLabel = new QLabel(tr("Enter URL templates with ${rom_id}, ${internal_name}, or ${rom_name} placeholders:"), this);
    urlLayout->addWidget(urlLabel);
    
    m_urlTextEdit = new QTextEdit(this);
    m_urlTextEdit->setPlaceholderText(tr("https://example.com/covers/${rom_id}.png"));
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

CoverDownloader::~CoverDownloader()
{
    if (m_isDownloading) {
        saveSettings();
    }
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
    m_urlTextEdit->setPlainText(settings.coverUrlTemplates());
    m_useTitleNamesCheckBox->setChecked(settings.coverDownloaderUseTitleNames());
    m_overwriteExistingCheckBox->setChecked(settings.coverDownloaderOverwriteExisting());
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

void CoverDownloader::scanRoms()
{
    // TODO: Implement scanning of ROMs in the configured ROM directory
    // This is a placeholder implementation
    
    // Clear previous data
    m_downloadQueue.clear();
    m_romIdToFilename.clear();
    
    // Placeholder: Populate with sample ROM IDs
    // In a real implementation, this would scan the ROM directory and extract ROM IDs
    m_romIdToFilename["NZLE"] = "The Legend of Zelda - Ocarina of Time"; // Example: Zelda OOT
    m_romIdToFilename["NSMB"] = "Super Mario 64"; // Example: Super Mario 64
    m_romIdToFilename["NK4E"] = "Mario Kart 64"; // Example: Mario Kart 64
    
    // Add ROMs to download queue that don't have covers yet
    for (auto it = m_romIdToFilename.constBegin(); it != m_romIdToFilename.constEnd(); ++it) {
        QString romId = it.key();
        QString coverPath = getCoverFilePath(romId);
        
        // Check if cover exists and if we should overwrite
        bool needsDownload = !QFile::exists(coverPath) || m_overwriteExistingCheckBox->isChecked();
        
        if (needsDownload) {
            m_downloadQueue.enqueue(romId);
        }
    }
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
    
    m_currentRomId = m_downloadQueue.dequeue();
    m_currentRom++;
    
    // Display progress
    m_progressBar->setValue(m_currentRom);
    
    // Get internal name (use filename in this example)
    QString internalName = m_romIdToFilename.value(m_currentRomId);
    
    // Try each URL template
    bool requestSent = false;
    for (const QString &urlTemplate : m_urlTemplates) {
        QString url = replacePlaceholders(urlTemplate, m_currentRomId, internalName);
        if (!url.isEmpty()) {
            updateStatus(tr("Downloading cover for %1 (%2/%3)").arg(internalName).arg(m_currentRom).arg(m_totalRoms));
            
            // Fix vexing parse issue with curly braces
            QNetworkRequest request{QUrl(url)};
            QNetworkReply *reply = m_networkManager->get(request);
            
            connect(reply, &QNetworkReply::downloadProgress, this, &CoverDownloader::onDownloadProgress);
            
            requestSent = true;
            break;
        }
    }
    
    if (!requestSent) {
        // Failed to create a valid URL, skip this ROM
        m_failCount++;
        downloadNext();
    }
}

void CoverDownloader::downloadFinished(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray data = reply->readAll();
        processCoverDownload(data, m_currentRomId);
        m_successCount++;
    } else {
        qDebug() << "Download error:" << reply->errorString();
        m_failCount++;
    }
    
    reply->deleteLater();
    
    // Continue with next download
    downloadNext();
}

void CoverDownloader::processCoverDownload(const QByteArray &data, const QString &romId)
{
    QPixmap pixmap;
    if (pixmap.loadFromData(data)) {
        // Save the image
        QString filePath = getCoverFilePath(romId);
        if (pixmap.save(filePath)) {
            qDebug() << "Cover saved to" << filePath;
        } else {
            qDebug() << "Failed to save cover to" << filePath;
            m_failCount++;
            m_successCount--; // Revert the increment from downloadFinished
        }
    } else {
        qDebug() << "Invalid image data received";
        m_failCount++;
        m_successCount--; // Revert the increment from downloadFinished
    }
}

QString CoverDownloader::replacePlaceholders(const QString &urlTemplate, const QString &romId, const QString &internalName)
{
    QString url = urlTemplate;
    url.replace("${internal_name}", internalName);
    url.replace("${rom_id}", romId);
    url.replace("${rom_name}", internalName); // In this simple example, use internal name as filename
    
    return url;
}

void CoverDownloader::onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    if (bytesTotal > 0) {
        int percent = static_cast<int>((bytesReceived * 100) / bytesTotal);
        updateStatus(tr("Downloading cover for %1 (%2/%3) - %4%")
                    .arg(m_romIdToFilename.value(m_currentRomId))
                    .arg(m_currentRom)
                    .arg(m_totalRoms)
                    .arg(percent));
    }
}

void CoverDownloader::updateStatus(const QString &message)
{
    m_statusLabel->setText(message);
}

QString CoverDownloader::getCoversDirectory()
{
    // Get the default covers directory
    // In a real implementation, this would come from project settings
    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    return appDataPath + "/Covers";
}

QString CoverDownloader::getCoverFilePath(const QString &romId)
{
    QString filename;
    if (m_useTitleNamesCheckBox->isChecked()) {
        filename = m_romIdToFilename.value(romId);
    } else {
        filename = romId;
    }
    
    // Ensure filename is valid for the filesystem
    // Replace QRegExp with QRegularExpression for Qt6
    filename = filename.replace(QRegularExpression("[\\\\/:*?\"<>|]"), "_");
    
    return getCoversDirectory() + "/" + filename + ".png";
}

} // namespace QT_UI
