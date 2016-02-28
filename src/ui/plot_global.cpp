#include "plot_global.h"

#include <QtCore>

#ifdef Q_WS_MAC
#include <QMacStyle>
#endif

void smallWidgetSizePolicyOnMac(QWidget* w)
{
#ifdef Q_WS_MAC
    QMacStyle::setWidgetSizePolicy(w, QMacStyle::SizeSmall);
#else
    Q_UNUSED(w)
#endif
}

void miniWidgetSizePolicyOnMac(QWidget* w)
{
#ifdef Q_WS_MAC
    QMacStyle::setWidgetSizePolicy(w, QMacStyle::SizeMini);
#else
    Q_UNUSED(w)
#endif
}
