#ifndef PAGERFOLDERVIEW_H
#define PAGERFOLDERVIEW_H

#include <QTimer>
#include <QLineEdit>
#include <QMouseEvent>
#include <QPropertyAnimation>

#include "pager/pager.h"
#include "pager/verticalpager.h"

class PagerFolderView : public Pager
{
    Q_OBJECT
public:
    ~PagerFolderView() {}
    explicit PagerFolderView(QWidget *parent, std::vector<KDEApplication> appTree, bool withBackgound = false);
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent * event) override;
    void dragMoveEvent(QDragMoveEvent * event) override;
    void leaveEvent(QEvent * event) override;
    QLineEdit * getNameField() {return m_folderNameField;}
    QTimer * getCloseFolderTimer() {return m_closeFolder;}
    void setWindowAnimation(QParallelAnimationGroup * windowAnimation) {m_windowAnimation = windowAnimation;}
    QParallelAnimationGroup * getWindowAnimation() {return m_windowAnimation;}

signals:
    void leavingPagerFolderView();

public slots:
    void enterIconDraggingMode(bool on, IconGridItemCanvas * canvas = nullptr) override;

private:
    QLineEdit * m_folderNameField;
    std::vector<KDEApplication> m_appTree;
    QTimer * m_closeFolder;
    QParallelAnimationGroup * m_windowAnimation;
};

#endif // PAGERFOLDERVIEW_H
