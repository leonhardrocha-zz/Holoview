#include "qtLib.h"
#include "DockFrame.h"

QColor bgColorForName(const QString &name)
{
    if (name == "Black")
        return QColor("#D8D8D8");
    else if (name == "White")
        return QColor("#F1F1F1");
    else if (name == "Red")
        return QColor("#F1D8D8");
    else if (name == "Green")
        return QColor("#D8E4D8");
    else if (name == "Blue")
        return QColor("#D8D8F1");
    else if (name == "Yellow")
        return QColor("#F1F0D8");
    return QColor(name).light(110);
}

QColor fgColorForName(const QString &name)
{
    if (name == "Black")
        return QColor("#6C6C6C");
    else if (name == "White")
        return QColor("#F8F8F8");
    else if (name == "Red")
        return QColor("#F86C6C");
    else if (name == "Green")
        return QColor("#6CB26C");
    else if (name == "Blue")
        return QColor("#6C6CF8");
    else if (name == "Yellow")
        return QColor("#F8F76C");
    return QColor(name);
}


DockFrame::DockFrame(const QString &c, QWidget *parent)
    : QFrame(parent) , color(c)
{
    QFont font = this->font();
    font.setPointSize(8);
    setFont(font);
    szHint = QSize(-1, -1);
    minSzHint = QSize(125, 75);
}

QSize DockFrame::sizeHint() const
{
    return szHint;
}

QSize DockFrame::minimumSizeHint() const
{
    return minSzHint;
}

void DockFrame::paintEvent(QPaintEvent *e)
{

#ifdef DEBUG_SIZEHINTS

	QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.fillRect(rect(), bgColorForName(color));

    p.save();

    extern void render_qt_text(QPainter *, int, int, const QColor &);
    render_qt_text(&p, width(), height(), fgColorForName(color));

    p.restore();

    p.setRenderHint(QPainter::Antialiasing, false);

    QSize sz = size();
    QSize szHint = sizeHint();
    QSize minSzHint = minimumSizeHint();
    QSize maxSz = maximumSize();
    QString text = QString::fromLatin1("sz: %1x%2\nszHint: %3x%4\nminSzHint: %5x%6\n"
                                        "maxSz: %8x%9")
                    .arg(sz.width()).arg(sz.height())
                    .arg(szHint.width()).arg(szHint.height())
                    .arg(minSzHint.width()).arg(minSzHint.height())
                    .arg(maxSz.width()).arg(maxSz.height());

    QRect r = fontMetrics().boundingRect(rect(), Qt::AlignLeft|Qt::AlignTop, text);
    r.adjust(-2, -2, 1, 1);
    p.translate(4, 4);
    QColor bg = Qt::yellow;
    bg.setAlpha(120);
    p.setBrush(bg);
    p.setPen(Qt::black);
    p.drawRect(r);
    p.drawText(rect(), Qt::AlignLeft|Qt::AlignTop, text);
#endif // DEBUG_SIZEHINTS
}

static QSpinBox *createSpinBox(int value, QWidget *parent, int max = 1000)
{
    QSpinBox *result = new QSpinBox(parent);
    result->setMinimum(-1);
    result->setMaximum(max);
    result->setValue(value);
    return result;
}

void DockFrame::changeSizeHints()
{
    QDialog dialog(this);
    dialog.setWindowTitle(color);

    QVBoxLayout *topLayout = new QVBoxLayout(&dialog);

    QGridLayout *inputLayout = new QGridLayout();
    topLayout->addLayout(inputLayout);

    inputLayout->addWidget(new QLabel(tr("Size Hint:"), &dialog), 0, 0);
    inputLayout->addWidget(new QLabel(tr("Min Size Hint:"), &dialog), 1, 0);
    inputLayout->addWidget(new QLabel(tr("Max Size:"), &dialog), 2, 0);
    inputLayout->addWidget(new QLabel(tr("Dockwgt Max Size:"), &dialog), 3, 0);

    QSpinBox *szHintW = createSpinBox(szHint.width(), &dialog);
    inputLayout->addWidget(szHintW, 0, 1);
    QSpinBox *szHintH = createSpinBox(szHint.height(), &dialog);
    inputLayout->addWidget(szHintH, 0, 2);

    QSpinBox *minSzHintW = createSpinBox(minSzHint.width(), &dialog);
    inputLayout->addWidget(minSzHintW, 1, 1);
    QSpinBox *minSzHintH = createSpinBox(minSzHint.height(), &dialog);
    inputLayout->addWidget(minSzHintH, 1, 2);

    QSize maxSz = maximumSize();
    QSpinBox *maxSzW = createSpinBox(maxSz.width(), &dialog, QWIDGETSIZE_MAX);
    inputLayout->addWidget(maxSzW, 2, 1);
    QSpinBox *maxSzH = createSpinBox(maxSz.height(), &dialog, QWIDGETSIZE_MAX);
    inputLayout->addWidget(maxSzH, 2, 2);

    QSize dwMaxSz = parentWidget()->maximumSize();
    QSpinBox *dwMaxSzW = createSpinBox(dwMaxSz.width(), &dialog, QWIDGETSIZE_MAX);
    inputLayout->addWidget(dwMaxSzW, 3, 1);
    QSpinBox *dwMaxSzH = createSpinBox(dwMaxSz.height(), &dialog, QWIDGETSIZE_MAX);
    inputLayout->addWidget(dwMaxSzH, 3, 2);

    inputLayout->setColumnStretch(1, 1);
    inputLayout->setColumnStretch(2, 1);

    topLayout->addStretch();

    QHBoxLayout *buttonBox = new QHBoxLayout();
    topLayout->addLayout(buttonBox);

    QPushButton *okButton = new QPushButton(tr("Ok"), &dialog);
    QPushButton *cancelButton = new QPushButton(tr("Cancel"), &dialog);
    connect(okButton, SIGNAL(clicked()), &dialog, SLOT(accept()));
    connect(cancelButton, SIGNAL(clicked()), &dialog, SLOT(reject()));
    buttonBox->addStretch();
    buttonBox->addWidget(cancelButton);
    buttonBox->addWidget(okButton);


    if (!dialog.exec())
        return;

    szHint = QSize(szHintW->value(), szHintH->value());
    minSzHint = QSize(minSzHintW->value(), minSzHintH->value());
    maxSz = QSize(maxSzW->value(), maxSzH->value());
    setMaximumSize(maxSz);
    dwMaxSz = QSize(dwMaxSzW->value(), dwMaxSzH->value());
    parentWidget()->setMaximumSize(dwMaxSz);
    updateGeometry();
    update();
}

void DockFrame::setCustomSizeHint(const QSize &size)
{
    szHint = size;
    updateGeometry();
}