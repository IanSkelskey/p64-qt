#include "ConfigDialog.h"
#include "BaseSettingsPage.h"
#include "GeneralSettingsPage.h"
#include "DirectoriesSettingsPage.h"
#include "RomBrowserSettingsPage.h"

#include <QVBoxLayout>
#include <QMessageBox>
#include <QDebug>

namespace QT_UI {

ConfigDialog::ConfigDialog(QWidget *parent)
    : QDialog(parent)
    , m_currentTab(0)
{
    setupUi();
    
    // Connect the dialog buttons
    connect(m_buttonBox, &QDialogButtonBox::accepted, this, [this]() {
        applySettings();
        accept();
    });
    connect(m_buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

ConfigDialog::~ConfigDialog()
{
    // All settings pages are automatically cleaned up as children
}

void ConfigDialog::setupUi()
{
    // Set window properties
    setWindowTitle(tr("Configuration"));
    setWindowIcon(QIcon(":/icons/pj64.ico"));
    resize(650, 550);
    
    // Create main layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // Create tab widget
    m_tabWidget = new QTabWidget(this);
    connect(m_tabWidget, &QTabWidget::currentChanged, this, &ConfigDialog::tabChanged);
    
    // Create settings pages
    addSettingsPage(new GeneralSettingsPage());
    addSettingsPage(new DirectoriesSettingsPage());
    addSettingsPage(new RomBrowserSettingsPage());
    
    // Connect to ROM browser column settings changes
    for (auto page : m_settingsPages) {
        RomBrowserSettingsPage* romBrowserPage = qobject_cast<RomBrowserSettingsPage*>(page);
        if (romBrowserPage) {
            connect(romBrowserPage, &RomBrowserSettingsPage::columnSettingsChanged, 
                    this, &ConfigDialog::columnSettingsChanged);
        }
    }
    
    mainLayout->addWidget(m_tabWidget);
    
    // Add button box
    m_buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Apply | QDialogButtonBox::Reset, 
        this);
    
    connect(m_buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked, 
            this, &ConfigDialog::applySettings);
    connect(m_buttonBox->button(QDialogButtonBox::Reset), &QPushButton::clicked, 
            this, &ConfigDialog::resetSettings);
    
    mainLayout->addWidget(m_buttonBox);
    
    setLayout(mainLayout);
}

void ConfigDialog::addSettingsPage(BaseSettingsPage* page)
{
    if (!page) return;
    
    page->setParent(this);
    m_tabWidget->addTab(page, page->pageTitle());
    m_settingsPages[page->pageTitle()] = page;
    
    // Connect signals
    connect(page, &BaseSettingsPage::settingsChanged, this, [this]() {
        m_buttonBox->button(QDialogButtonBox::Apply)->setEnabled(true);
    });
}

void ConfigDialog::applySettings()
{
    // Save settings in all pages
    for (auto page : m_settingsPages) {
        page->saveSettings();
    }
    
    QMessageBox::information(this, tr("Settings Applied"), tr("Settings have been applied."));
    m_buttonBox->button(QDialogButtonBox::Apply)->setEnabled(false);
}

void ConfigDialog::resetSettings()
{
    // Reset only the current page
    BaseSettingsPage* currentPage = qobject_cast<BaseSettingsPage*>(m_tabWidget->currentWidget());
    if (currentPage) {
        currentPage->resetSettings();
    }
}

void ConfigDialog::tabChanged(int index)
{
    m_currentTab = index;
}

} // namespace QT_UI
