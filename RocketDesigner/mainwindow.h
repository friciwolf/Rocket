#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "pager/pager.h"

#include <QMainWindow>
#include <QSpinBox>
#include <QCheckBox>
#include <QComboBox>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void OkClicked();
    void ResetClicked();
    void ApplyClicked();
    void CancelClicked();

private:
    Ui::MainWindow *ui;

    // Rocket Meta Setting
    QCheckBox * m_rocketMetaCheckBox;

    // Grid Properties Box
    QSpinBox * m_rowSpin;
    QSpinBox * m_columnSpin;
    QCheckBox * m_horizontalInversionCheckBox;
    QCheckBox * m_verticalInversionCheckBox;
    QComboBox * m_gridOrientationCombo;
    QCheckBox * m_systemWallpaperCheckBox;
    QSpinBox * m_systemWallpaperScreenSpin;

    // Colours
    QColor m_basecolour;
    QLabel * m_baseLabel_r;
    QLabel * m_baseLabel_g;
    QLabel * m_baseLabel_b;
    QLabel * m_baseLabel_a;
    QWidget * m_baseColourPanel;

    QColor m_secondarycolour;
    QLabel * m_secondaryLabel_r;
    QLabel * m_secondaryLabel_g;
    QLabel * m_secondaryLabel_b;
    QLabel * m_secondaryLabel_a;
    QWidget * m_secondaryColourPanel;

    // Font
    QSpinBox * m_font1Spin;
    QSpinBox * m_font2Spin;
};

#endif // MAINWINDOW_H
