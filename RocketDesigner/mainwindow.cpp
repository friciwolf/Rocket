#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSlider>
#include <QColorDialog>
#include <QFileDialog>
#include <QDir>
#include <QSpinBox>
#include <QProcess>

#include <KConfigGroup>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setFixedSize(size());

    /*
     * Rocket Meta Setting
     * */
    m_rocketMetaCheckBox = findChild<QCheckBox*>("rocketMetaCheckBox");
    m_rocketMetaCheckBox->setChecked(ConfigManager.getStyleValue(RocketConfig::Settings::group,RocketConfig::Settings::metastart,RocketStyle::kwin_start_meta));

    /*
     * Grid Properties Box
     * */

    m_rowSpin = findChild<QSpinBox*>("rowSpin");
    m_rowSpin->setValue(ConfigManager.getRowNumber());

    m_columnSpin = findChild<QSpinBox*>("columnSpin");
    m_columnSpin->setValue(ConfigManager.getColumnNumber());

    m_horizontalInversionCheckBox = findChild<QCheckBox*>("horizontalInversionCheckBox");
    m_horizontalInversionCheckBox->setChecked(ConfigManager.getStyleValue(RocketConfig::Settings::group,RocketConfig::Settings::invertedscrollingx,RocketStyle::inverted_scrolling_x));

    m_verticalInversionCheckBox = findChild<QCheckBox*>("verticalInversionCheckBox");
    m_verticalInversionCheckBox->setChecked(ConfigManager.getStyleValue(RocketConfig::Settings::group,RocketConfig::Settings::invertedscrollingy,RocketStyle::inverted_scrolling_y));

    m_gridOrientationCombo = findChild<QComboBox*>("gridOrientationCombo");
    m_gridOrientationCombo->setCurrentIndex(ConfigManager.getVerticalModeSetting());

    m_enableBoxesCheckBox = findChild<QCheckBox*>("enableBoxesCheckBox");
    m_enableBoxesCheckBox->setChecked(ConfigManager.getBoxSetting());

    m_systemWallpaperCheckBox = findChild<QCheckBox*>("systemWallpaperCheckBox");
    m_systemWallpaperCheckBox->setChecked(ConfigManager.getUsingSystemWallpaper());

    m_systemWallpaperScreenSpin = findChild<QSpinBox*>("systemWallpaperScreenSpin");
    KConfig config(QDir::homePath()+"/.config/plasma-org.kde.plasma.desktop-appletsrc");
    QStringList wallpaper_candidates;
    QStringList list = config.group("Containments").groupList();
    for (QString l : list)
    {
        if (config.group("Containments").group(l).groupList().contains("Wallpaper"))
        {
            QString variable = config.group("Containments").group(l).group("Wallpaper").group("org.kde.image").group("General").readEntry("Image");
            if (variable.split("file://").size()>1)
            {
                wallpaper_candidates.push_back(config.group("Containments").group(l).readEntry("lastScreen")+variable.split("file://")[1]);
            }
            else
            {
                if (variable.at(variable.size()-1)!="/") wallpaper_candidates.push_back(config.group("Containments").group(l).readEntry("lastScreen")+variable);
            }
        }
    }
    wallpaper_candidates.sort();
    int candidate = (ConfigManager.getWallpaperScreen()>=wallpaper_candidates.size() ? 0 : ConfigManager.getWallpaperScreen());
    m_systemWallpaperScreenSpin->setMaximum(wallpaper_candidates.size()-1);
    m_systemWallpaperScreenSpin->setValue(candidate);

    m_systemWallpaperBlurSpin = findChild<QSpinBox*>("systemWallpaperBlurSpin");
    m_systemWallpaperBlurSpin->setValue(ConfigManager.getBlurRadius());

    QString wallpaper_path = wallpaper_candidates[candidate].right(wallpaper_candidates[candidate].size()-1);
    if (!ConfigManager.getUsingSystemWallpaper()) wallpaper_path= QDir::homePath()+"/.config/rocket/wallpaper.jpeg";
    QLabel * wallpaperLabel = findChild<QLabel*>("wallpaperLabel");
    wallpaperLabel->setText(wallpaper_path.split("/").last());
    wallpaperLabel->setToolTip(wallpaper_path);
    QLabel * currentWallpaperLabel = findChild<QLabel*>("currentWallpaperLabel");
    currentWallpaperLabel->setToolTip(wallpaper_path);

    connect(m_systemWallpaperCheckBox,&QCheckBox::toggled,wallpaperLabel,[=](){
        QString wallpaper_path = wallpaper_candidates[candidate].right(wallpaper_candidates[candidate].size()-1);
        if (!m_systemWallpaperCheckBox->isChecked()) wallpaper_path = QDir::homePath()+"/.config/rocket/wallpaper.jpeg";
        wallpaperLabel->setText(wallpaper_path.split("/").last());
        wallpaperLabel->setToolTip(wallpaper_path);
        currentWallpaperLabel->setToolTip(wallpaper_path);
    });

    connect(m_systemWallpaperScreenSpin,QOverload<int>::of(&QSpinBox::valueChanged),wallpaperLabel,[=](int i){
        int candidate = i;
        QString wallpaper_path = wallpaper_candidates[candidate].right(wallpaper_candidates[candidate].size()-1);
        wallpaperLabel->setText(wallpaper_path.split("/").last());
        wallpaperLabel->setToolTip(wallpaper_path);
        currentWallpaperLabel->setToolTip(wallpaper_path);
    });

    QPushButton * wallpaperSelectorButton = findChild<QPushButton*>("wallpaperSelectorButton");
    QFileDialog * wallpaperDialog = new QFileDialog(this,"Rocket Wallpaper Selection",QDir::homePath(),"Images (*.jpg *.jpeg *.png)");
    connect(wallpaperSelectorButton,&QPushButton::pressed,wallpaperDialog,[=](){
        wallpaperDialog->setFileMode(QFileDialog::FileMode::ExistingFile);
        if(wallpaperDialog->exec())
        {
            QString file = wallpaperDialog->selectedFiles()[0];
            qDebug() << file;
            if (QFile::exists(QDir::homePath()+"/.config/rocket/wallpaper.jpeg"))
                QFile::remove(QDir::homePath()+"/.config/rocket/wallpaper.jpeg");
            QFile::copy(file,QDir::homePath()+"/.config/rocket/wallpaper.jpeg");
        }
    });

    /*
     * Base Colour Box
     * */
    m_basecolour = ConfigManager.getBaseColour();

    m_baseLabel_r = findChild<QLabel*>("baseColourLabel_r");
    m_baseLabel_r->setText(QString::number(m_basecolour.red()));
    m_baseLabel_g = findChild<QLabel*>("baseColourLabel_g");
    m_baseLabel_g->setText(QString::number(m_basecolour.green()));
    m_baseLabel_b = findChild<QLabel*>("baseColourLabel_b");
    m_baseLabel_b->setText(QString::number(m_basecolour.blue()));
    m_baseLabel_a = findChild<QLabel*>("baseColourLabel_a");
    m_baseLabel_a->setText(QString::number(m_basecolour.alpha()));

    m_baseColourPanel = findChild<QWidget*>("baseColourPanel");
    m_baseColourPanel->setAutoFillBackground(true);
    m_baseColourPanel->setPalette(QPalette(QColor(m_basecolour)));

    QPushButton * baseColourPickerButton = findChild<QPushButton*>("baseColourPickerButton");
    QColorDialog * baseColourDialog = new QColorDialog(nullptr);
    connect(baseColourPickerButton,&QPushButton::clicked,baseColourDialog,[=](){
        QColor newcolor = baseColourDialog->getColor(QColor(m_baseLabel_r->text().toInt(),m_baseLabel_g->text().toInt(),m_baseLabel_b->text().toInt(),m_baseLabel_a->text().toInt()),nullptr,"Base Color",QColorDialog::ColorDialogOption::ShowAlphaChannel);
        if (newcolor.isValid())
        {
            m_baseColourPanel->setPalette(QPalette(newcolor));
            m_baseLabel_r->setText(QString::number(newcolor.red()));
            m_baseLabel_g->setText(QString::number(newcolor.green()));
            m_baseLabel_b->setText(QString::number(newcolor.blue()));
            m_baseLabel_a->setText(QString::number(newcolor.alpha()));
            m_basecolour = newcolor;
        }
    });

    /*
     * Secondary Colour Box
     * */
    m_secondarycolour = ConfigManager.getSecondaryColour();

    m_secondaryLabel_r = findChild<QLabel*>("secondaryColourLabel_r");
    m_secondaryLabel_r->setText(QString::number(m_secondarycolour.red()));
    m_secondaryLabel_g = findChild<QLabel*>("secondaryColourLabel_g");
    m_secondaryLabel_g->setText(QString::number(m_secondarycolour.green()));
    m_secondaryLabel_b = findChild<QLabel*>("secondaryColourLabel_b");
    m_secondaryLabel_b->setText(QString::number(m_secondarycolour.blue()));
    m_secondaryLabel_a = findChild<QLabel*>("secondaryColourLabel_a");
    m_secondaryLabel_a->setText(QString::number(m_secondarycolour.alpha()));

    m_secondaryColourPanel = findChild<QWidget*>("secondaryColourPanel");
    m_secondaryColourPanel->setAutoFillBackground(true);
    m_secondaryColourPanel->setPalette(QPalette(QColor(m_secondarycolour)));

    QPushButton * secondaryColourPickerButton = findChild<QPushButton*>("secondaryColourPickerButton");
    QColorDialog * secondaryColourDialog = new QColorDialog(nullptr);
    connect(secondaryColourPickerButton,&QPushButton::clicked,secondaryColourDialog,[=](){
        QColor newcolor = secondaryColourDialog->getColor(QColor(m_secondaryLabel_r->text().toInt(),m_secondaryLabel_g->text().toInt(),m_secondaryLabel_b->text().toInt(),m_secondaryLabel_a->text().toInt()),nullptr,"Secondary Color",QColorDialog::ColorDialogOption::ShowAlphaChannel);
        if (newcolor.isValid())
        {
            m_secondaryColourPanel->setPalette(QPalette(newcolor));
            m_secondaryLabel_r->setText(QString::number(newcolor.red()));
            m_secondaryLabel_g->setText(QString::number(newcolor.green()));
            m_secondaryLabel_b->setText(QString::number(newcolor.blue()));
            m_secondaryLabel_a->setText(QString::number(newcolor.alpha()));
            m_secondarycolour = newcolor;
        }
    });

    /*
     * Font Size Box
     * */

    m_font1Spin = findChild<QSpinBox*>("font1Spin");
    m_font1Spin->setValue(ConfigManager.getFontSize1());

    m_font2Spin = findChild<QSpinBox*>("font2Spin");
    m_font2Spin->setValue(ConfigManager.getFontSize2());

    QDialogButtonBox * buttonBox = findChild<QDialogButtonBox*>("buttonBox");
    connect(buttonBox->button(QDialogButtonBox::Ok),&QPushButton::clicked,this,&MainWindow::OkClicked);
    connect(buttonBox->button(QDialogButtonBox::Reset),&QPushButton::clicked,this,&MainWindow::ResetClicked);
    connect(buttonBox->button(QDialogButtonBox::Apply),&QPushButton::clicked,this,&MainWindow::ApplyClicked);
    connect(buttonBox->button(QDialogButtonBox::Cancel),&QPushButton::clicked,this,&MainWindow::CancelClicked);
}

