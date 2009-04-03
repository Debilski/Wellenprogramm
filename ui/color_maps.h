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


class LinearTransparentColorMap: public QwtColorMap
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
    LinearTransparentColorMap( const QColor &from, const QColor &to,
        QwtColorMap::Format = QwtColorMap::RGB);

    LinearTransparentColorMap(const LinearTransparentColorMap &);

    virtual ~LinearTransparentColorMap();

    LinearTransparentColorMap &operator=(const LinearTransparentColorMap &);

    virtual QwtColorMap *copy() const;

    void setMode(Mode);
    Mode mode() const;

    void setColorInterval(const QColor &color1, const QColor &color2);
    void addColorStop(double value, const QColor&);
    QwtArray<double> colorStops() const;

    QColor color1() const;
    QColor color2() const;

    virtual QRgb rgb(const QwtDoubleInterval &, double value) const;
    virtual unsigned char colorIndex(
        const QwtDoubleInterval &, double value) const;

    class ColorStops;

private:
    class PrivateData;
    PrivateData *d_data;
};


class ColorMaps {
public:
    enum ColorMapTypes {
        standardColorMap, greyColorMap, jetColorMap
    };

    ColorMapTypes colorMapType;

    typedef QPair< ColorMaps::ColorMapTypes, QString > T_identifier;
    const QList< QPair< ColorMapTypes, QString > >& colorMapNames() const {
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
                colorMap = QwtLinearColorMap( Qt::black, Qt::white );
                break;
            case jetColorMap:
                double pos;
                colorMap = QwtLinearColorMap( QColor( 0, 0, 189 ), QColor( 132, 0, 0 ) );
                pos = 1.0 / 13.0 * 1.0;
                colorMap.addColorStop( pos, QColor( 0, 0, 255 ) );
                pos = 1.0 / 13.0 * 2.0;
                colorMap.addColorStop( pos, QColor( 0, 66, 255 ) );
                pos = 1.0 / 13.0 * 3.0;
                colorMap.addColorStop( pos, QColor( 0, 132, 255 ) );
                pos = 1.0 / 13.0 * 4.0;
                colorMap.addColorStop( pos, QColor( 0, 189, 255 ) );
                pos = 1.0 / 13.0 * 5.0;
                colorMap.addColorStop( pos, QColor( 0, 255, 255 ) );
                pos = 1.0 / 13.0 * 6.0;
                colorMap.addColorStop( pos, QColor( 66, 255, 189 ) );
                pos = 1.0 / 13.0 * 7.0;
                colorMap.addColorStop( pos, QColor( 132, 255, 132 ) );
                pos = 1.0 / 13.0 * 8.0;
                colorMap.addColorStop( pos, QColor( 189, 255, 66 ) );
                pos = 1.0 / 13.0 * 9.0;
                colorMap.addColorStop( pos, QColor( 255, 255, 0 ) );
                pos = 1.0 / 13.0 * 10.0;
                colorMap.addColorStop( pos, QColor( 255, 189, 0 ) );
                pos = 1.0 / 13.0 * 12.0;
                colorMap.addColorStop( pos, QColor( 255, 66, 0 ) );
                pos = 1.0 / 13.0 * 13.0;
                colorMap.addColorStop( pos, QColor( 189, 0, 0 ) );
                break;
            case standardColorMap:
            default:
                colorMap = QwtLinearColorMap( Qt::darkBlue, Qt::darkRed ); // -2.2, 2.5
                colorMap.addColorStop( 0.0426, Qt::darkCyan ); // u = -2
                colorMap.addColorStop( 0.1277, Qt::cyan ); // u = -1.6
                colorMap.addColorStop( 0.5532, Qt::green ); // u = 0.4
                colorMap.addColorStop( 0.8085, Qt::yellow ); // u = 1.6
                colorMap.addColorStop( 0.8936, Qt::red ); // u = 2
        }
        return colorMap;

    }
    ColorMaps()
    {
        colorMapNames_.append(QPair< ColorMapTypes, QString > ( standardColorMap, "Standard Color Map" ) );
        colorMapNames_.append(QPair< ColorMapTypes, QString > ( greyColorMap, "Grey Color Map" ) );
        colorMapNames_.append(QPair< ColorMapTypes, QString > ( jetColorMap, "Jet Color Map" ) );
        getColorMap(standardColorMap);
    }
    ~ColorMaps()
    {
    }
private:
    QwtLinearColorMap colorMap;
    QList< QPair< ColorMapTypes, QString > > colorMapNames_;
};


Q_DECLARE_METATYPE(ColorMaps::ColorMapTypes)


#endif /* COLOUR_MAPS_H_ */
