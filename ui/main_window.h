#ifndef MAIN_H
#define MAIN_H

#include "ui_main_window.h"
#include "plotwindow_2d.h"

#include "tiny_double_edit.h"

class MainWindow : public QMainWindow, private Ui::settingsDLG
{
    Q_OBJECT
public:
    MainWindow(QMainWindow *parent = 0);
    Waveprogram2DPlot *plotWin_2D;
    //void closeEvent();
protected:
    void closeEvent ( QCloseEvent* event );
    QMenu* lattice_menu;
    QMenu* chain_menu;
private:
    void readSettings();
    void writeSettings();

public slots:
    void getPath();
    void openPlot2D();
    void openPlot2D(int realSize, int latticeSize);
    void openPlot2D(QAction * act);

    void about();
 signals:
    void window_closed();
};


#endif
