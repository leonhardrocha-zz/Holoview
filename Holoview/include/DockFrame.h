#include "qtLib.h"
#ifndef _DOCK_FRAME_H
#define _DOCK_FRAME_H

#undef DEBUG_SIZEHINTS

class DockFrame : public QFrame
{
    Q_OBJECT
public:
    DockFrame(const QString &name, QWidget *parent);

    virtual QSize sizeHint() const;
    virtual QSize minimumSizeHint() const;

    void setCustomSizeHint(const QSize &size);

public slots:
    void changeSizeHints();

protected:
    QString name;
    QSize szHint, minSzHint;
};
#endif