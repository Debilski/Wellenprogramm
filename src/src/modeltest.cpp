#include <iostream>

#include <QString>
#include <QImage>

#include "qwt_color_map.h"

#include "rds_lattice.h"
//#include "lattice_models.h"
#include "fhn_lattice.h"
#include "barkley_lattice.h"

#include <boost/progress.hpp>

#include "progressbar/progressbar.h"


void savePng(QString fileName, LatticeInterface* lattice)
{
    QwtLinearColorMap colorMap( Qt::darkBlue, Qt::darkRed ); // -2.2, 2.5
    colorMap.addColorStop( 0.0426, Qt::darkCyan ); // u = -2
    colorMap.addColorStop( 0.1277, Qt::cyan ); // u = -1.6
    colorMap.addColorStop( 0.5532, Qt::green ); // u = 0.4
    colorMap.addColorStop( 0.8085, Qt::yellow ); // u = 1.6
    colorMap.addColorStop( 0.8936, Qt::red ); // u = 2

    std::cout << "saving " << qPrintable( fileName ) << " ";
    QImage image = QImage(
            lattice->latticeSizeX(), lattice->latticeSizeY(),
            QImage::Format_ARGB32 );

    for (int i = 0; i < lattice->latticeSizeX(); ++i) {
        for (int j = 0; j < lattice->latticeSizeY(); ++j) {
            image.setPixel( i, j, colorMap.color( QwtDoubleInterval(
                    -2.2, 2.5 ), lattice->getComponentAt( firstComponent, i, j ) ).rgb() );
        }
    }
    image.mirrored( true, false ).save( fileName, "PNG" );
    std::cout << "... done";
}

int main(int argc, char** argv)
{
#ifndef __APPLE__
    omp_set_dynamic(0);
    omp_set_num_threads(2);
#endif

    Fftw3Wrapper* fftw = Fftw3Wrapper::instance();
    LatticeInterface* lat;// = new BarkleyLattice( 8 * 16, 8 * 16, 512, 512 );
    lat = new FhnLattice();

    lat->init(128, 128, 128, 128);
    lat->clear();

    lat->setDiffusion( 0, 1 );
    lat->setSpotAtComponent(50, 50, 2, 2, 0, true);
/*
    blitz::Array<double, 2> r(128, 128);
    r = 0;
    for(int i = 0; i<128; ++i)
        for(int j=0; j<60; ++j)
            r(i,j) = 2;

    lat->setRawComponent(0, r);
*/

    lat->status();
    SurfacePoint c, d;
    const int steps = 1000;
    ProgressBar p( steps );

    lat->suggestedTimeStep();

    boost::progress_display show_progress( steps );
    boost::progress_timer timer;

    for (int i = 0; i < steps; ++i) {
        //  std::cout << i << " " << std::flush;
        //p.print( i );
        //lat->setTimeStep( lat->suggestedTimeStep() );
        //c = lat->centreOfMass(0);
        //d = lat->centreOfExcitation(0);
        //cout << endl << "?"<< lat->isAlive() << endl;
        lat->step();
        //lat->getClusters();
        //cout << lat->suggestedTimeStep() << endl;
        if ( i % 50 == 0 ) {

            //std::cout << std::endl << lat->centreOfMass(0).x << " " << lat->centreOfExcitation(0).x << std::endl;
            savePng( QString("testfile%1.png").arg(i), lat);
        }
        ++show_progress;
    }

    lat->status();
    delete lat;

    return 0;
}
