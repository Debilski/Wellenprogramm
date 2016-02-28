/*****************************************************************************
 * RDS Lattice Library / ‘Wellenprogramm’
 * Copyright (C) 2008–2009  Rike-Benjamin Schuppner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Public License, Version 2.0
 *****************************************************************************/
/*
 * spectrogram_data.h
 *
 *  Created on: 06.03.2009
 *      Author: rikebs
 */

#ifndef SPECTROGRAM_DATA_H_
#define SPECTROGRAM_DATA_H_

#include "lattice_controller.h"
#include "plot_view.h"

class SpectrogramData : public QwtRasterData
{
    LatticeController* latticeController_;
    int variable_;
    PlotLayer* plotStackLayer_;

public:
    SpectrogramData(LatticeController* latticeController, int variable = 0,
        PlotLayer* plotStackLayer = 0) : QwtRasterData(QwtDoubleRect(0., 0., (latticeController->sizeX()), (latticeController->sizeY()))), latticeController_(latticeController), variable_(variable), plotStackLayer_(plotStackLayer)
    {
    }
    ~SpectrogramData()
    {
    }
    virtual QSize rasterHint(const QwtDoubleRect&) const
    {
        return QSize(latticeController_->latticeSizeX(), latticeController_->latticeSizeY());
    }

    virtual SpectrogramData* copy() const
    {
        return new SpectrogramData(latticeController_, variable_, plotStackLayer_);
    }

    virtual QwtDoubleInterval range() const
    {
        return plotStackLayer_->range();
    }

    virtual double value(double x, double y) const
    {
        int ix = qRound(x / latticeController_->lattice()->scaleX());
        int iy = qRound(y / latticeController_->lattice()->scaleY());
        double val = latticeController_->lattice()->getComponentAt(variable_, ix, iy - 1);
        if (val == -1.0)
            return -100;
        return val;
    }
};


class SpectrogramDataFft : public QwtRasterData
{
    LatticeController* latticeController_;
    int variable_;
    Waveprogram2DPlot* programBase;

public:
    SpectrogramDataFft(LatticeController* latticeController, int variable = 0,
        Waveprogram2DPlot* programBase = 0) : QwtRasterData(QwtDoubleRect(0., 0., (latticeController->sizeX()), (latticeController->sizeY()))), latticeController_(latticeController), variable_(variable), programBase(programBase)
    {
    }
    ~SpectrogramDataFft(){};
    // Seltsamer Fehler in Berechnung. Lasse Fkt aus
    virtual QSize rasterHint(const QwtDoubleRect&) const
    {
        return QSize(latticeController_->latticeSizeX(), latticeController_->latticeSizeY());
    }

    virtual QwtRasterData* copy() const
    {
        return new SpectrogramDataFft(latticeController_, variable_, programBase);
    }

    virtual QwtDoubleInterval range() const
    {

        /*double min = lattice_->componentInfos.at( variable_ ).assumedMin;
         double max = lattice_->componentInfos.at( variable_ ).assumedMax;
         max = lattice_->getMax( variable_ );
         //min = lattice_->getMin( variable_ );*/
        //todo: ranges für fft
        //return programBase->plotRanges_real[ variable_ ];
        //return QwtDoubleInterval( min, max );
        return QwtDoubleInterval();
    }

    virtual double value(double x, double y) const
    {
        int ix = qRound(x / latticeController_->lattice()->scaleX());  // % (lattice_->sizeX() / 2);
        int iy = qRound(y / latticeController_->lattice()->scaleY());


        iy = (iy + latticeController_->latticeSizeY() / 2) % latticeController_->latticeSizeY();
        iy = (iy < latticeController_->latticeSizeY() / 2) ? iy : latticeController_->latticeSizeY() - iy;
        ix = (ix + latticeController_->latticeSizeX() / 2) % latticeController_->latticeSizeX();


        return std::abs(latticeController_->lattice()->getFftComponentAt(variable_, ix, iy));  // sqrt( lattice_->latticeSize() );
    }
};


#endif /* SPECTROGRAM_DATA_H_ */
