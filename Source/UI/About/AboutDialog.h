#ifndef ABOUT_DIALOG_H
#define ABOUT_DIALOG_H

#include <QDialog>
#include <QLabel>
#include <QDialogButtonBox>

namespace QT_UI {

class AboutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AboutDialog(QWidget *parent = nullptr);
    ~AboutDialog();

private:
    void setupUi();
    void setVersionText();

    // UI elements
    QLabel* m_logoLabel;
    QLabel* m_versionLabel;
    QLabel* m_aboutMessageLabel;
    QLabel* m_thanksCoreLabel;
    QLabel* m_coreThanksListLabel;
    QLabel* m_thankYouLabel;
    QLabel* m_thankYouListLabel;
    QDialogButtonBox* m_buttonBox;
};

} // namespace QT_UI

#endif // ABOUT_DIALOG_H
