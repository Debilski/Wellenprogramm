/*
 * detachable_tab_widget.cpp
 *
 *  Created on: 03.04.2009
 *      Author: rikebs
 */

#include "detachable_tab_widget.h"

#include <qtabbar.h>
#include <qpoint.h>
#include <qmenu.h>

class DetachableTabWidget::PrivateData {
public:
    QMenu menu;
};

DetachableTabWidget::DetachableTabWidget(QWidget* parent) :
    QTabWidget( parent )
{
    tabBar()->setContextMenuPolicy( Qt::CustomContextMenu );

    d_data = new PrivateData;
    QAction* detach = new QAction( tr( "Detach" ), this );
    detach->setEnabled( false );
    d_data->menu.addAction( detach );

    connect(
        tabBar(), SIGNAL( customContextMenuRequested(const QPoint&) ), this,
        SLOT( showTabMenu (const QPoint&)) );
}

DetachableTabWidget::~DetachableTabWidget()
{
    delete d_data;
}

void DetachableTabWidget::showTabMenu(const QPoint& p)
{
    // irgendwie mit TabAt und so weiter einbauen.
    d_data->menu.popup( tabBar()->mapToGlobal( p ) );
}