void MainWindow::OkClicked()
{
    ApplyClicked();
    CancelClicked();
}

void MainWindow::ResetClicked()
{
    m_rocketMetaCheckBox->setChecked(RocketStyle::kwin_start_meta);

    m_rowSpin->setValue(RocketStyle::m_rows);
    m_columnSpin->setValue(RocketStyle::m_cols);
    m_horizontalInversionCheckBox->setChecked(RocketStyle::inverted_scrolling_x);
    m_verticalInversionCheckBox->setChecked(RocketStyle::inverted_scrolling_y);
    m_gridOrientationCombo->setCurrentIndex((int)(RocketStyle::pager_vertical_orientation));
    m_enableBoxesCheckBox->setChecked(RocketStyle::enable_boxes);
    m_systemWallpaperCheckBox->setChecked(RocketStyle::use_system_wallpaper);
    m_systemWallpaperScreenSpin->setValue(RocketStyle::use_system_wallpaper_screen);
    m_systemWallpaperBlurSpin->setValue(RocketStyle::blurradius);

    m_basecolour = RocketStyle::BaseColour;
    m_baseColourPanel->setPalette(QPalette(m_basecolour));
    m_baseLabel_r->setText(QString::number(m_basecolour.red()));
    m_baseLabel_g->setText(QString::number(m_basecolour.green()));
    m_baseLabel_b->setText(QString::number(m_basecolour.blue()));
    m_baseLabel_a->setText(QString::number(m_basecolour.alpha()));

    m_secondarycolour = RocketStyle::SecondaryColour;
    m_secondaryColourPanel->setPalette(QPalette(m_secondarycolour));
    m_secondaryLabel_r->setText(QString::number(m_secondarycolour.red()));
    m_secondaryLabel_g->setText(QString::number(m_secondarycolour.green()));
    m_secondaryLabel_b->setText(QString::number(m_secondarycolour.blue()));
    m_secondaryLabel_a->setText(QString::number(m_secondarycolour.alpha()));

    m_font1Spin->setValue(RocketStyle::fontsize1);
    m_font2Spin->setValue(RocketStyle::fontsize2);
}

