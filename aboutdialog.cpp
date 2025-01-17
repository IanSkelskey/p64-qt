#include "aboutdialog.h"
#include "ui_aboutdialog.h"

AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AboutDialog)
{
    ui->setupUi(this);

    // Set the text and font properties for the QLabel elements
    ui->versionLabel->setText("Version: 1.0.0");
    ui->aboutMessageLabel->setText("Project64 is a completely free and open-source emulator for the Nintendo 64 and 64DD written in C++.\n\nCapable of playing your favorite N64 games on your PC with high-definition graphics, excellent compatibility, save states, built-in cheat codes, and more!");
    ui->thanksCoreLabel->setText("Special thanks to previous core members:");
    ui->coreThanksListLabel->setText("Jabo, Smiff, Gent");
    ui->thankYouLabel->setText("Thanks also goes to:");
    ui->thankYouListLabel->setText("Jahra!n, Witten, RadeonUser, Azimer, Shygoo, Frank, LuigiBlood, theboy181, Gonetz, BlueToonYoshi, Kimbjo, Melchior, retroben, AIO, and krom");

    // Set the font properties for the QLabel elements
    QFont boldFont = ui->versionLabel->font();
    boldFont.setBold(true);
    ui->versionLabel->setFont(boldFont);
    ui->thanksCoreLabel->setFont(boldFont);
    ui->thankYouLabel->setFont(boldFont);
}

AboutDialog::~AboutDialog()
{
    delete ui;
}