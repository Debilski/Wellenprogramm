/*
 * colour_maps.h
 *
 *  Created on: 02.04.2009
 *      Author: rikebs
 */

#ifndef COLOUR_MAPS_H_
#define COLOUR_MAPS_H_


class ColourMaps {
public:
    enum ColourMapTypes {
        standardColourMap, greyColourMap, jetColourMap
    };

    ColourMapTypes colourMapType;

    typedef QPair< ColourMaps::ColourMapTypes, QString > T_identifier;
    const QList< QPair< ColourMapTypes, QString > >& colourMapNames() const {
        return colourMapNames_;
    }
    const QwtColorMap& getColourMap() const
    {
        return colorMap;
    }
    const QwtColorMap& getColourMap(ColourMapTypes type)
    {
        colourMapType = type;
        switch (colourMapType) {
            case greyColourMap:
                colorMap = QwtLinearColorMap( Qt::black, Qt::white );
                qDebug() << "grey";
                break;
            case jetColourMap:
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
            case standardColourMap:
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
    ColourMaps()
    {
        colourMapNames_.append(QPair< ColourMapTypes, QString > ( standardColourMap, "Standard Colour Map" ) );
        colourMapNames_.append(QPair< ColourMapTypes, QString > ( greyColourMap, "Grey Colour Map" ) );
        colourMapNames_.append(QPair< ColourMapTypes, QString > ( jetColourMap, "Jet Colour Map" ) );
        getColourMap(standardColourMap);
    }
    ~ColourMaps()
    {
    }
private:
    QwtLinearColorMap colorMap;
    QList< QPair< ColourMapTypes, QString > > colourMapNames_;
};


Q_DECLARE_METATYPE(ColourMaps::ColourMapTypes)


#endif /* COLOUR_MAPS_H_ */
