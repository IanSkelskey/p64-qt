#ifndef COVER_DOWNLOADER_H
#define COVER_DOWNLOADER_H

#include <QDialog>
#include <QLineEdit>
#include <QCheckBox>
#include <QProgressBar>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QQueue>
#include <QMap>
#include <QRegularExpression>
#include "../../Core/DatabaseManager.h" // Changed: full include instead of forward declaration

namespace Ui {
class CoverDownloaderDialog;
}

namespace QT_UI {

class CoverDownloader : public QDialog
{
    Q_OBJECT

public:
    explicit CoverDownloader(QWidget *parent = nullptr);
    ~CoverDownloader();

signals:
    void coversDownloaded(int successCount); // New signal for when covers are downloaded

private slots:
    void startDownload();
    void downloadFinished(QNetworkReply *reply);
    void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);

private:
    void setupUi();
    void saveSettings();
    void loadSettings();
    void scanRoms();
    void downloadNext();
    void processCoverDownload(const QByteArray &data, const QString &cartridgeCode);
    void updateStatus(const QString &message);
    QString getCoverFilePath(const QString &cartridgeCode, const QString &romName);
    bool parseRomHeader(const QString &romPath, QString &cartridgeCode, QString &romName);
    QString replacePlaceholders(const QString &urlTemplate, const QString &cartridgeCode, const QString &romName);

    // UI elements
    QTextEdit* m_urlTextEdit;
    QCheckBox* m_useTitleNamesCheckBox;
    QCheckBox* m_overwriteExistingCheckBox;
    QPushButton* m_startButton;
    QProgressBar* m_progressBar;
    QLabel* m_statusLabel;

    // Network manager
    QNetworkAccessManager* m_networkManager;
    
    // Download state
    QString m_baseUrl;
    QQueue<QString> m_downloadQueue;
    QMap<QString, QString> m_cartridgeCodeToRomName;
    QMap<QString, QString> m_cartridgeCodeToRomPath;
    QString m_currentCartridgeCode;
    int m_currentRom;
    int m_totalRoms;
    int m_successCount;
    int m_failCount;
    bool m_isDownloading;
    QStringList m_urlTemplates; // Added back

    // ROM and cover directories
    QString m_romDirectory;
    QString m_coverDirectory;
    
    // Database manager
    DatabaseManager* m_dbManager;
};

} // namespace QT_UI

#endif // COVER_DOWNLOADER_H
