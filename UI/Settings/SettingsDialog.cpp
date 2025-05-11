#include "SettingsDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>

namespace QT_UI {

SettingsPlaceholderWidget::SettingsPlaceholderWidget(const QString& title, QWidget* parent)
    : QWidget(parent), m_title(title)
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    
    // Create a placeholder label
    QLabel* placeholderLabel = new QLabel(tr("This is a placeholder for the %1 settings page.").arg(title), this);
    placeholderLabel->setAlignment(Qt::AlignCenter);
    placeholderLabel->setWordWrap(true);
    
    QFont font = placeholderLabel->font();
    font.setPointSize(12);
    placeholderLabel->setFont(font);
    
    layout->addWidget(placeholderLabel);
    layout->addStretch();
}

SettingsDialog::SettingsDialog(QWidget* parent)
    : QDialog(parent),
      m_tabWidget(nullptr),
      m_applyButton(nullptr),
      m_okButton(nullptr),
      m_cancelButton(nullptr),
      m_resetButton(nullptr),
      m_generalSettings(nullptr),
      m_graphicsSettings(nullptr),
      m_audioSettings(nullptr),
      m_inputSettings(nullptr),
      m_advancedSettings(nullptr)
{
    setupUi();
}

SettingsDialog::~SettingsDialog()
{
    // Cleanup if needed
}

void SettingsDialog::setupUi()
{
    setWindowTitle(tr("Project64 Settings"));
    setMinimumSize(600, 500);
    
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // Create tab widget
    m_tabWidget = new QTabWidget(this);
    
    // Create setting pages
    m_generalSettings = new SettingsPlaceholderWidget(tr("General"), this);
    m_graphicsSettings = new SettingsPlaceholderWidget(tr("Graphics"), this);
    m_audioSettings = new SettingsPlaceholderWidget(tr("Audio"), this);
    m_inputSettings = new SettingsPlaceholderWidget(tr("Input"), this);
    m_advancedSettings = new SettingsPlaceholderWidget(tr("Advanced"), this);
    
    // Add tabs
    m_tabWidget->addTab(m_generalSettings, tr("General"));
    m_tabWidget->addTab(m_graphicsSettings, tr("Graphics"));
    m_tabWidget->addTab(m_audioSettings, tr("Audio"));
    m_tabWidget->addTab(m_inputSettings, tr("Input"));
    m_tabWidget->addTab(m_advancedSettings, tr("Advanced"));
    
    mainLayout->addWidget(m_tabWidget);
    
    // Create button layout
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    
    m_resetButton = new QPushButton(tr("Reset Page"), this);
    m_applyButton = new QPushButton(tr("Apply"), this);
    m_okButton = new QPushButton(tr("OK"), this);
    m_cancelButton = new QPushButton(tr("Cancel"), this);
    
    buttonLayout->addWidget(m_resetButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_applyButton);
    buttonLayout->addWidget(m_okButton);
    buttonLayout->addWidget(m_cancelButton);
    
    mainLayout->addLayout(buttonLayout);
    
    // Connect signals
    connect(m_applyButton, &QPushButton::clicked, this, &SettingsDialog::onApply);
    connect(m_okButton, &QPushButton::clicked, this, &SettingsDialog::onOk);
    connect(m_cancelButton, &QPushButton::clicked, this, &SettingsDialog::onCancel);
    connect(m_resetButton, &QPushButton::clicked, this, &SettingsDialog::onReset);
    connect(m_tabWidget, &QTabWidget::currentChanged, this, &SettingsDialog::onTabChanged);
}

void SettingsDialog::applySettings()
{
    // Implement settings application logic
    // This is just a placeholder implementation
    QMessageBox::information(this, tr("Settings Applied"), tr("Settings have been applied."));
}

int SettingsDialog::exec(int initialTab)
{
    if (initialTab >= 0 && initialTab < m_tabWidget->count()) {
        m_tabWidget->setCurrentIndex(initialTab);
    }
    return QDialog::exec();
}

void SettingsDialog::onApply()
{
    applySettings();
}

void SettingsDialog::onOk()
{
    applySettings();
    accept();
}

void SettingsDialog::onCancel()
{
    reject();
}

void SettingsDialog::onReset()
{
    // Reset current tab settings to default
    QMessageBox::information(this, tr("Reset"), tr("Settings on this page have been reset to default."));
}

void SettingsDialog::onTabChanged(int index)
{
    // Handle tab changes if needed
}

} // namespace QT_UI
