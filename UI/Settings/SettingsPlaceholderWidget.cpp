#include "SettingsDialog.h"
#include <QVBoxLayout>
#include <QLabel>

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

} // namespace QT_UI
