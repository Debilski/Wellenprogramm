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
    QwtLinearColorMap colorMap(Qt::darkBlue, Qt::darkRed);  // -2.2, 2.5
    colorMap.addColorStop(0.0426, Qt::darkCyan);  // u = -2
    colorMap.addColorStop(0.1277, Qt::cyan);  // u = -1.6
    colorMap.addColorStop(0.5532, Qt::green);  // u = 0.4
    colorMap.addColorStop(0.8085, Qt::yellow);  // u = 1.6
    colorMap.addColorStop(0.8936, Qt::red);  // u = 2

    std::cout << "saving " << qPrintable(fileName) << " ";
    QImage image = QImage(
        lattice->latticeSizeX(), lattice->latticeSizeY(),
        QImage::Format_ARGB32);

    for (int i = 0; i < lattice->latticeSizeX(); ++i) {
        for (int j = 0; j < lattice->latticeSizeY(); ++j) {
            image.setPixel(i, j, colorMap.color(QwtDoubleInterval(
                                                    -2.2, 2.5),
                                             lattice->getComponentAt(firstComponent, i, j))
                                     .rgb());
        }
    }
    image.mirrored(true, false).save(fileName, "PNG");
    std::cout << "... done";
}

int main(int argc, char** argv)
{
#ifndef __APPLE__
    omp_set_dynamic(0);
    omp_set_num_threads(2);
#endif
    /*  blitz::Array< double, 2> b[2];
     b[0].resize(4,4);
     b[1].resize(4,4);
     blitz::Array< fftw_complex, 2> bc[2];
     bc[0].resize(4,3);
     bc[1].resize(4,4);

     fftw_plan plan[2], planback[2];

     plan[0] = fftw_plan_dft_r2c_2d(4, 4, b[0].data(), bc[0].data(), FFTW_ESTIMATE);
     planback[0] = fftw_plan_dft_c2r_2d(4, 4, bc[0].data(), b[0].data(), FFTW_ESTIMATE);
     plan[1] = fftw_plan_dft_r2c_2d(4, 4, b[1].data(), bc[1].data(), FFTW_ESTIMATE);
     planback[1] = fftw_plan_dft_c2r_2d(4, 4, bc[1].data(), b[1].data(), FFTW_ESTIMATE);

     fftw_execute( plan[0] );
     fftw_execute( planback[0] );
     fftw_execute( plan[1] );
     fftw_execute( planback[1] );
     fftw_execute( plan[0] );
     fftw_execute( planback[0] );
     fftw_execute( plan[1] );
     fftw_execute( planback[1] );
     */
    /*
     LatticeInterface* lat = new FhnLattice(128,128,128,128);
     lat->clear();
     lat->timeStep(0.05);
     lat->setComponentAt(0,2,20,20);
     lat->step();
     lat->step();
     lat->step();
     lat->step();
     lat->step();
     delete lat;
     lat = new BistableLattice(128,128,128,128);
     lat->clear();
     lat->timeStep(0.05);
     lat->setComponentAt(0,2,20,20);
     lat->step();
     lat->step();
     lat->step();
     lat->step();
     lat->step();

     */
    Fftw3Wrapper* fftw = Fftw3Wrapper::instance();
    //fftw->fftwFlags = FFTW_ESTIMATE;
    //fftw->fftwCacheFileName = "fftw_test";

    /*    LatticeInterface* l1 = new FhnLattice(4,4,128,128);
    l1->clear();
    l1->setDiffusion(0, 1.0);
    l1->step();
return 0;
    */
    /*LatticeInterface* l1 = new FhnLattice(60,60,110,110);
    l1->setSpotAtComponent(40,44,12,2,0,true);
    savePng("l1.png", l1);
    LatticeInterface* l2 = new FhnLattice(160,160,110,110);
    l2->copyFromOtherLattice(*l1);
    savePng("l2.png", l2);
    LatticeInterface* l3 = new FhnLattice(60,60,64,64);
    l3->copyFromOtherReal(*l1);
    savePng("l3.png", l3);
    LatticeInterface* l4 = new FhnLattice(60,60,512,512);
    l4->copyFromOtherReal(*l1);
    savePng("l4.png", l4);
*/
    //LatticeInterface* l5 = new FhnLattice(60,60,4512,4512);
    //savePng("l5.png", l5);
    //return 0;
    /*   l5->copyFromOtherReal(*l3);
    savePng("l5.png", l5);

    l3->copyFromOther(*l5, 420, 420);
    savePng("l6.png", l3);

    l5->insertOtherAt(*l3, 400, 400);
    savePng("l7.png", l5);

    blitz::Array<double, 2> ar = l5->getRawComponent(0);
    ar = 2.0*(ar+0.5);
    savePng("l8.png", l5);
    l5->setRawComponent(0,ar);
    savePng("l9.png", l5);
    ar = ar+0.5;
    savePng("lA.png", l5);

    LatticeInterface* l6 = new FhnLattice(60,60,4512,4512);
    l6->copyFromOtherLattice(*l5);
    return 0;
*/
    //    FhnLattice l2 = FhnLattice(8*16,8*16,512,512);

    //    FhnLattice l3 = l2;
    LatticeInterface* lat;  // = new BarkleyLattice( 8 * 16, 8 * 16, 512, 512 );
    //delete lat;
    lat = new FhnLattice();

    std::cout << "C" << std::flush;
    lat->init(8 * 16, 8 * 16, 512, 512);
    //    LatticeInterface* lat2 = new FhnLattice( 8 * 16, 8 * 16, 512, 512 );
    //FhnLattice lat = FhnLattice(4,4,4*32,4*32);

    lat->clear();

    //    NoiseGenerator* ng1 = new NoiseLattice(lat->geometry());
    //    ng1->disconnectFromLattice();
    //    ng1->connectToLattice(lat2);
    //NoiseGenerator* noise = new NoiseLattice(*lat);
    //lat->setNoiseGenerator(noise);
    //  lat.clear();
    //  lat.timestep(0.0001);
    //lat.setNoiseCorrelation(1);
    //lat.setNoiseIntensity(0.01);
    //  lat.setSpotAt(10, 10, 2., 1., 2.5, true);

    /*GeneralComponentSystem points( 2 );
     points[ 0 ] = 1.;
     points[ 1 ] = 0.;*/
    TwoComponentSystem sys(1, 2);
    TwoComponentSystem diff(1, 0);
    sys.u() = 1;
    sys.v() = 2;
    //lat.setAt( sys, 1,2);
    //lat.setDiffusion( diff );

    //    lat->setComponentAt( firstComponent, 1, 1, 2 );
    //    lat->setComponentAt( secondComponent, 2, 1, 2 );
    //lat.setDiffusion( diff );
    //    lat->setSpotAtComponent( 10, 10, 2, 20, firstComponent, true );
    lat->setDiffusion(0, 1);

    //  std::cout << lat;

    lat->status();
    SurfacePoint c, d;
    const int steps = 1000;
    ProgressBar p(steps);
    //    lat->setTimeStep( lat->suggestedTimeStep() );
    lat->suggestedTimeStep();

    boost::progress_display show_progress(steps);
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
        if (i % 50 == 0) {

            //std::cout << std::endl << lat->centreOfMass(0).x << " " << lat->centreOfExcitation(0).x << std::endl;
            //savePng( QString("testfile%1.png").arg(i), lat);
        }
        ++show_progress;
    }

    lat->status();
    delete lat;

    lat = new BarkleyLattice();
    lat->init(1, 1, 1, 1);
    delete lat;
    //std::cout << lat;
    //  lat.step();
    //char* test;
    //std::cin >> test;
    //  std::cout << lat;

    //delete lat;
    /*
     //  Lattice *lat;
     //lat = new Lattice(64,64,1024,1024);
     //Lattice lat(128,128,256,256);
     Lattice lat(128,128,1024,1024);
     //lat = new Lattice(64,64,2048,2048);
     //lat = new Lattice(64,64,512,512);
     cout << time(NULL) << endl;
     //lat.noise_intensity(0.05);
     //lat->periodic(true);
     lat.setSpotAt(10, 10, 2., 1., 2.5, true);

     static int numSteps=50 ;

     ProgressBar p = ProgressBar(numSteps);
     for (int i=0; i<numSteps; ++i) {
     if (i==200) lat.periodic(true);
     p.print(i);
     lat.step(5);
     cout << lat.numClusters();
     SurfacePoint po = lat.centreOfExcitation();
     pair<double, double> curv = lat.getCurvature();
     cout << po.x << " " << curv.first << " "<< curv.second << endl;
     }*/
    /*
     for (int i=0; i<100; ++i) {
     p.print(i);
     lat.step();
     }*/ /*
     SurfacePoint po = lat.centreOfExcitation();
     cout << po.x << endl;*/
    //  delete lat;
    return 0;
}
