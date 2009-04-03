/*
 * parameter_dock_widget.h
 *
 *  Created on: 03.04.2009
 *      Author: rikebs
 */

#ifndef PARAMETER_DOCK_WIDGET_H_
#define PARAMETER_DOCK_WIDGET_H_

#include <qdockwidget.h>

class ParameterDockWidget : public QDockWidget {
    Q_OBJECT
public:
    ParameterDockWidget(QWidget* parent = 0, Qt::WindowFlags flags = 0);
};

#endif /* PARAMETER_DOCK_WIDGET_H_ */
