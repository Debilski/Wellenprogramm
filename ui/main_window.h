/*****************************************************************************
 * RDS Lattice Library / ‘Wellenprogramm’
 * Copyright (C) 2008–2009  Rike-Benjamin Schuppner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Public License, Version 2.0
 *****************************************************************************/
#ifndef MAIN_H
#define MAIN_H

#include "ui_main_window.h"

class Waveprogram2DPlot;

class MainWindow : public QMainWindow, private Ui::settingsDLG
{
    Q_OBJECT
public:
    MainWindow(QMainWindow* parent = 0);
    //void closeEvent();
public slots:
    void getPath();
    void openPlot2D();
    void openPlot2D(int realSize, int latticeSize);
    void openPlot2D(QAction * act);

    void about();
 signals:
    void window_closed();

protected:
    void closeEvent ( QCloseEvent* event );
    QMenu* lattice_menu;
    QMenu* chain_menu;
private:
    void readSettings();
    void writeSettings();

    Waveprogram2DPlot* plotWindow_;
};


#endif
