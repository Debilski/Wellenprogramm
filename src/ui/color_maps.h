/*****************************************************************************
 * RDS Lattice Library / ‘Wellenprogramm’
 * Copyright (C) 2008–2009  Rike-Benjamin Schuppner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Public License, Version 2.0
 *****************************************************************************/
/*
 * color_maps.h
 *
 *  Created on: 02.04.2009
 *      Author: rikebs
 */

#ifndef COLOUR_MAPS_H_
#define COLOUR_MAPS_H_

#include <QtCore>

#include "qwt_color_map.h"

class LinearTransparentColorMap : public QwtColorMap
{
public:
    /*!
     Mode of color map
     \sa setMode(), mode()
     */
    enum Mode
    {
        FixedColors,
        ScaledColors
    };

    LinearTransparentColorMap(QwtColorMap::Format = QwtColorMap::RGB);
    LinearTransparentColorMap(const QColor& from, const QColor& to, QwtColorMap::Format = QwtColorMap::RGB);

    LinearTransparentColorMap(const LinearTransparentColorMap&);

    virtual ~LinearTransparentColorMap();

    LinearTransparentColorMap& operator=(const LinearTransparentColorMap&);

    virtual QwtColorMap* copy() const;

    void setMode(Mode);
    Mode mode() const;

    void setColorInterval(const QColor& color1, const QColor& color2);
    void addColorStop(double value, const QColor&);
    QwtArray<double> colorStops() const;

    QColor color1() const;
    QColor color2() const;

    virtual QRgb rgb(const QwtDoubleInterval&, double value) const;
    virtual unsigned char colorIndex(const QwtDoubleInterval&, double value) const;

    class ColorStops;

private:
    class PrivateData;
    PrivateData* d_data;
};

class ColorMaps
{
public:
    enum ColorMapTypes
    {
        standardColorMap,
        greyColorMap,
        jetColorMap,
        halfJetColorMap,
        autumnColorMap,
        hotColorMap
    };

    ColorMapTypes colorMapType;

