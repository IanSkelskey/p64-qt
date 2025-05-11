#include "AboutDialog.h"
#include <QVBoxLayout>
#include <QPixmap>
#include <QFont>

namespace QT_UI {

AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUi();
    setVersionText();
}

AboutDialog::~AboutDialog()
{
    // No need to delete UI elements as they are parented to this dialog
}

void AboutDialog::setupUi()
{
    // Set window properties
    setWindowTitle(tr("About Project64"));
    setWindowIcon(QIcon(":/assets/icons/Resources/icons/pj64.ico"));
    resize(823, 600);
    
    QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setSizePolicy(sizePolicy);
    setMinimumHeight(333);
    
    // Main layout
    QVBoxLayout* layout = new QVBoxLayout(this);
    
    // Logo
    m_logoLabel = new QLabel(this);
    m_logoLabel->setLineWidth(1);
    m_logoLabel->setPixmap(QPixmap(":/assets/images/Resources/images/banner.png"));
    m_logoLabel->setScaledContents(false);
    m_logoLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(m_logoLabel);
    
    // Version label
    m_versionLabel = new QLabel(this);
    QFont versionFont = m_versionLabel->font();
    versionFont.setPointSize(11);
    versionFont.setBold(true);
    m_versionLabel->setFont(versionFont);
    m_versionLabel->setText(tr("Version: 1.0.0"));
    layout->addWidget(m_versionLabel);
    
    // About message
    m_aboutMessageLabel = new QLabel(this);
    m_aboutMessageLabel->setText(tr("Project64 is a completely free and open-source emulator for the Nintendo 64 and 64DD written in C++.\n\n"
                                  "Capable of playing your favorite N64 games on your PC with high-definition graphics, "
                                  "excellent compatibility, save states, built-in cheat codes, and more!"));
    m_aboutMessageLabel->setTextFormat(Qt::RichText);
    m_aboutMessageLabel->setWordWrap(true);
    layout->addWidget(m_aboutMessageLabel);
    
    // Special thanks to core members
    m_thanksCoreLabel = new QLabel(this);
    QFont thanksFont = m_thanksCoreLabel->font();
    thanksFont.setPointSize(11);
    thanksFont.setBold(true);
    m_thanksCoreLabel->setFont(thanksFont);
    m_thanksCoreLabel->setText(tr("Special thanks to previous core members:"));
    layout->addWidget(m_thanksCoreLabel);
    
    // Core members list
    m_coreThanksListLabel = new QLabel(this);
    m_coreThanksListLabel->setText(tr("Jabo, Smiff, Gent"));
    layout->addWidget(m_coreThanksListLabel);
    
    // Thanks to others
    m_thankYouLabel = new QLabel(this);
    m_thankYouLabel->setFont(thanksFont);
    m_thankYouLabel->setText(tr("Thanks also goes to:"));
    layout->addWidget(m_thankYouLabel);
    
    // Others list
    m_thankYouListLabel = new QLabel(this);
    m_thankYouListLabel->setText(tr("Jahra!n, Witten, RadeonUser, Azimer, Shygoo, Frank, LuigiBlood, theboy181, "
                                  "Gonetz, BlueToonYoshi, Kimbjo, Melchior, retroben, AIO, and krom"));
    m_thankYouListLabel->setWordWrap(true);
    layout->addWidget(m_thankYouListLabel);
    
    // Add spacer
    layout->addStretch();
    
    // Button box - simplify to only have Close button
    m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Close, this);
    
    // Explicitly connect to reject() which closes the dialog
    connect(m_buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    
    layout->addWidget(m_buttonBox);
}

void AboutDialog::setVersionText()
{
    // In a real implementation, this would come from project constants or build info
    const QString version = "3.0.1";
    const QString buildType = "Qt UI";
    
    m_versionLabel->setText(tr("Version: %1 (%2)").arg(version, buildType));
}

} // namespace QT_UI