void MainWindow::ApplyClicked()
{
    KConfig * config = ConfigManager.getStyleConfig();

    // Rocket Meta Setting
    KConfig kwin_config(QDir::homePath()+"/.config/kwinrc");
    QString kwin_value = kwin_config.group("ModifierOnlyShortcuts").readEntry("Meta");
    QString config_value = QString(ConfigManager.getStyleValue(RocketConfig::Settings::group,RocketConfig::Settings::kwinrcoldmeta));
    if (kwin_value!=config_value && kwin_value!=RocketStyle::kwindbus)
    {
        // backup
        config->group(RocketConfig::Settings::group).writeEntry(RocketConfig::Settings::kwinrcoldmeta,kwin_value);
    }
    if (m_rocketMetaCheckBox->isChecked())
    {
        kwin_config.group("ModifierOnlyShortcuts").writeEntry("Meta",RocketStyle::kwindbus);
    }
    else
    {
        if (kwin_value==RocketStyle::kwindbus)
        {
            if (config_value=="")
            {
                kwin_config.group("ModifierOnlyShortcuts").deleteEntry("Meta");
            }
            else {
               kwin_config.group("ModifierOnlyShortcuts").writeEntry("Meta",config_value);
            }
        }
    }
    if ((m_rocketMetaCheckBox->isChecked()) != (ConfigManager.getStyleValue(RocketConfig::Settings::group,RocketConfig::Settings::metastart,RocketStyle::kwin_start_meta)))
    {
        QProcess p;
        p.start("qdbus org.kde.KWin /KWin reconfigure");
        p.waitForFinished(1000);
    }
    config->group(RocketConfig::Settings::group).writeEntry(RocketConfig::Settings::metastart,m_rocketMetaCheckBox->isChecked());

    // Grid Properties Box
    config->group(RocketConfig::Dimensions::group).writeEntry(RocketConfig::Dimensions::rows,m_rowSpin->value());
    config->group(RocketConfig::Dimensions::group).writeEntry(RocketConfig::Dimensions::columns,m_columnSpin->value());
    config->group(RocketConfig::Settings::group).writeEntry(RocketConfig::Settings::invertedscrollingx,m_horizontalInversionCheckBox->isChecked());
    config->group(RocketConfig::Settings::group).writeEntry(RocketConfig::Settings::invertedscrollingy,m_verticalInversionCheckBox->isChecked());
    config->group(RocketConfig::Dimensions::group).writeEntry(RocketConfig::Dimensions::verticalorientation,(bool)(m_gridOrientationCombo->currentIndex()));
    config->group(RocketConfig::Settings::group).writeEntry(RocketConfig::Settings::enable_boxes,m_enableBoxesCheckBox->isChecked());
    config->group(RocketConfig::Background::group).writeEntry(RocketConfig::Background::usesystemwallpaper,m_systemWallpaperCheckBox->isChecked());
    config->group(RocketConfig::Background::group).writeEntry(RocketConfig::Background::wallpaperofscreen,m_systemWallpaperScreenSpin->value());
    config->group(RocketConfig::Background::group).writeEntry(RocketConfig::Background::blurradius,m_systemWallpaperBlurSpin->value());

    // Colours
    config->group(RocketConfig::Color::group).writeEntry(RocketConfig::Color::base,m_basecolour);
    config->group(RocketConfig::Color::group).writeEntry(RocketConfig::Color::secondary,m_secondarycolour);

    // Font
    config->group(RocketConfig::Font::group).writeEntry(RocketConfig::Font::size1,m_font1Spin->value());
    config->group(RocketConfig::Font::group).writeEntry(RocketConfig::Font::size2,m_font2Spin->value());

    config->sync();
}

void MainWindow::CancelClicked()
{
    qApp->exit();
}


MainWindow::~MainWindow()
{
    delete ui;
}
