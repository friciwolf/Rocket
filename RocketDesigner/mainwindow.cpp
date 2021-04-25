#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSlider>
#include <QColorDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setFixedSize(size());

    /*
     * Grid Properties Box
     * */

    QSpinBox * rowSpin = findChild<QSpinBox*>("rowSpin");
    rowSpin->setValue(ConfigManager.getRowNumber());

    QSpinBox * columnSpin = findChild<QSpinBox*>("columnSpin");
    columnSpin->setValue(ConfigManager.getColumnNumber());

    QCheckBox * horizontalInversionCheckBox = findChild<QCheckBox*>("horizontalInversionCheckBox");
    horizontalInversionCheckBox->setChecked(ConfigManager.getInvertedScrollFactorXfromSettings());

    QCheckBox * verticalInversionCheckBox = findChild<QCheckBox*>("verticalInversionCheckBox");
    verticalInversionCheckBox->setChecked(ConfigManager.getInvertedScrollFactorYfromSettings());

    QComboBox * gridOrientationCombo = findChild<QComboBox*>("gridOrientationCombo");
    gridOrientationCombo->setCurrentIndex(ConfigManager.getVerticalModeSetting());

    QCheckBox * systemWallpaperCheckBox = findChild<QCheckBox*>("systemWallpaperCheckBox");
    systemWallpaperCheckBox->setChecked(ConfigManager.getUsingSystemWallpaper());

    /*
     * Base Colour Box
     * */
    QColor basecolour = ConfigManager.getBaseColour();

    QLabel * baseLabel_r = findChild<QLabel*>("baseColourLabel_r");
    baseLabel_r->setText(QString::number(basecolour.red()));
    QLabel * baseLabel_g = findChild<QLabel*>("baseColourLabel_g");
    baseLabel_g->setText(QString::number(basecolour.green()));
    QLabel * baseLabel_b = findChild<QLabel*>("baseColourLabel_b");
    baseLabel_b->setText(QString::number(basecolour.blue()));
    QLabel * baseLabel_a = findChild<QLabel*>("baseColourLabel_a");
    baseLabel_a->setText(QString::number(basecolour.alpha()));

    QWidget * baseColourPanel = findChild<QWidget*>("baseColourPanel");
    baseColourPanel->setAutoFillBackground(true);
    baseColourPanel->setPalette(QPalette(QColor(basecolour)));

    QPushButton * baseColourPickerButton = findChild<QPushButton*>("baseColourPickerButton");
    QColorDialog * baseColourDialog = new QColorDialog(nullptr);
    connect(baseColourPickerButton,&QPushButton::clicked,baseColourDialog,[=](){
        QColor newcolor = baseColourDialog->getColor(QColor(baseLabel_r->text().toInt(),baseLabel_g->text().toInt(),baseLabel_b->text().toInt(),baseLabel_a->text().toInt()),nullptr,"Base Color",QColorDialog::ColorDialogOption::ShowAlphaChannel);
        if (newcolor.isValid())
        {
            baseColourPanel->setPalette(QPalette(newcolor));
            baseLabel_r->setText(QString::number(newcolor.red()));
            baseLabel_g->setText(QString::number(newcolor.green()));
            baseLabel_b->setText(QString::number(newcolor.blue()));
            baseLabel_a->setText(QString::number(newcolor.alpha()));
        }
    });

    /*
     * Secondary Colour Box
     * */
    QColor secondarycolour = ConfigManager.getSecondaryColour();

    QLabel * secondaryLabel_r = findChild<QLabel*>("secondaryColourLabel_r");
    secondaryLabel_r->setText(QString::number(secondarycolour.red()));
    QLabel * secondaryLabel_g = findChild<QLabel*>("secondaryColourLabel_g");
    secondaryLabel_g->setText(QString::number(secondarycolour.green()));
    QLabel * secondaryLabel_b = findChild<QLabel*>("secondaryColourLabel_b");
    secondaryLabel_b->setText(QString::number(secondarycolour.blue()));
    QLabel * secondaryLabel_a = findChild<QLabel*>("secondaryColourLabel_a");
    secondaryLabel_a->setText(QString::number(secondarycolour.alpha()));

    QWidget * secondaryColourPanel = findChild<QWidget*>("secondaryColourPanel");
    secondaryColourPanel->setAutoFillBackground(true);
    secondaryColourPanel->setPalette(QPalette(QColor(secondarycolour)));

    QPushButton * secondaryColourPickerButton = findChild<QPushButton*>("secondaryColourPickerButton");
    QColorDialog * secondaryColourDialog = new QColorDialog(nullptr);
    connect(secondaryColourPickerButton,&QPushButton::clicked,secondaryColourDialog,[=](){
        QColor newcolor = secondaryColourDialog->getColor(QColor(secondaryLabel_r->text().toInt(),secondaryLabel_g->text().toInt(),secondaryLabel_b->text().toInt(),secondaryLabel_a->text().toInt()),nullptr,"Secondary Color",QColorDialog::ColorDialogOption::ShowAlphaChannel);
        if (newcolor.isValid())
        {
            secondaryColourPanel->setPalette(QPalette(newcolor));
            secondaryLabel_r->setText(QString::number(newcolor.red()));
            secondaryLabel_g->setText(QString::number(newcolor.green()));
            secondaryLabel_b->setText(QString::number(newcolor.blue()));
            secondaryLabel_a->setText(QString::number(newcolor.alpha()));
        }
    });

    /*
     * Font Size Box
     * */

    QSpinBox * font1Spin = findChild<QSpinBox*>("font1Spin");
    font1Spin->setValue(ConfigManager.getFontSize1());

    QSpinBox * font2Spin = findChild<QSpinBox*>("font2Spin");
    font2Spin->setValue(ConfigManager.getFontSize2());

    QDialogButtonBox * buttonBox = findChild<QDialogButtonBox*>("buttonBox");
    connect(buttonBox->button(QDialogButtonBox::Ok),&QPushButton::clicked,this,&MainWindow::OkClicked);
    connect(buttonBox->button(QDialogButtonBox::Reset),&QPushButton::clicked,this,&MainWindow::ResetClicked);
    connect(buttonBox->button(QDialogButtonBox::Apply),&QPushButton::clicked,this,&MainWindow::ApplyClicked);
    connect(buttonBox->button(QDialogButtonBox::Cancel),&QPushButton::clicked,this,&MainWindow::CancelClicked);
}

void MainWindow::OkClicked()
{
    MainWindow::ApplyClicked();
    MainWindow::CancelClicked();
}

void MainWindow::ResetClicked()
{

}

void MainWindow::ApplyClicked()
{

}

void MainWindow::CancelClicked()
{
    qApp->exit();
}


MainWindow::~MainWindow()
{
    delete ui;
}
