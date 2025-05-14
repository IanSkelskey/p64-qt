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
#include <QRegularExpression> // Add this for Qt6

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
    void processCoverDownload(const QByteArray &data, const QString &romId);
    QString replacePlaceholders(const QString &urlTemplate, const QString &romId, const QString &internalName);
    void updateStatus(const QString &message);
    QString getCoversDirectory();
    QString getCoverFilePath(const QString &romId);

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
    QStringList m_urlTemplates;
    QQueue<QString> m_downloadQueue;
    QMap<QString, QString> m_romIdToFilename;
    QString m_currentRomId;
    int m_currentRom;
    int m_totalRoms;
    int m_successCount;
    int m_failCount;
    bool m_isDownloading;
};

} // namespace QT_UI

#endif // COVER_DOWNLOADER_H
