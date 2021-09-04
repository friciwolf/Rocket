#ifndef PAGERFOLDERVIEW_H
#define PAGERFOLDERVIEW_H

#include <QLineEdit>

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
signals:
    void leavingPagerFolderView();

public slots:

private:
    QLineEdit * m_folderNameField;
    std::vector<KDEApplication> m_appTree;
};

#endif // PAGERFOLDERVIEW_H
