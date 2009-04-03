/*
 * parameter_dock_widget.cpp
 *
 *  Created on: 03.04.2009
 *      Author: rikebs
 */

#include "parameter_dock_widget.h"

#include <QtGui>

ParameterDockWidget::ParameterDockWidget(QWidget* parent, Qt::WindowFlags flags) :
    QDockWidget(tr("Parameter Dock"),parent, flags)
{
    QWidget* w = new QWidget(this);
    QSpinBox* box = new QSpinBox(w);
    QFormLayout* l = new QFormLayout(w);
    l->addRow(box);
    w->setLayout(l);
    setWidget(w);
}