    typedef QPair<ColorMaps::ColorMapTypes, QString> T_identifier;
    const QList<QPair<ColorMapTypes, QString> >& colorMapNames() const
    {
        return colorMapNames_;
    }
    const QwtColorMap& getColorMap() const
    {
        return colorMap;
    }
    const QwtColorMap& getColorMap(ColorMapTypes type)
    {
        colorMapType = type;
        switch (colorMapType) {
            case greyColorMap:
                colorMap = QwtLinearColorMap(Qt::black, Qt::white);
                break;
            case jetColorMap:
                double pos;
                colorMap = QwtLinearColorMap(QColor(0, 0, 189), QColor(132, 0, 0));
                pos = 1.0 / 13.0 * 1.0;
                colorMap.addColorStop(pos, QColor(0, 0, 255));
                pos = 1.0 / 13.0 * 2.0;
                colorMap.addColorStop(pos, QColor(0, 66, 255));
                pos = 1.0 / 13.0 * 3.0;
                colorMap.addColorStop(pos, QColor(0, 132, 255));
                pos = 1.0 / 13.0 * 4.0;
                colorMap.addColorStop(pos, QColor(0, 189, 255));
                pos = 1.0 / 13.0 * 5.0;
                colorMap.addColorStop(pos, QColor(0, 255, 255));
                pos = 1.0 / 13.0 * 6.0;
                colorMap.addColorStop(pos, QColor(66, 255, 189));
                pos = 1.0 / 13.0 * 7.0;
                colorMap.addColorStop(pos, QColor(132, 255, 132));
                pos = 1.0 / 13.0 * 8.0;
                colorMap.addColorStop(pos, QColor(189, 255, 66));
                pos = 1.0 / 13.0 * 9.0;
                colorMap.addColorStop(pos, QColor(255, 255, 0));
                pos = 1.0 / 13.0 * 10.0;
                colorMap.addColorStop(pos, QColor(255, 189, 0));
                pos = 1.0 / 13.0 * 12.0;
                colorMap.addColorStop(pos, QColor(255, 66, 0));
                pos = 1.0 / 13.0 * 13.0;
                colorMap.addColorStop(pos, QColor(189, 0, 0));
                break;
            case halfJetColorMap:
                colorMap = QwtLinearColorMap(QColor(0, 0, 189), QColor(255, 255, 0));
                pos = 1.0 / 9.0 * 1.0;
                colorMap.addColorStop(pos, QColor(0, 0, 255));
                pos = 1.0 / 9.0 * 2.0;
                colorMap.addColorStop(pos, QColor(0, 66, 255));
                pos = 1.0 / 9.0 * 3.0;
                colorMap.addColorStop(pos, QColor(0, 132, 255));
                pos = 1.0 / 9.0 * 4.0;
                colorMap.addColorStop(pos, QColor(0, 189, 255));
                pos = 1.0 / 9.0 * 5.0;
                colorMap.addColorStop(pos, QColor(0, 255, 255));
                pos = 1.0 / 9.0 * 6.0;
                colorMap.addColorStop(pos, QColor(66, 255, 189));
                pos = 1.0 / 9.0 * 7.0;
                colorMap.addColorStop(pos, QColor(132, 255, 132));
                pos = 1.0 / 9.0 * 8.0;
                colorMap.addColorStop(pos, QColor(189, 255, 66));
                pos = 1.0 / 9.0 * 9.0;
                colorMap.addColorStop(pos, QColor(255, 255, 0));
                break;
            case autumnColorMap: {
                QVector<QColor> autumnColours;
                autumnColours << QColor(255, 0, 0) << QColor(255, 4, 0) << QColor(255, 8, 0)
                              << QColor(255, 12, 0) << QColor(255, 16, 0) << QColor(255, 20, 0)
                              << QColor(255, 24, 0) << QColor(255, 28, 0) << QColor(255, 32, 0)
                              << QColor(255, 36, 0) << QColor(255, 40, 0) << QColor(255, 45, 0)
                              << QColor(255, 49, 0) << QColor(255, 53, 0) << QColor(255, 57, 0)
                              << QColor(255, 61, 0) << QColor(255, 65, 0) << QColor(255, 69, 0)
                              << QColor(255, 73, 0) << QColor(255, 77, 0) << QColor(255, 81, 0)
                              << QColor(255, 85, 0) << QColor(255, 89, 0) << QColor(255, 93, 0)
                              << QColor(255, 97, 0) << QColor(255, 101, 0) << QColor(255, 105, 0)
                              << QColor(255, 109, 0) << QColor(255, 113, 0) << QColor(255, 117, 0)
                              << QColor(255, 121, 0) << QColor(255, 125, 0) << QColor(255, 130, 0)
                              << QColor(255, 134, 0) << QColor(255, 138, 0) << QColor(255, 142, 0)
                              << QColor(255, 146, 0) << QColor(255, 150, 0) << QColor(255, 154, 0)
                              << QColor(255, 158, 0) << QColor(255, 162, 0) << QColor(255, 166, 0)
                              << QColor(255, 170, 0) << QColor(255, 174, 0) << QColor(255, 178, 0)
                              << QColor(255, 182, 0) << QColor(255, 186, 0) << QColor(255, 190, 0)
                              << QColor(255, 194, 0) << QColor(255, 198, 0) << QColor(255, 202, 0)
                              << QColor(255, 206, 0) << QColor(255, 210, 0) << QColor(255, 215, 0)
                              << QColor(255, 219, 0) << QColor(255, 223, 0) << QColor(255, 227, 0)
                              << QColor(255, 231, 0) << QColor(255, 235, 0) << QColor(255, 239, 0)
                              << QColor(255, 243, 0) << QColor(255, 247, 0) << QColor(255, 251, 0)
                              << QColor(255, 255, 0);
                colorMap = QwtLinearColorMap(autumnColours.first(), autumnColours.last());
                autumnColours.pop_front();
                for (int i = 1; i <= autumnColours.size(); ++i) {
                    colorMap.addColorStop(1.0 * i / autumnColours.size(), autumnColours[i - 1]);
                }
            } break;
            case hotColorMap: {
                QVector<QColor> hotColours;
                hotColours << QColor(11, 0, 0) << QColor(21, 0, 0) << QColor(32, 0, 0)
                           << QColor(43, 0, 0) << QColor(53, 0, 0) << QColor(64, 0, 0) << QColor(
                                                                                              74, 0, 0)
                           << QColor(85, 0, 0) << QColor(96, 0, 0) << QColor(106, 0, 0)
                           << QColor(117, 0, 0) << QColor(128, 0, 0) << QColor(138, 0, 0) << QColor(
                                                                                                 149, 0, 0)
                           << QColor(159, 0, 0) << QColor(170, 0, 0) << QColor(181, 0, 0)
                           << QColor(191, 0, 0) << QColor(202, 0, 0) << QColor(213, 0, 0) << QColor(
                                                                                                 223, 0, 0)
                           << QColor(234, 0, 0) << QColor(244, 0, 0) << QColor(255, 0, 0)
                           << QColor(255, 11, 0) << QColor(255, 21, 0) << QColor(255, 32, 0)
                           << QColor(255, 43, 0) << QColor(255, 53, 0) << QColor(255, 64, 0)
                           << QColor(255, 74, 0) << QColor(255, 85, 0) << QColor(255, 96, 0)
                           << QColor(255, 106, 0) << QColor(255, 117, 0) << QColor(255, 128, 0)
                           << QColor(255, 138, 0) << QColor(255, 149, 0) << QColor(255, 159, 0)
                           << QColor(255, 170, 0) << QColor(255, 181, 0) << QColor(255, 191, 0)
                           << QColor(255, 202, 0) << QColor(255, 213, 0) << QColor(255, 223, 0)
                           << QColor(255, 234, 0) << QColor(255, 244, 0) << QColor(255, 255, 0)
                           << QColor(255, 255, 16) << QColor(255, 255, 32) << QColor(255, 255, 48)
                           << QColor(255, 255, 64) << QColor(255, 255, 80) << QColor(255, 255, 96)
                           << QColor(255, 255, 112) << QColor(255, 255, 128) << QColor(255, 255, 143)
                           << QColor(255, 255, 159) << QColor(255, 255, 175) << QColor(255, 255, 191)
                           << QColor(255, 255, 207) << QColor(255, 255, 223) << QColor(255, 255, 239)
                           << QColor(255, 255, 255);

                colorMap = QwtLinearColorMap(hotColours.first(), hotColours.last());
                hotColours.pop_front();
                for (int i = 1; i <= hotColours.size(); ++i) {
                    colorMap.addColorStop(1.0 * i / hotColours.size(), hotColours[i - 1]);
                }
            } break;
            case standardColorMap:
            default:
                colorMap = QwtLinearColorMap(Qt::darkBlue, Qt::darkRed);  // -2.2, 2.5
                colorMap.addColorStop(0.0426, Qt::darkCyan);  // u = -2
                colorMap.addColorStop(0.1277, Qt::cyan);  // u = -1.6
                colorMap.addColorStop(0.5532, Qt::green);  // u = 0.4
                colorMap.addColorStop(0.8085, Qt::yellow);  // u = 1.6
                colorMap.addColorStop(0.8936, Qt::red);  // u = 2
        }
        return colorMap;
    }
    ColorMaps()
    {
        colorMapNames_.append(QPair<ColorMapTypes, QString>(
            standardColorMap, "Standard Color Map"));
        colorMapNames_.append(QPair<ColorMapTypes, QString>(greyColorMap, "Grey Color Map"));
        colorMapNames_.append(QPair<ColorMapTypes, QString>(jetColorMap, "Jet Color Map"));
        colorMapNames_.append(QPair<ColorMapTypes, QString>(
            halfJetColorMap, "Half-Jet Color Map"));
        colorMapNames_.append(
            QPair<ColorMapTypes, QString>(autumnColorMap, "Autumn Color Map"));
        colorMapNames_.append(QPair<ColorMapTypes, QString>(hotColorMap, "Hot Color Map"));
        getColorMap(standardColorMap);
    }
    ~ColorMaps()
    {
    }

private:
    QwtLinearColorMap colorMap;
    QList<QPair<ColorMapTypes, QString> > colorMapNames_;
};

Q_DECLARE_METATYPE(ColorMaps::ColorMapTypes)

#endif /* COLOUR_MAPS_H_ */
