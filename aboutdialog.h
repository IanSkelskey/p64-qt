#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>
#include <QLabel>

namespace Ui {
class AboutDialog;
}

class AboutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AboutDialog(QWidget *parent = nullptr);
    ~AboutDialog();

private:
    Ui::AboutDialog *ui;
    QLabel *logoLabel;
    QLabel *versionLabel;
    QLabel *aboutMessageLabel;
    QLabel *thanksCoreLabel;
    QLabel *coreThanksListLabel;
    QLabel *thankYouLabel;
    QLabel *thankYouListLabel;
};

#endif // ABOUTDIALOG_H