/*
 * plot_single.h
 *
 *  Created on: 08.03.2009
 *      Author: rikebs
 */

#ifndef PLOT_SINGLE_H_
#define PLOT_SINGLE_H_

#include <QtCore>
#include <QtGui>

#include <qwt_plot.h>
#include <qwt_plot_curve.h>

#include "lattice_interface.h"

#include "ui_plot_single.h"

// Label hinzufügen

class PlotSingle : public QDialog, private Ui::plotSingle {
Q_OBJECT
public:
    PlotSingle( LatticeInterface* lattice, QWidget* parent = 0 );
    ~PlotSingle();
public slots:
    void update();
    void clearData();
    void on_actionCopy_to_Clipboard_triggered();

protected:
    void closeEvent ( QCloseEvent* event );

private:
    void readSettings();
    void writeSettings();

    LatticeInterface* lattice_;
    QVector< QwtPlotCurve* > curves_;
    QVector< QVector< double > > data_;
    QVector< double > timeArray_;

    double timeShown_;
    double minTimeIncrease_;

};

#endif /* PLOT_SINGLE_H_ */
