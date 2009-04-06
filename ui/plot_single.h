/*
 * plot_single.h
 *
 *  Created on: 08.03.2009
 *      Author: rikebs
 */

#ifndef PLOT_SINGLE_H_
#define PLOT_SINGLE_H_

#include <qdialog.h>

#include "ui_plot_single.h"

class LatticeInterface;

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

    class PrivateData;
    PrivateData* d_data;
};

#endif /* PLOT_SINGLE_H_ */
