/*****************************************************************************
 * RDS Lattice Library / ‘Wellenprogramm’
 * Copyright (C) 2008–2009  Rike-Benjamin Schuppner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Public License, Version 2.0
 *****************************************************************************/
/*
 * plot_single.cpp
 *
 *  Created on: 08.03.2009
 *      Author: rikebs
 */

#include "plot_single.h"

#include <QtCore>
#include <QtGui>

#include <qwt_plot.h>
#include <qwt_plot_curve.h>

#include "lattice_interface.h"

class PlotSingle::PrivateData
{
public:
    LatticeInterface* lattice;
    QVector<QwtPlotCurve*> curves;
    QVector<QVector<double> > curveData;
    QVector<double> timeArray;

    double timeShown;
    double minTimeIncrease;
};

PlotSingle::PlotSingle(LatticeInterface* lattice, QWidget* parent) : QDialog(parent)
{
    d_data = new PrivateData;

    d_data->lattice = lattice;
    d_data->timeShown = 10.0;

    setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle("Lattice Element");

    updateIntervallSpinBox->setValue(0.05);
    updateIntervallSpinBox->setDecimals(3);

    readSettings();

    actionCopy_to_Clipboard->setShortcut(QKeySequence::Copy);

    if (xSelector->value() >= d_data->lattice->sizeX())
        xSelector->setValue(d_data->lattice->sizeX());

    if (ySelector->value() >= d_data->lattice->sizeY())
        ySelector->setValue(d_data->lattice->sizeY());

    xSelector->setMaximum(d_data->lattice->sizeX());
    ySelector->setMaximum(d_data->lattice->sizeY());

    connect(clearButton, SIGNAL(clicked()), this, SLOT(clearData()));

    for (uint i = 0; i < d_data->lattice->numberOfVariables(); ++i) {
        QwtPlotCurve* c = new QwtPlotCurve();
        QBrush brush(Qt::blue);
        QPen pen(brush, 2, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin);
        c->setPen(pen);
        QVector<double> d;

        c->setData(d_data->timeArray, d);
        c->attach(singlePlotView);
        d_data->curveData << d;
        d_data->curves << c;
    }
}

PlotSingle::~PlotSingle()
{
    qDebug() << "delete";
    foreach (QwtPlotCurve* c, d_data->curves) {
        delete c;
    }
    delete d_data;
}

void PlotSingle::closeEvent(QCloseEvent* event)
{
    qDebug() << "close";
    d_data->lattice = 0;
    writeSettings();
    event->accept();
}

/**
 * Liest neue Werte aus der Lattice und schreibt den Plot neu.
 */
void PlotSingle::update()
{
    // Sollte eigentlich nicht notwendigsein. Als Workaround jedoch noch drin.
    if (d_data->lattice == 0)
        return;

    if (d_data->timeArray.size() != 0 && d_data->lattice->time() - d_data->timeArray.last() <= updateIntervallSpinBox->value())
        return;

    int x = xSelector->value();
    int y = ySelector->value();

    d_data->timeArray << d_data->lattice->time();

    for (uint component = 0; component < d_data->lattice->numberOfVariables(); ++component) {
        double val = d_data->lattice->getComponentAt(component, x, y);
        d_data->curveData[component] << val;

        d_data->curves[component]->setData(d_data->timeArray, d_data->curveData[component]);
        d_data->curves[component]->attach(singlePlotView);
    }
    singlePlotView->replot();
}

void PlotSingle::clearData()
{
    d_data->timeArray.clear();
    for (uint component = 0; component < d_data->lattice->numberOfVariables(); ++component) {
        d_data->curveData[component].clear();
    }
}

/**
 * Schreibt den aktuellen Plot in einem Matlab-lesbaren Format in die Zwischenablage.
 */
void PlotSingle::on_actionCopy_to_Clipboard_triggered()
{
    QClipboard* clipboard = QApplication::clipboard();
    QString text;
    text += QString("[");

    for (uint c = 0; c < d_data->lattice->numberOfVariables(); ++c) {
        for (int i = 0; i < d_data->curveData[c].size() - 1; ++i) {
            text += QString("%1").arg(d_data->curveData[c][i], 0, 'f', 4);
            text += QString(", ");
        }
        text += QString("%1").arg(d_data->curveData[c].last(), 0, 'f', 4);
        text += QString("; ");
    }
    for (int i = 0; i < d_data->timeArray.size() - 1; ++i) {
        text += QString("%1").arg(d_data->timeArray[i], 0, 'f', 4);
        text += QString(", ");
    }
    text += QString("%1").arg(d_data->timeArray.last(), 0, 'f', 4);

    text += QString("]");
    clipboard->setText(text);
}

void PlotSingle::readSettings()
{
    QSettings settings;
    settings.beginGroup("PlotSingle");
    restoreGeometry(settings.value("geometry").toByteArray());
    xSelector->setValue(settings.value("x", 0).toInt());
    ySelector->setValue(settings.value("y", 0).toInt());

    settings.endGroup();
}

void PlotSingle::writeSettings()
{
    QSettings settings;
    settings.beginGroup("PlotSingle");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("x", xSelector->value());
    settings.setValue("y", ySelector->value());

    settings.endGroup();
}
