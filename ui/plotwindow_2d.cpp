// #include <iostream>
#include <fstream>
#include <cmath>

#include <QtGui>
#include <QtCore>
#include "plotwindow_2d.h"
#include "main_window.h"

//#include "helper.cpp"

/*
 class SpectrogramData: public QwtRasterData
 {
 private:
 double * m_Array;
 double m_minValue;
 double m_maxValue;

 struct structMinMax{
 double min;
 double max;
 };
 structMinMax m_RangeX;
 structMinMax m_RangeY;
 struct structXY{
 double x;
 double y;
 };
 structXY m_DataSize;
 structXY m_RealToArray;

 public:
 // Constructor giving back the QwtRasterData Constructor
 SpectrogramData(): QwtRasterData()
 {
 m_Array = NULL;
 }

 ~SpectrogramData()
 {
 if (m_Array != NULL)
 delete [] m_Array;
 }

 virtual QwtRasterData *copy() const
 {
 SpectrogramData *clone = new SpectrogramData();
 clone->setRangeX(m_RangeX.min, m_RangeX.max);
 clone->setRangeY(m_RangeY.min, m_RangeY.max);
 clone->setBoundingRect(QwtDoubleRect(m_RangeX.min, m_RangeY.min, m_RangeX.max, m_RangeY.max));
 clone->setData(m_Array, m_DataSize.x, m_DataSize.y);
 return clone;
 }

 virtual QwtDoubleInterval range() const
 {
 return QwtDoubleInterval(m_minValue, m_maxValue);
 }

 double value(double x, double y) const
 {
 int xpos = (int)((x - m_RangeX.min) / m_RealToArray.x);
 int ypos = (int)((y - m_RangeY.min) / m_RealToArray.y);
 int pos = ArrPos(xpos, ypos);
 double dvalue = m_Array[pos];
 return dvalue;
 }

 void setData(double * Array, int sizex, int sizey)
 {
 m_DataSize.x = sizex;
 m_DataSize.y = sizey;
 int size = sizex * sizey;
 MinMaxArrayValue(Array, size, &m_minValue, &m_maxValue);
 if (m_Array != NULL)
 delete [] m_Array;
 m_Array = new double [size];
 memcpy(m_Array, Array, size * sizeof(double));

 m_RealToArray.x = (m_RangeX.max - m_RangeX.min) / (m_DataSize.x - 1);
 m_RealToArray.y = (m_RangeY.max - m_RangeY.min) / (m_DataSize.y - 1);
 }

 void setRangeX(const double min, const double max)
 {
 m_RangeX.min = min;
 m_RangeX.max = max;
 }

 void setRangeY(const double min, const double max)
 {
 m_RangeY.min = min;
 m_RangeY.max = max;
 }

 int ArrPos(const int x, const int y) const
 {
 return y + m_DataSize.y * x;
 }

 };


 class MyZoomer: public QwtPlotZoomer
 {
 public:
 MyZoomer(QwtPlotCanvas* canvas): QwtPlotZoomer(canvas)
 {
 setTrackerMode(QwtPicker::AlwaysOn);
 }

 protected:
 virtual QwtText trackerText( const QwtDoublePoint& p ) const
 {
 QwtText t( QwtPlotPicker::trackerText( p ));

 QColor c(Qt::white);
 c.setAlpha(180);
 t.setBackgroundBrush( QBrush(c) );

 return t;
 }
 };

 */

class SpectrogramData : public QwtRasterData {
    Waveprogram2DPlot::ModelLattice* lattice_;
    int variable_;
    Waveprogram2DPlot* programBase;
public:
    SpectrogramData(Waveprogram2DPlot::ModelLattice* lattice, int variable = 0,
                    Waveprogram2DPlot* programBase = 0) :
        QwtRasterData( QwtDoubleRect( 0., 0., (lattice->sizeX()), (lattice->sizeY()) ) ), lattice_(
            lattice ), variable_( variable ), programBase( programBase )
    {
    }
    ~SpectrogramData()
    {
    }
    ;
    // Seltsamer Fehler in Berechnung. Lasse Fkt aus
    virtual QSize rasterHint(const QwtDoubleRect &) const
    {
        return QSize( lattice_->latticeSizeX(), lattice_->latticeSizeY() );
    }

    virtual QwtRasterData* copy() const
    {
        return new SpectrogramData( lattice_, variable_, programBase );
    }

    virtual QwtDoubleInterval range() const
    {

        /*double min = lattice_->componentInfos.at( variable_ ).assumedMin;
         double max = lattice_->componentInfos.at( variable_ ).assumedMax;
         max = lattice_->getMax( variable_ );
         //min = lattice_->getMin( variable_ );*/

        return programBase->plotRanges_real[ variable_ ];
        //return QwtDoubleInterval( min, max );
    }

    virtual double value(double x, double y) const
    { //std::cout << "(" << x << ";" <<  y << ") ";
        // if (y==0) return 1;
        // return 0;
        //return x / 16 * 4 - 2;

        //return _lattice->getU((unsigned int) floor(x * (_lattice->latticeSizeX() ) / _lattice->sizeX() ),
        //                       (unsigned int) floor(y * (_lattice->latticeSizeY() ) / _lattice->sizeY() ));
        int ix = qRound( x / lattice_->scaleX() );
        int iy = qRound( y / lattice_->scaleY() );

        //if ( ix >= 0 && ix < 10 && iy >= 0 && iy < 10 )
        //return _values[ix][iy];

        //assert( ix >= 0 );
        //assert( iy - 1 >= 0 );
        //assert( ix < lattice_->latticeSizeX() );
        //assert( iy - 1 < lattice_->latticeSizeY() );

        return lattice_->getComponentAt( variable_, ix, iy - 1 );
        //return lattice_->getAt((unsigned int) floor(x / lattice_->scaleX()), (unsigned int) floor(y / lattice_->scaleY()))[ variable_ ];
    }
};


class SpectrogramDataFft : public QwtRasterData {
    Waveprogram2DPlot::ModelLattice* lattice_;
    int variable_;
    Waveprogram2DPlot* programBase;
public:
    SpectrogramDataFft(Waveprogram2DPlot::ModelLattice* lattice, int variable = 0,
                    Waveprogram2DPlot* programBase = 0) :
        QwtRasterData( QwtDoubleRect( 0., 0., (lattice->sizeX()), (lattice->sizeY()) ) ), lattice_(
            lattice ), variable_( variable ), programBase( programBase )
    {
    }
    ~SpectrogramDataFft()
    {
    }
    ;
    // Seltsamer Fehler in Berechnung. Lasse Fkt aus
    virtual QSize rasterHint(const QwtDoubleRect &) const
    {
        return QSize( lattice_->latticeSizeX(), lattice_->latticeSizeY() );
    }

    virtual QwtRasterData* copy() const
    {
        return new SpectrogramDataFft( lattice_, variable_, programBase );
    }

    virtual QwtDoubleInterval range() const
    {

        /*double min = lattice_->componentInfos.at( variable_ ).assumedMin;
         double max = lattice_->componentInfos.at( variable_ ).assumedMax;
         max = lattice_->getMax( variable_ );
         //min = lattice_->getMin( variable_ );*/
//todo: ranges für fft
        return programBase->plotRanges_real[ variable_ ];
        //return QwtDoubleInterval( min, max );
    }

    virtual double value(double x, double y) const
    {
        int ix = qRound( x / lattice_->scaleX() );// % (lattice_->sizeX() / 2);
        int iy = qRound( y / lattice_->scaleY() );


        iy = (iy + lattice_->latticeSizeY() / 2) % lattice_->latticeSizeY();
        iy = (iy < lattice_->latticeSizeY() / 2) ? iy : lattice_->latticeSizeY() - iy;
        ix = (ix + lattice_->latticeSizeX() / 2) % lattice_->latticeSizeX();


        return std::abs(lattice_->getFftComponentAt( variable_, ix, iy )) ;// sqrt( lattice_->latticeSize() );
    }
};

void Waveprogram2DPlot::setTitle()
{
    if ( lattice ) {
        setWindowTitle( QString::fromUtf8(
            "‘%1’: Simulate a field size of %2×%2 on a lattice of %3×%3 " ).arg(
            lattice->modelName().c_str() ).arg( lattice->sizeX() ).arg( lattice->latticeSizeX() ) );
    } else {
        setWindowTitle( QString::fromUtf8( "Wellenprogramm: No Model loaded" ) );
    }
}

#include <dlfcn.h>

Waveprogram2DPlot::Waveprogram2DPlot(QMainWindow * parent, int realSize, int latticeSize) :
    QMainWindow( parent ), realSize_( realSize ), latticeSize_( latticeSize ), parent( parent ),
        lattice( 0 )
{
    std::cout << realSize_ << "/" << latticeSize_ << std::endl;
    QStringList arguments = QCoreApplication::instance()->arguments();

    QStringList pathList = arguments.filter( "--libdir=" );

    QStringList filters;
#ifdef Q_OS_DARWIN
    filters << "*lattice.dylib";
#else
    filters << "*lattice.so";
#endif
    QString testLibDir;
    if ( !pathList.empty() ) {
        testLibDir = pathList.first();
        testLibDir.replace( "--libdir=", "" );
    } else {
#ifdef Q_OS_DARWIN
        testLibDir = "builds/darwin/models";
#else
        testLibDir = "builds/lomo/models";
#endif
    }
    std::cout << "Checking if " << qPrintable(testLibDir) << " exists… ";
    QDir libDir( testLibDir );
    if ( !libDir.exists() ) {
        std::cout << "No!" << std::endl;
        exit( -1 );
    } else {
        std::cout << "Yes." << std::endl;
    }
    libDir.setNameFilters( filters );
    QFileInfoList libs = libDir.entryInfoList();

    QList< void * > dl_list; // list to hold handles
    // for dynamic libs

    foreach (const QFileInfo &libInfo, libs) {
            std::cout << "Found library: " << qPrintable(libInfo.filePath()) << std::endl;
            char name[ 1024 ];
            std::strcpy( name, qPrintable( libInfo.filePath() ));

            void* dlib = dlopen( name, RTLD_NOW );
            if ( dlib == NULL ) {
                std::cerr << dlerror() << std::endl;
                exit( -1 );
            }
            dl_list << dlib;
        }

    /*
     void *dlib;
     char name[ 1024 ];
     while (fgets( in_buf, BUF_SIZE, dl )) {
     // trim off the whitespace
     char *ws = strpbrk( in_buf, " \t\n" );
     if ( ws )
     *ws = '\0';
     // append ./ to the front of the lib name
     sprintf( name, "./%s", in_buf );
     std::cout << name << std::endl;
     dlib = dlopen( name, RTLD_NOW );
     if ( dlib == NULL ) {
     std::cerr << dlerror() << std::endl;
     exit( -1 );
     }
     // add the handle to our list
     dl_list.insert( dl_list.end(), dlib );
     }
     */
    /*    int i = 0;
     // create an array of the shape names
     for (fitr = factory.begin(); fitr != factory.end(); fitr++) {
     shape_names.insert( shape_names.end(), fitr->first );
     i++;
     }
     */

    colourMapType = standardColourMap;
    colourMapMode = defaultColourMapMode;

    setupUi( this ); // this sets up GUI
    sliceWidget->setVisible( false );

    //    setAttribute( Qt::WA_MacMetalStyle );
    setTitle();

    adaptationMode_ = false;
    loopruns = false;

    boundaryConditionIdentifier[ FixedBoundary ] = "Fixed Boundary";
    boundaryConditionIdentifier[ PeriodicBoundary ] = "Periodic Boundary";
    boundaryConditionIdentifier[ NoFluxBoundary ] = "Reflecting Boundary";
    boundaryConditionIdentifier[ FixedValueBoundary ] = "Fixed Value Boundary";
    boundaryConditionIdentifier[ NoReactionBoundary ] = "No Reaction Boundary";

    time_t seed;

    time( &seed );
    srand48( (unsigned int) seed );

    lastClustersUpdateTime = 0;

    movieQueue = QQueue< QString > ();

    connect( parent, SIGNAL(window_closed() ), this, SLOT(close() ) );

    connect( startStopButton, SIGNAL( clicked() ), this, SLOT( toggleStartStop() ) );
    connect( actionStartStop, SIGNAL( triggered() ), this, SLOT( toggleStartStop() ) );

    setUpActions();

    setUpColourSchemeMenu();

    initField( realSize, latticeSize, "FhnLattice" );



    boundaryConditionsComboBox->setCurrentIndex( lattice->boundaryCondition() );

    noise_correlationValue->setValue( lattice->noiseCorrelation() );
    timestepValue->setValue( lattice->timeStep() );

    midpoint_sizeValue->setValue( 0 );

    showClusterIds_ = false;

    //QwtValueList contourLevels;

    /*for (double level = 0.5; level < 10.0; level += 1.0)
     contourLevels += level;
     d_spectrogram->setContourLevels(contourLevels);
     d_curvature_spectrogram->setContourLevels(contourLevels);
     */

    updatePeriodTime_ = 25;
    setUpUpdatePeriodMenu();
    defectsEditor = 0;
    this->show();

    replot();

    timer = new QTimer( this );
    connect( timer, SIGNAL(timeout()), this, SLOT(replot()));

    clipboard = QApplication::clipboard();

    matlabExportFile_ = QString();
    matlabExportIndex_ = 0;

    readSettings();
}

void Waveprogram2DPlot::closeEvent(QCloseEvent * event)
{
    writeSettings();
    event->accept();
}

void Waveprogram2DPlot::on_actionClose_Window_triggered()
{
    close();
}

void Waveprogram2DPlot::on_actionEdit_Script_triggered()
{
    scriptEditor = new ScriptEditor(this);
    scriptEditor->show();
    scriptEditor->raise();
    scriptEditor->activateWindow();
}

void Waveprogram2DPlot::on_actionEdit_defects_triggered()
{
    if ( !defectsEditor ) {
        defectsEditor = new DefectsEditor( defectsList, boundaryConditionIdentifier, this );
        connect( defectsEditor, SIGNAL(accepted()), this, SLOT(updateDefects()) );
    }

    defectsEditor->show();
    defectsEditor->raise();
    defectsEditor->activateWindow();

}

void Waveprogram2DPlot::updateDefects()
{
    std::cout << "updateDefects";
    defectsList = defectsEditor->defects;
    lattice->removeDefects();
    for (int i = 0; i < defectsList.size(); ++i) {
        lattice->addDefect( defectsList.at( i ) );
    }
}

void Waveprogram2DPlot::setUpColourSchemeMenu()
{

    changeColourSchemeGroup = new QActionGroup( this );
    changeColourSchemeGroup->addAction( actionStandard );
    changeColourSchemeGroup->addAction( actionGrey );
    changeColourSchemeGroup->addAction( actionJet );
    actionStandard->setChecked( true );
    connect( changeColourSchemeGroup, SIGNAL( triggered( QAction* ) ), this, SLOT( updateColourScheme( QAction* )) );

    changeColourSchemeModeGroup = new QActionGroup( this );
    changeColourSchemeModeGroup->addAction( actionDefault_Colour_Scheme );
    changeColourSchemeModeGroup->addAction( actionAdaptive_Scheme );
    changeColourSchemeModeGroup->addAction( actionDelayed_Adaptive_Scheme );
    actionDefault_Colour_Scheme->setChecked( true );
    connect( changeColourSchemeModeGroup, SIGNAL( triggered( QAction* ) ), this, SLOT( updateColourSchemeMode( QAction* )) );
}

void Waveprogram2DPlot::updateColourSchemeMode(QAction* a)
{
    if ( a == actionDefault_Colour_Scheme )
        colourMapMode = defaultColourMapMode;
    if ( a == actionAdaptive_Scheme )
        colourMapMode = adaptiveColourMapMode;
    if ( a == actionDelayed_Adaptive_Scheme )
        colourMapMode = delayedAdaptiveColourMapMode;
    setUpColorMap();
}

void Waveprogram2DPlot::updateColourScheme(QAction* a)
{
    if ( a == actionStandard )
        colourMapType = standardColourMap;
    if ( a == actionGrey )
        colourMapType = greyColourMap;
    if ( a == actionJet )
        colourMapType = jetColourMap;
    setUpColorMap();
}

void Waveprogram2DPlot::setUpUpdatePeriodMenu()
{
    updatePeriodGroup = new QActionGroup( this );
    updatePeriodGroup->addAction( action100_ms );
    updatePeriodGroup->addAction( action1_s );
    updatePeriodGroup->addAction( action25_ms );
    updatePeriodGroup->addAction( action500_ms );
    updatePeriodGroup->addAction( action5_s );
    action25_ms->setChecked( true );
    connect( updatePeriodGroup, SIGNAL( triggered( QAction* ) ), this, SLOT( updateUpdatePeriod( QAction* ) ) );
}

void Waveprogram2DPlot::updateUpdatePeriod(QAction* a)
{
    if ( a == action100_ms )
        updatePeriodTime_ = 100;
    if ( a == action1_s )
        updatePeriodTime_ = 1000;
    if ( a == action25_ms )
        updatePeriodTime_ = 25;
    if ( a == action500_ms )
        updatePeriodTime_ = 500;
    if ( a == action5_s )
        updatePeriodTime_ = 5000;

    timer->setInterval( updatePeriodTime_ );
    if ( timer->isActive() )
        timer->start();
}

void Waveprogram2DPlot::setUpSlices()
{
    for (uint i = 0; i < slice.size(); ++i)
        slice[ i ] ->detach();
    slice.resize( lattice->numberOfVariables() );
    for (uint component = 0; component < lattice->numberOfVariables(); ++component) {
        slice[ component ] = new QwtPlotCurve( QString( "slice %1" ).arg( QString::fromStdString(
            lattice->componentInfos[ component ].name() ) ) );

        slice[ component ]->attach( slicePlot );

    }
}

void Waveprogram2DPlot::setUpColorMap()
{
    //colourMapType = greyColourMap;
    switch (colourMapType) {
        case greyColourMap:
            colorMap = QwtLinearColorMap( Qt::black, Qt::white );
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
    emit colorMapChanged( colorMap );
}

void Waveprogram2DPlot::removeTabs()
{
    while (plotTabWidget->count() > 0) {
        if ( QLatin1String( plotTabWidget->currentWidget()->metaObject()->className() )
            == QLatin1String( "PlotView" ) )
        {

            PlotView* currentPlotView = static_cast< PlotView* > ( plotTabWidget->currentWidget() );

            disconnect( this, 0, currentPlotView, 0 );
            delete currentPlotView->plot_;
            //delete currentPlotView->spectrogram_;
            //delete currentPlotView->colorMap_;
            delete currentPlotView;
        }

        plotTabWidget->removeTab( 0 );
    }
}

void Waveprogram2DPlot::setUpRanges()
{
    plotRanges_real.clear();
    plotRanges_real.resize( lattice->numberOfVariables() );
    std::cout << lattice->numberOfVariables();
    for (uint component = 0; component < lattice->numberOfVariables(); ++component) {
        double max;
        double min;

        max = lattice->componentInfos[ component ].assumedMax();
        min = lattice->componentInfos[ component ].assumedMin();
        if ( min >= max ) {
            max = 2.5;
            min = -2.2;
        }
        plotRanges_real[ component ] = QwtDoubleInterval( min, max );
    }
}

void Waveprogram2DPlot::adaptRange(uint& component, bool& isFft)
{
    double max;
    double min;

    max = lattice->componentInfos[ component ].assumedMax();
    min = lattice->componentInfos[ component ].assumedMin();
    if ( min >= max ) {
        max = 2.5;
        min = -2.2;
    }
    if ( colourMapMode == adaptiveColourMapMode ) {
        max = lattice->getMax( component );
        min = lattice->getMin( component );

        max = std::floor( max * 5.0 + 1 ) / 5.0;
        min = std::ceil( min * 5.0 - 1 ) / 5.0;
    }

    if ( colourMapMode == delayedAdaptiveColourMapMode ) {
        double oldMax = plotRanges_real[ component ].maxValue();
        double oldMin = plotRanges_real[ component ].minValue();

        // Neuen Wert abschätzen
        max = lattice->getMax( component );
        min = lattice->getMin( component );

        // Wenn Abweichung nicht zu größer 1: alten Wert behalten, sonst auf 0.2 runden
        if ( max < oldMax && max > oldMax - 1 ) {
            max = oldMax;
        } else {
            max = std::floor( max * 5.0 + 1 ) / 5.0;
        }
        if ( min > oldMin && min < oldMin + 1 ) {
            min = oldMin;
        } else {
            min = std::ceil( min * 5.0 - 1 ) / 5.0;
        }
    }
    plotRanges_real[ component ] = QwtDoubleInterval( min, max );
}

void Waveprogram2DPlot::setUpTabs()
{
    QFont plotFont = QFont( "", 8 );
    QwtText labelIntensity( "Intensity" );
    labelIntensity.setFont( plotFont );
    setUpRanges();
    disconnect( this, SIGNAL( replotTab() ), 0, 0 );
    disconnect( this, SIGNAL( colorMapChanged( const QwtColorMap& )), 0, 0 );
    plotViewVector_.clear();
    for (uint component = 0; component < lattice->numberOfVariables(); ++component) {

        QString label;
        if ( !lattice->componentInfos[ component ].physicalQuantity().empty() ) {
            label = QString( "%1 / %2" );
            label = label.arg(
                lattice->componentInfos[ component ].physicalQuantity().c_str(),
                lattice->componentInfos[ component ].physicalUnitSymbol().c_str() );
        } else {
            label = QString( "Intensity" );
        }
        PlotView* tab = new PlotView(
            SpectrogramData( lattice, component, this ), colorMap, component, label, plotTabWidget );

        plotViewVector_ << tab;
        // tab->plot_->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );

        connect( tab, SIGNAL( adaptComponent(uint&, bool&) ), this, SLOT( adaptRange(uint&, bool&) ) );
        connect( this, SIGNAL( replotTab() ), tab, SLOT( replot() ) );

        connect( this, SIGNAL( colorMapChanged( const QwtColorMap& )), tab, SLOT( setColorMap( const QwtColorMap& )));
        //! Sendet an potenziell zu viele Tabs…
        connect( plotTabWidget, SIGNAL( currentChanged(int) ), tab, SLOT( replot(int) ) );

        QString name = QString( "%1 (%2)" ).arg( QString::fromStdString(
            lattice->componentInfos[ component ].name() ), QString::fromStdString(
            lattice->componentInfos[ component ].shortName() ) );

        plotTabWidget->addTab( tab, name );
    }


    for (uint component = 0; component < lattice->numberOfVariables(); ++component) {

        QString label;
        if ( !lattice->componentInfos[ component ].physicalQuantity().empty() ) {
            label = QString( "%1 / %2" );
            label = label.arg(
                lattice->componentInfos[ component ].physicalQuantity().c_str(),
                lattice->componentInfos[ component ].physicalUnitSymbol().c_str() );
        } else {
            label = QString( "Intensity" );
        }
        PlotView* tab = new PlotView(
            SpectrogramDataFft( lattice, component, this ), colorMap, component, label, plotTabWidget, true );

        plotViewVector_ << tab;
        // tab->plot_->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );

        connect( tab, SIGNAL( adaptComponent(uint&, bool&) ), this, SLOT( adaptRange(uint&, bool&) ) );
        connect( this, SIGNAL( replotTab() ), tab, SLOT( replot() ) );

        connect( this, SIGNAL( colorMapChanged( const QwtColorMap& )), tab, SLOT( setColorMap( const QwtColorMap& )));
        //! Sendet an potenziell zu viele Tabs…
        connect( plotTabWidget, SIGNAL( currentChanged(int) ), tab, SLOT( replot(int) ) );

        QString name = QString( "Fourier %1 (%2)" ).arg( QString::fromStdString(
            lattice->componentInfos[ component ].name() ), QString::fromStdString(
            lattice->componentInfos[ component ].shortName() ) );

        plotTabWidget->addTab( tab, name );
    }


    plotTabWidget->show();

    /*  for (uint component=0; component < lattice->number_of_Variables; ++component) {
     PlotView* tab = new PlotView();
     tab->plot_ = new QwtPlot(tab);
     tab->spectrogram_ = new QwtPlotSpectrogram();
     tab->spectrogram_->setData(SpectrogramData(lattice, component));
     tab->spectrogram_->attach(tab->plot_);

     QString name = QString("%1 (%2)").arg(QString::fromStdString(lattice->componentNames[component]),QString::fromStdString(lattice->componentNamesShort[component]));
     plotTabWidget->addTab(tab, name);
     }*/
}

void Waveprogram2DPlot::setUpDiffusion()
{
    for (uint component = 0; component < lattice->numberOfVariables(); ++component) {
        QDoubleSpinBox* diffusionBox = new QDoubleSpinBox( parameterWidgetContents );

        //! Bisschen unsauber…
        diffusionBox->setProperty( "component", QVariant( component ) );
        diffusionBox->setDecimals( 3 );
        diffusionBox->setMaximum( 1000 );
        diffusionBox->setValue( lattice->getDiffusion( component ) );
        QString name = "Diffusion " + QString::fromStdString(
            lattice->componentInfos[ component ].shortName() );
        QLabel* label = new QLabel( name, parameterWidgetContents );
        parameterWidgetFormLayout->addRow( label, diffusionBox );
        connect( diffusionBox, SIGNAL( valueChanged(const double&) ), this, SLOT( changeDiffusion(const double&) ) );
    }
}

void Waveprogram2DPlot::changeDiffusion(double value)
{
    if ( sender() != 0 ) {
        std::cout << "Diff" << sender()->property( "component" ).toInt() << " to " << value;
        changeDiffusion( sender()->property( "component" ).toInt(), value );
    }
}

void Waveprogram2DPlot::changeDiffusion(int component, double value)
{
    lattice->setDiffusion( component, value );
}

void Waveprogram2DPlot::setUpModelProperties()
{
    if ( parameterWidgetContents )
        delete parameterWidgetContents;
    //  if (parameterWidgetFormLayout)
    //    delete parameterWidgetFormLayout;
    if ( lattice ) {
        parameterWidget->setWindowTitle( QString( "%1 Properties" ).arg(
            lattice->modelName().c_str() ) );
    }
    parameterWidgetContents = new QWidget();
    parameterWidgetContents->setObjectName( QString::fromUtf8( "parameterWidgetContents" ) );
    verticalLayout = new QVBoxLayout( parameterWidgetContents );
    verticalLayout->setObjectName( QString::fromUtf8( "verticalLayout" ) );
    parameterWidgetFormLayout = new QFormLayout();
    parameterWidgetFormLayout->setObjectName( QString::fromUtf8( "parameterWidgetFormLayout" ) );

    verticalLayout->addLayout( parameterWidgetFormLayout );

    parameterWidget->setWidget( parameterWidgetContents );

    if ( adaptationParameterWidgetContents )
        delete adaptationParameterWidgetContents;
    //  if (parameterWidgetFormLayout)
    //    delete parameterWidgetFormLayout;
    if ( lattice ) {
        adaptationParameterWidget->setWindowTitle( QString( "%1 Properties" ).arg(
            lattice->modelName().c_str() ) );
    }
    adaptationParameterWidgetContents = new QWidget();
    adaptationParameterWidgetContents->setObjectName( QString::fromUtf8(
        "adaptationParameterWidgetContents" ) );
    verticalLayout = new QVBoxLayout( adaptationParameterWidgetContents );
    verticalLayout->setObjectName( QString::fromUtf8( "verticalLayout" ) );
    adaptationParameterWidgetFormLayout = new QFormLayout();
    adaptationParameterWidgetFormLayout->setObjectName( QString::fromUtf8(
        "adaptationParameterWidgetFormLayout" ) );

    verticalLayout->addLayout( adaptationParameterWidgetFormLayout );
    adaptationParameterWidget->setWidget( adaptationParameterWidgetContents );

    QLabel* label = new QLabel( "Adaptation Mode", adaptationParameterWidgetContents );
    adaptationModeCheckBox = new QCheckBox( adaptationParameterWidgetContents );
    adaptationModeCheckBox->setObjectName( QString::fromUtf8( "adaptationModeCheckBox" ) );
    connect( adaptationModeCheckBox, SIGNAL( clicked(bool) ), this, SLOT ( adaptationModeCheckBox_clicked(bool) ) );
    adaptationParameterWidgetFormLayout->addRow( label, adaptationModeCheckBox );

    setUpDiffusion();
    setUpParameters();
    setUpAdaptationParameters();
}

void Waveprogram2DPlot::setUpParameters()
{
    std::list< Parameter< double >* > params = lattice->parameters();
    std::list< Parameter< double >* >::iterator param;

    latticeParameters.clear();
    for (param = params.begin(); param != params.end(); ++param) {
        QString paramName = QString::fromUtf8( (*param)->name.c_str() );
        latticeParameters.insert( paramName, *param );
        std::cout << paramName.toStdString() << ":" << " " << *param;

        ParameterSpinBox* parameterBox = new ParameterSpinBox( *param, parameterWidgetContents );

        QLabel* label = new QLabel( paramName, parameterWidgetContents );
        parameterWidgetFormLayout->addRow( label, parameterBox );
        connect( parameterBox, SIGNAL( valueChanged(const double&) ), this, SLOT( changeParameter(const double&) ) );
        connect( this, SIGNAL( updateParameters() ), parameterBox, SLOT ( updateValue() ));
    }
}

void Waveprogram2DPlot::setUpAdaptationParameters()
{
    std::list< Parameter< double >* > params = lattice->adaptationParameters();
    std::list< Parameter< double >* >::iterator param;

    latticeAdaptationParameters.clear();
    for (param = params.begin(); param != params.end(); ++param) {
        QString paramName = QString::fromUtf8( (*param)->name.c_str() );
        latticeAdaptationParameters.insert( paramName, *param );
        std::cout << paramName.toStdString() << ":" << " " << *param;

        ParameterSpinBox* parameterBox = new ParameterSpinBox(
            *param, adaptationParameterWidgetContents );

        /*        QDoubleSpinBox* parameterBox = new QDoubleSpinBox( adaptationParameterWidgetContents );
         //! Bisschen unsauber…
         parameterBox->setProperty( "parameter", QVariant( paramName ) );
         parameterBox->setDecimals( (*param)->decimals() );
         parameterBox->setMaximum( (*param)->max() );
         parameterBox->setMinimum( (*param)->min() );
         parameterBox->setSingleStep( (*param)->stepSizeHint() );
         parameterBox->setValue( (*param)->get() );
         */
        QLabel* label = new QLabel( paramName, adaptationParameterWidgetContents );
        adaptationParameterWidgetFormLayout->addRow( label, parameterBox );
        connect( parameterBox, SIGNAL( valueChanged(const double&) ), this, SLOT( changeParameter(const double&) ) );

        connect( this, SIGNAL( updateParameters() ), parameterBox, SLOT ( updateValue() ));
    }
    if ( latticeAdaptationParameters.size() == 0 ) {
        adaptationModeCheckBox->setCheckable( false );
    } else {
        adaptationModeCheckBox->setCheckable( true );
    }
}

void Waveprogram2DPlot::changeParameter(double value)
{
    if ( sender() != 0 ) {
        QString paramName = sender()->property( "parameter" ).toString();
        std::cout << paramName.toStdString();
        Parameter< double >* p = latticeParameters.value( paramName );
        std::cout << p;
        if ( p != 0 ) {
            changeParameter( p, value );
            return;
        }
        p = latticeAdaptationParameters.value( paramName );
        std::cout << p;
        if ( p != 0 ) {
            changeParameter( p, value );
            return;
        }
    }
}

void Waveprogram2DPlot::changeParameter(Parameter< double >* p, double value)
{
    p->set( value );
}

void Waveprogram2DPlot::setUpBoundaryConditionsSelector()
{
    //  boundaryConditionsComboBox;
}

void Waveprogram2DPlot::on_boundaryConditionsComboBox_currentIndexChanged(int i)
{
    if ( i == 0 )
        lattice->setBoundaryCondition( FixedBoundary );
    if ( i == 1 )
        lattice->setBoundaryCondition( PeriodicBoundary );
    if ( i == 2 )
        lattice->setBoundaryCondition( NoFluxBoundary );
}

void Waveprogram2DPlot::on_noiseBox_valueChanged(double d)
{
    lattice->setNoiseIntensity( d );
}

void Waveprogram2DPlot::on_noise_correlationValue_valueChanged(int d)
{
    lattice->setNoiseCorrelation( d );
}

void Waveprogram2DPlot::on_timestepValue_valueChanged(double d)
{
    lattice->setTimeStep( d );
}

void Waveprogram2DPlot::on_actionShow_Curvature_triggered(bool b)
{
    //! if (b==true) curvaturePlot->show();
    //! else curvaturePlot->hide();
}

void Waveprogram2DPlot::on_actionShow_Cluster_Ids_triggered(bool b)
{
    showClusterIds_ = b;
    if ( !showClusterIds_ )
        bufferMap.clear();
}

void Waveprogram2DPlot::on_midpoint_sizeValue_valueChanged(double d)
{
    Defect< GeneralComponentSystem > midpointDefect;
    midpointDefect.centre.x = realSize_ / 2.0;
    midpointDefect.centre.y = realSize_ / 2.0;
    midpointDefect.boundaryCondition = NoReactionBoundary;
    midpointDefect.radius = d;
    lattice->addDefect( midpointDefect );

    // double s = 0;//! = lattice->getSpotSize(d, true);
    // QString text = QString( "Area: %1 FE" ).arg( s );
    // statusBar()->showMessage( text );
}

void Waveprogram2DPlot::replot()
{

    if ( !lattice )
        return;
    foreach(QwtPlotCurve* curve, curves ) {
            curve->attach( 0 );
            delete curve;
        }
    curves.clear();

    for (int i = 0; i < qMarkerVector.size(); ++i) {
        qMarkerVector[ i ]->attach( 0 );
        delete qMarkerVector[ i ];
    }
    qMarkerVector.clear();

    // Check, if we have a PlotView in the current tab.
    if ( showClusterIds_ && QLatin1String(
        plotTabWidget->currentWidget()->metaObject()->className() ) == QLatin1String( "PlotView" ) )
    {
        PlotView* currentView = static_cast< PlotView* > ( plotTabWidget->currentWidget() );
        QwtPlot * currentPlot = currentView->plot_;

        // Color Map einbauen…
        //currentView->spectrogram_->set
        //currentPlot->rightAxis->setColorMap( tab->spectrogram_->data().range(), tab->spectrogram_->colorMap() );
        qMarkerVector.reserve( lattice->numberOfClusters() );
        std::vector< Cluster > clusterVector = lattice->getClusters();
/*
        std::vector< SurfacePoint > tips = lattice->getSpiralTips();
        for (uint i = 0; i < tips.size(); ++i) {
            QwtPlotMarker* marker = new QwtPlotMarker;
            marker->setValue( tips[ i ].x, tips[ i ].y );
            marker->setLabel( QString( "Hi!" ) );
            marker->attach( currentPlot );
        }
*/
        // Marker
        for (uint i = 0; i < clusterVector.size(); ++i) {
            QwtPlotMarker* marker = new QwtPlotMarker;
            marker->setValue( clusterVector[ i ].position.x, clusterVector[ i ].position.y );
            QString label = "#%1\nSize: %2\nSpeed: %3 (est.)";
            label = label.arg( clusterVector[ i ].clusterId ).arg( clusterVector[ i ].size );

            // Calculate speed
            if ( lastStepClusters.contains( clusterVector[ i ].clusterId ) ) {
                double len = lattice->euklidianDistance(
                    lastStepClusters[ clusterVector[ i ].clusterId ], clusterVector[ i ].position );
                double speed = len / (-lastClustersUpdateTime + lattice->time());
                label = label.arg( speed );
            } else {
                label = label.arg( "–" );
            }

            marker->setLabel( label );
            marker->attach( currentPlot );
            qMarkerVector << marker;

            std::cout << "\n"<< lattice->time() << ": "<< clusterVector[ i ].position.x << "!";
        }

        lastClustersUpdateTime = lattice->time();
        lastStepClusters.clear();

        temporaryBufferMap.clear();

        for (uint i = 0; i < clusterVector.size(); ++i) {
            long int cId = clusterVector[ i ].clusterId;
            if ( !bufferMap.contains( cId ) ) {
                QList< QPair< SurfacePoint, double > > cb;

                temporaryBufferMap[ cId ] = cb;
            } else {
                temporaryBufferMap[ cId ] = bufferMap[ cId ];
            }
            SurfacePoint sp;
            sp.x = clusterVector[ i ].position.x;
            sp.y = clusterVector[ i ].position.y;

            lastStepClusters[ cId ] = sp;
            temporaryBufferMap[ cId ].append( QPair< SurfacePoint, double > ( sp, lattice->time() ) );
        }

        typedef QMap< long int, QList< QPair< SurfacePoint, double > > > T_bufferMap;
        typedef QList< QPair< SurfacePoint, double > > T_listPairs;
        typedef QPair< SurfacePoint, double > T_pair;

        // Entfernt alles, was älter als tdiff=5 ist
        for (T_bufferMap::Iterator i = temporaryBufferMap.begin(); i != temporaryBufferMap.end(); ++i)
        {
            for (T_listPairs::Iterator j = (*i).begin(); j != (*i).end(); ++j) {
                if ( (*j).second < lattice->time() - 5 ) {
                    (*i).erase( j );
                }
            }
        }
        bufferMap = temporaryBufferMap;
        /*
         QList< long int > bufferMapKeys = bufferMap.keys();
         foreach ( long int id, bufferMapKeys ) {
         bool in_current = false;
         for ( uint i = 0; i < clusterVector.size(); ++i ) {
         if ( clusterVector[ i ].clusterId == id ) {
         in_current = true;
         break;
         }
         }
         if ( !in_current ) {
         bufferMap.remove( id );
         }
         }
         */

        //boost::circular_buffer<SurfacePoint> cb(50);
        QBrush brush( Qt::blue );
        QPen pen( brush, 2, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin );

        foreach( T_listPairs cb, bufferMap ) {
                // QwtPlotCurve* curve1 = new QwtPlotCurve();
                QwtArray< double > qv1, qv2;
                double lastx = -1;
                double lasty = -1;
                foreach( T_pair sp , cb ) {

                        if ( lastx > 0 && (abs( lastx - sp.first.x ) > 25 || abs( lasty
                            - sp.first.y ) > 25) )
                        {

                            QwtPlotCurve* curve = new QwtPlotCurve();
                            curve->setPen( pen );
                            curve->setData( qv1, qv2 );
                            curve->attach( currentPlot );
                            curves << curve;
                            qv1.clear();
                            qv2.clear();
                        }
                        qv1 << sp.first.x;
                        qv2 << sp.first.y;
                        lastx = sp.first.x;
                        lasty = sp.first.y;
                        //        std::cout << ".";
                    }
                //      std::cout << std::endl << std::flush;
                QwtPlotCurve* curve = new QwtPlotCurve();
                curve->setPen( pen );
                curve->setData( qv1, qv2 );
                curve->attach( currentPlot );
                curves << curve;
            }

        emit
        replotTab();

        foreach( QwtPlotCurve* curve, curves) {
                //  curve->attach( NULL );
                // delete curve;
            }

    } else {
        emit replotTab();
    }
    /*
     for(int i=0; i<qMarkerList.size(); ++i) {
     delete qMarkerList.at(i);
     }
     */

    if ( slicePlot->isVisible() ) {
        QVector< double > sliceData[ lattice->numberOfVariables() ];
        QVector< double > sliceLatticePoints;

        for (int i = 0; i < lattice->latticeSizeX(); ++i) {
            for (uint component = 0; component < lattice->numberOfVariables(); ++component) {
                sliceData[ component ].push_back( lattice->getComponentAt(
                    component, i, lattice->latticeSizeY() / 2 ) );
            }
            sliceLatticePoints.push_back( i * lattice->scaleX() );
        }
        if ( slice.size() == lattice->numberOfVariables() )
            for (uint component = 0; component < lattice->numberOfVariables(); ++component) {
                slice[ component ]->setData( sliceLatticePoints, sliceData[ component ] );
            }
        slicePlot->replot();
    }

    //phase->replot();
    waveSizeLabel->setNum( lattice->currentWavesize() );
    if ( latticeParameters.value( "gamma" ) )
        gammaLabel->setNum( latticeParameters.value( "gamma" )->get() );
}

Waveprogram2DPlot::~Waveprogram2DPlot()
{
    killField();
    delete timer;

}

void Waveprogram2DPlot::setUpActions()
{
    QList< QAction * > availableActions = menuMode->actions();

    ModelAction* action;
    std::list< std::string > models = LatticePluginRegistration::instance()->models();
    for (std::list< std::string >::const_iterator it = models.begin(); it != models.end(); ++it) {
        action = new ModelAction( QString( (*it).c_str() ), (*it), this );
        menuMode->insertAction( availableActions.first(), action );
        connect( action, SIGNAL( modelTriggered(std::string) ), this, SLOT( changeModel(std::string) ) );
    }

#if 0

    action = new ModelAction( "FithHugh-Nagumo", RdsModels::FhnLattice, this );
    menuMode->insertAction( availableActions.first(), action );
    connect( action, SIGNAL( modelTriggered(RdsModels::RdsModelType) ), this, SLOT( changeModel(RdsModels::RdsModelType) ) );
    action = new ModelAction( "FithHugh-Nagumo 3 Comp.", RdsModels::FhnAdditionalLattice, this );
    menuMode->insertAction( availableActions.first(), action );
    connect( action, SIGNAL( modelTriggered(RdsModels::RdsModelType) ), this, SLOT( changeModel(RdsModels::RdsModelType) ) );
    action = new ModelAction( "Bistable", RdsModels::BistableLattice, this );
    menuMode->insertAction( availableActions.first(), action );
    connect( action, SIGNAL( modelTriggered(RdsModels::RdsModelType) ), this, SLOT( changeModel(RdsModels::RdsModelType) ) );
    action = new ModelAction( "Piecewise Linear", RdsModels::PiecewiseLinearLattice, this );
    menuMode->insertAction( availableActions.first(), action );
    connect( action, SIGNAL( modelTriggered(RdsModels::RdsModelType) ), this, SLOT( changeModel(RdsModels::RdsModelType) ) );
    action = new ModelAction( "Barkley", RdsModels::BarkleyLattice, this );
    menuMode->insertAction( availableActions.first(), action );
    connect( action, SIGNAL( modelTriggered(RdsModels::RdsModelType) ), this, SLOT( changeModel(RdsModels::RdsModelType) ) );
    action = new ModelAction( "Scott", RdsModels::ScottLattice, this );
    menuMode->insertAction( availableActions.first(), action );
    connect( action, SIGNAL( modelTriggered(RdsModels::RdsModelType) ), this, SLOT( changeModel(RdsModels::RdsModelType) ) );
    action = new ModelAction( "Oregonator", RdsModels::OregonatorLattice, this );
    menuMode->insertAction( availableActions.first(), action );
    connect( action, SIGNAL( modelTriggered(RdsModels::RdsModelType) ), this, SLOT( changeModel(RdsModels::RdsModelType) ) );
    action = new ModelAction( "Hodgkin-Huxley", RdsModels::HodgkinHuxleyLattice, this );
    menuMode->insertAction( availableActions.first(), action );
    connect( action, SIGNAL( modelTriggered(RdsModels::RdsModelType) ), this, SLOT( changeModel(RdsModels::RdsModelType) ) );
#endif
}

void Waveprogram2DPlot::changeModel(std::string m)
{
    loopStop();
    killField();

    //removeTabs();
    initField( 1, 1, m );
}

void Waveprogram2DPlot::initField(int realSize, int latticeSize, std::string model)
{

    removeTabs();
    if ( parent != 0 ) {
        if ( QLatin1String( parent->metaObject()->className() ) == QLatin1String( "mainWin" ) ) {
            parent->statusBar()->showMessage( QString(
                "Creating a lattice of %1 x %1. This might take some time" ).arg( latticeSize_ ) );
        }
    }
    QCoreApplication::processEvents();
    std::cout << model << std::endl;
    /*    lattice = new FhnLattice(1,1,1,1);
     delete lattice;
     lattice = new BarkleyLattice(1,1,1,1);
     delete lattice;

     */

    std::list< std::string > m = LatticePluginRegistration::instance()->models();
    int n = 0;
    for (std::list< std::string >::iterator it = m.begin(); it != m.end(); ++it) {
        ++n;
        std::cout << n << "! ";
        std::cout << (*it).c_str() << std::endl;
    }

    latticeIdentifier_ = model;
    lattice = LatticePluginRegistration::instance()->getMakerByName( latticeIdentifier_ )(
        realSize_, realSize_, latticeSize_, latticeSize_ );

    lattice->setDiffusion( 0, 1. );
    lattice->clear();
    if ( parent != 0 ) {
        if ( QLatin1String( parent->metaObject()->className() ) == QLatin1String( "mainWin" ) ) {
            parent->statusBar()->clearMessage();
        }
    }
    //QCoreApplication::processEvents();
    /*
     ModelLattice::ComponentSystem c(2, lattice->fixpointV() );
     //lattice->setSpotAt(10, 10, c, 2.5, true);
     lattice->setDiffusion(0, 1.);

     lattice->setSpotAt(20, 20, 6, NoFluxBoundary);*/
    //Defect< GeneralComponentSystem > def( lattice->numberOfVariables() );


    //lat.setDiffusion( diff );
    lattice->setSpotAtComponent( 10, 10, 2, 2, 0, true );
    /*
     for (int i=0; i< lattice->latticeSizeX(); ++i)
     for (int j=0; j< lattice->latticeSizeY(); ++j) {
     if (i < lattice->latticeSizeX() / 2)
     lattice->setComponentAt(1, 0.8, i, j);
     if (j > (10 + lattice->latticeSizeY()/2 ) )
     lattice->setComponentAt(0, 0.8, i, j);
     }
     */
    /*Defect< GeneralComponentSystem > def;
     def.value.resize( lattice->numberOfVariables() );
     SurfacePoint s = { 60, 100 };
     def.centre = s;
     def.radius = 30;
     def.boundaryCondition = FixedBoundary;
     lattice->addDefect( def );
     defectsList << def;

     def.radius = 33;
     def.boundaryCondition = NoReactionBoundary;
     lattice->addDefect( def );
     defectsList << def;

     s.y = 80;
     def.centre = s;
     def.radius = 30;
     def.boundaryCondition = NoReactionBoundary;
     lattice->addDefect( def );
     defectsList << def;

     def.centre.x = 100;
     def.radius = 15;
     def.boundaryCondition = FixedValueBoundary;
     def.value[ 0 ] = 0.;
     //def.value[1] = -1;//lattice->fixpoint()[1];
     lattice->addDefect( def );
     defectsList << def;

     def.centre.x = 40;
     def.centre.y = 40;
     def.radius = 10;
     def.boundaryCondition = NoFluxBoundary;
     lattice->addDefect( def );
     defectsList << def;
     */
    //lattice->setSpotAt(40, 40, 10, NoFluxBoundary);

    //  lattice->setSpotAt(_realSize / 2, _realSize / 2, 31.9965, lattice->fixpointV(), 0.3, true);

    //lattice->setUGaussAt(_realSize / 2, _realSize / 2, 11.9965, 0.2);
    //lattice->setSpotAt(2, 2, 4860., lattice->fixpointV(), .2, true);
    //lattice->setUGaussAt(2., 2., 10, .2);


    //lattice->setCentre(2., 1., 1., true, 1);
    /*
     lattice->setSpotAt(80, 80, 2., 1., 5, true);
     lattice->setSpotAt(40, 80, 2., 1., 4, true);
     lattice->setSpotAt(70, 50, 2., 1., 3, true);
     lattice->setSpotAt(30, 50, 2., 1., 2, false);
     lattice->setSpotAt(80, 20, 2., 1., 1.5, false);
     lattice->setSpotAt(40, 20, 2., 1., 1, false);
     lattice->setSpotAt(10, 20, 2., 1., 0.5, true);
     */

    //  std::cout << ":" << lattice->getAt(10, 10).getU() << " " << std::flush;


    setUpTabs();
    setUpColorMap();
    setUpModelProperties();
    setUpBoundaryConditionsSelector();
    setUpSlices();
    setTitle();
}

void Waveprogram2DPlot::killField()
{
    removeTabs();
    if ( lattice && !latticeIdentifier_.empty() ) {
        LatticePluginRegistration::instance()->getDestroyerByName( latticeIdentifier_ )( lattice );
        lattice = 0;
        latticeIdentifier_.clear();
    }
    setTitle();
}

void Waveprogram2DPlot::loopStart()
{
    loopruns = true;
    loop();
}

void Waveprogram2DPlot::loopStop()
{
    loopruns = false;
    QTimer::singleShot( 10, this, SLOT(replot()));
}

void Waveprogram2DPlot::toggleStartStop()
{
    if ( loopruns == false ) {
        timer->start( updatePeriodTime_ );
        startStopButton->setText( "Stop" );
        actionStartStop->setText( "Stop" );
        this->loopStart();
    } else {
        timer->stop();
        startStopButton->setText( "Start" );
        actionStartStop->setText( "Start" );
        this->loopStop();
    }
}

void Waveprogram2DPlot::on_clearButton_clicked()
{
    lattice->clear();
    replot();
}

void Waveprogram2DPlot::on_actionSave_triggered()
{
    timer->stop();
    lattice->save();
    timer->start();
}

void Waveprogram2DPlot::on_actionRecall_triggered()
{
    timer->stop();
    //lattice->recall();
    QString fileName = QFileDialog::getOpenFileName( this, tr( "Open Dump" ), "." );
    //std::cout << fileName;
    lattice->recall( fileName.toStdString() );
    timer->start();
}

void Waveprogram2DPlot::on_actionDump_triggered()
{
    std::ofstream ofile;
    ofile.open( "dump" );
    lattice->dump( ofile, 1 );
    ofile.close();
}

void Waveprogram2DPlot::on_actionUndump_triggered()
{
    QString fileName = QFileDialog::getOpenFileName( this, tr( "Open Dump" ), "." );
    //std::cout << fileName;
    std::ifstream ifile;
    ifile.open( fileName.toLatin1() );
    lattice->undump( ifile, 1 );
    ifile.close();
}

/**
 * Schreibt den aktuellen Plot in einem Matlab-lesbaren Format in die Zwischenablage.
 */
void Waveprogram2DPlot::on_actionCopy_to_Clipboard_triggered()
{
    int component = plotTabWidget->currentIndex();
    component = ( component < lattice->numberOfVariables() ) ? component : 0;

    QString text;
    text += QString( "[" );
    for (int j = 0; j < lattice->latticeSizeY(); ++j) {
        text += QString( "[" );
        for (int i = 0; i < lattice->latticeSizeX(); ++i) {
            double value = lattice->getComponentAt( component, i, j );
            text += QString( "%1" ).arg( value, 0, 'f', 4 );
            if ( i != lattice->latticeSizeX() - 1)
                text += QString( ", " );
        }
        text += QString( "]" );
        if ( j != lattice->latticeSizeY() - 1)
            text += QString( "; " );
    }
    text += QString( "]" );
    clipboard->setText( text );
}

/**
 * Exportiert eine Zeitreihe in eine matlab-lesbare Datei
 */
void Waveprogram2DPlot::exportAsMatlabStructure(QString fileName, QString structureName, int timeIndex, bool append /*= true*/)
{
    QString text;
    for (int component = 0; component < lattice->numberOfVariables(); ++component) {
        text += QString("%1{%2,%3} = ").arg(structureName).arg(timeIndex).arg(component + 1);
        text += QString( "[" );

        double value = lattice->getComponentAt( component, 16, 16 );



                        text += QString( "%1" ).arg( value, 0, 'f', 4 );
                        text += QString( "];\n" );
        /*
        for (int j = 0; j < lattice->latticeSizeY(); ++j) {
            text += QString( "[" );
            for (int i = 0; i < lattice->latticeSizeX(); ++i) {
                double value = lattice->getComponentAt( component, i, j );

                if ( i!= 13 && j!=13) continue;

                text += QString( "%1" ).arg( value, 0, 'f', 4 );
                if ( i != lattice->latticeSizeX() - 1 )
                    text += QString( ", " );
            }
            text += QString( "]" );
            if ( j != lattice->latticeSizeY() - 1 )
                text += QString( "; " );
        }
        text += QString( "];\n" );
        */
    }
    QFile data(fileName);
    if ( append ? data.open(QFile::Append) : data.open(QFile::WriteOnly | QFile::Truncate)  ) {
        QTextStream out(&data);
        out << text;
    }
}

void Waveprogram2DPlot::on_actionSave_as_Png_triggered()
{
    QString fileName = QFileDialog::getSaveFileName( this, tr( "Image File" ), "img.png", tr(
        "Images (*.png)" ) );
    /*
     QImage image = QImage(graph->width(), graph->height(), QImage::Format_ARGB32);
     for (int i = 0; i <= lattice->sizeX() + 1; ++i) {
     for (int j = 0; j <= lattice->sizeY() + 1; ++j) {
     image.setPixel(i, j, QColor("white").rgb() );
     }
     }
     graph->print(image);
     */

    for (uint component = 0; component < lattice->numberOfVariables(); ++component) {
        QImage image = QImage(
            lattice->latticeSizeX(), lattice->latticeSizeY(), QImage::Format_ARGB32 );

        for (int i = 0; i < lattice->latticeSizeX(); ++i) {
            for (int j = 0; j < lattice->latticeSizeY(); ++j) {
                image.setPixel(
                    i, j, colorMap.color( QwtDoubleInterval( -2.2, 2.5 ), lattice->getComponentAt(
                        component, i, j ) ).rgb() );
            }
        }
        image.mirrored( false, true ).save( fileName + QString( ".%1.png" ).arg( component ), "PNG" );

    }

    //std::cout << (fileName.append("-inh"));
}

void Waveprogram2DPlot::on_actionLoad_from_Png_triggered()
{
    QString fileName = QFileDialog::getOpenFileName( this, tr( "Image File" ), ".", tr(
        "Images (*.png)" ) );
    for (uint component = 0; component < 1 /*lattice->numberOfVariables()*/; ++component) {
        QImage image = QImage( fileName ).mirrored( true, false );
        //QImage image_inhib = QImage(fileName.append("-inh.png")).mirrored(true, false);

        QHash< QRgb, double > reversableMap;
        for (int i = 0; i < 100000; ++i) {
            double d = -2.5 + (2.5 - (-2.5)) / 100000. * i;
            QRgb rgbval = plotViewVector_[ 0 ]->spectrogram_->colorMap().color(
                plotViewVector_[ 0 ]->spectrogram_->data().range(), d ).rgb();
            reversableMap.insert( rgbval, d );
        }
        /*QHash<QRgb, double> reversableMap_inhib;
         for(int i=0; i<100000; ++i) {
         double d = -2.5 + (3.5 - (-2.5))/100000. * i;
         QRgb rgbval = d_spectrogram->colorMap().color(QwtDoubleInterval (-2.5,3.5), d).rgb();
         reversableMap_inhib.insert(rgbval, d );
         }
         */

        for (int i = 0; i < image.width(); ++i) {
            for (int j = 0; j < image.height(); ++j) {
                QRgb rgb = image.pixel( i, j );
                //QRgb rgb_inhib = image_inhib.pixel(i, j);
                //std::cout << rgb << std::flush;
                if ( reversableMap.value( rgb ) == 0 )
                    std::cout << rgb << " " << reversableMap.value( rgb ) << std::endl
                        << std::flush;
                lattice->setComponentAt( component, i, j, reversableMap.value( rgb ) );
                //lattice->setV(i,j, reversableMap.value(rgb_inhib) );
            }
        }
    }
    replot();
}

void Waveprogram2DPlot::on_actionSave_as_Movie_Pngs_triggered()
{
    if ( !movieQueue.empty() ) {
        movieQueue = QQueue< QString > ();
        actionSave_as_Movie_Pngs->setText( QString( "savetomultiple" ) );
        return;
    }
    QString fileName = QFileDialog::getSaveFileName( this, tr( "Image File" ), "movie.png", tr(
        "Images (*.png)" ) );

    for (int i = 1; i <= 999; ++i) {
        QString path = QFileInfo( fileName ).path();
        QString baseName = QFileInfo( fileName ).completeBaseName();
        QString suffix = QFileInfo( fileName ).suffix();
        if ( i < 10 ) {
            movieQueue.enqueue( QString( path.append( "/" ) + baseName.append( "00%1." ).arg( i )
                + suffix ) );
        } else if ( i < 100 ) {
            movieQueue.enqueue( QString( path.append( "/" ) + baseName.append( "0%1." ).arg( i )
                + suffix ) );
        } else {
            movieQueue.enqueue( QString( path.append( "/" ) + baseName.append( "%1." ).arg( i )
                + suffix ) );
        }
    }
    actionSave_as_Movie_Pngs->setText( QString( "stop" ).append( " (%1)" ).arg( movieQueue.size() ) );
}

void Waveprogram2DPlot::on_actionExport_as_Matlab_Structure_triggered()
{
    if ( matlabExportFile_.isEmpty() ) {
        matlabExportIndex_ = 1;
        matlabExportFile_ = QFileDialog::getSaveFileName( this, tr( "Matlab File" ), "matlabexport.m", tr(
        "Matlab Files (*.m)" ) );
        actionExport_as_Matlab_Structure->setText( "is Exporting" );
    } else {
        matlabExportFile_.clear();
        actionExport_as_Matlab_Structure->setText( "Export as Matlab Structure" );
    }
}

void Waveprogram2DPlot::savePng(QString filename)
{
    QImage image = QImage( lattice->latticeSizeX(), lattice->latticeSizeY(), QImage::Format_ARGB32 );
    uint save_component = 0;

    for (int i = 0; i < lattice->latticeSizeX(); ++i) {
        for (int j = 0; j < lattice->latticeSizeY(); ++j) {

            if ( plotViewVector_.size() > 0 ) {
                image.setPixel( i, j, plotViewVector_[ save_component ]->spectrogram_->colorMap().color(
                    plotViewVector_[ save_component ]->spectrogram_->data().range(), lattice->getComponentAt(
                        save_component, i, j ) ).rgb() );
            } else {
                image.setPixel( i, j, colorMap.color(
                    QwtDoubleInterval( -2.2, 2.5 ), lattice->getComponentAt( save_component, i, j ) ).rgb() );
            }
        }
    }
    image.mirrored( false, true ).save( filename, "PNG" );
}

void Waveprogram2DPlot::adaptationModeCheckBox_clicked(bool b)
{
    adaptationMode( b );
}

/*
 void Waveprogram2DPlot::makeStableStart()
 {
 for (int j = 0; j < 10000; j++) {

 //        if (lattice->getAt(170,30).getX() > 0)
 //            lattice->copy(100,1,100,100,1,100);
 // if ((j % 400 == 0) && (lattice->epsilon > 0.08)) { lattice->epsilon += -0.0001; }
 if ( j % 100 == 0 ) {
 if ( lattice->currentWavesize() > wavesize_ ) {
 low = low + 0.01 * (high - low);
 } else {
 high = high - 0.01 * (high - low);
 }
 //      std::cout << "\nNew high: " << high << ", New low: " << low << ", Epsilon: " << lattice->epsilon() << "\n" << std::flush;
 }
 if ( lattice->currentWavesize() < wavesize_ ) {
 if ( latticeParameters.value( "gamma" ) )
 latticeParameters.value( "gamma" )->set( low );
 // std::cout << " g set to " << Model::g << " " << std::endl << std::flush;
 } else {
 if ( latticeParameters.value( "gamma" ) )
 latticeParameters.value( "gamma" )->set( high );
 // std::cout << " g set to " << Model::g << " " << std::endl << std::flush;
 }
 step();
 if ( j % 300 == 0 )
 replot();
 }
 }
 */

void Waveprogram2DPlot::adaptationMode(bool b)
{
    adaptationMode_ = b;
}

bool Waveprogram2DPlot::adaptationMode()
{
    return adaptationMode_;
}

/*
 void Waveprogram2DPlot::adaptValues()
 {
 if ( !latticeParameters.value( "gamma" ) )
 return;
 static int j = 0;
 if ( j % 10 == 0 ) {
 std::cout << lattice->currentWavesize();
 if ( lattice->currentWavesize() > wavesize_ ) {
 low = low + gammaspacing * (high - low);
 gamma_lowValue->setValue( low );
 } else {
 high = high - gammaspacing * (high - low);
 gamma_highValue->setValue( high );
 }
 //!    SurfacePoint po = lattice->centreOfExcitation();
 //!    std::cout << "\nNew high: " << high << ", New low: " << low << ", Epsilon: " << lattice->epsilon() << " " << po.x <<"\n" << std::flush;
 }
 if ( lattice->currentWavesize() < wavesize_ ) {
 latticeParameters.value( "gamma" )->set( low );
 // std::cout << " g set to " << Model::g << " " << std::endl << std::flush;
 } else {
 latticeParameters.value( "gamma" )->set( high );
 // std::cout << " g set to " << Model::g << " " << std::endl << std::flush;
 }
 j++;
 }
 */

void Waveprogram2DPlot::loop()
{
    while (loopruns) {
        //for(int j=0; j < 5; j++) {
        {

            if ( adaptationMode() ) {
                lattice->adaptParameters();
                emit updateParameters();
            }
            this->step( 5 );
            /*
             std::cout << lattice->getMax( 0 ) << lattice->getMax( 1 ) << std::endl;
             this->step(  );
             std::cout << lattice->getMax( 0 ) << lattice->getMax( 1 ) << std::endl;
             this->step(  );
             std::cout << lattice->getMax( 0 ) << lattice->getMax( 1 ) << std::endl;
             this->step(  );
             std::cout << lattice->getMax( 0 ) << lattice->getMax( 1 ) << std::endl;
             this->step(  );
             std::cout << lattice->getMax( 0 ) << lattice->getMax( 1 ) << std::endl;
             */
            //std::cout << lattice->time() << std::endl;

            simulationTimeLabel->setNum( lattice->time() );

            clusterNumberLabel->setNum( lattice->numberOfClusters() );
        }
        if ( actionResize_program_window_to_force_update->isChecked() ) {
            static int resizeCount = 0;
            ++resizeCount;
            if ( resizeCount == 2 ) {
                this->resize( this->size().rwidth(), this->size().rheight() + 1 );
            }
            if ( resizeCount == 4 ) {
                this->resize( this->size().rwidth(), this->size().rheight() - 1 );
                resizeCount = 0;
            }
        }
        //this->resize(this->size().rwidth(), this->size().rheight() + (( (int) (lattice->time() * 10) % 2)*2 - 1));
        //this->replot();
        static int count = 0;
        count++;
        //if (count % 2 == 0)
        QCoreApplication::processEvents();

        /*
         static QList<double> buffer;

         if (buffer.size() == 10) {
         buffer.pop_back();
         }
         buffer.push_front( lattice->getMaxU() );
         std::cout << buffer.front() << "\t";
         double deriv1, deriv2;
         if (buffer.size() > 4) {
         deriv1 = (3.*buffer[0] - 4.*buffer[1] + 1.*buffer[2]) / (2.0 * lattice->timeStep() );
         deriv2 = (2*buffer[0] - 5*buffer[1] + 4*buffer[2] -1* buffer[3] ) / (lattice->timeStep() * lattice->timeStep() );
         std::cout << deriv1 << "\t" << deriv2;
         }
         std::cout << "\n";
         */
        /*
         std::pair<double, double> curv = lattice->getCurvature();
         std::cout << "Curv " << curv.first << " Error: " << curv.second << std::endl;
         */

        if ( !movieQueue.empty() ) {
            savePng( movieQueue.head() );
            movieQueue.dequeue();
            actionSave_as_Movie_Pngs->setText( QString( "stop" ).append( " (%1)" ).arg(
                movieQueue.size() ) );
            if ( movieQueue.empty() ) {
                actionSave_as_Movie_Pngs->setText( QString( "savetomultiple" ) );
            }
        }

        if ( !matlabExportFile_.isEmpty() ) {
            QString modelName( lattice->modelName().c_str() );
            modelName = modelName.remove(QRegExp("[^A-Za-z]" ) );
            exportAsMatlabStructure( matlabExportFile_, modelName, matlabExportIndex_, true);
            ++matlabExportIndex_;
        }


/*        QScriptEngine engine;

             QScriptValue objectValue = engine.newQObject(this);
             engine.globalObject().setProperty("myObject", objectValue);
             qDebug() << engine.evaluate( "myObject.changeDiffusion( 10 ) " ).toNumber();

*/



    }
}

void Waveprogram2DPlot::step()
{
    lattice->step();
}

void Waveprogram2DPlot::step(int num)
{
    lattice->step( num );
}
void Waveprogram2DPlot::on_numInitialPushButton_clicked()
{
    int i = numInitialComboBox->currentIndex();
    lattice->toInitial( i );
    replot();
}

void Waveprogram2DPlot::on_actionAbout_triggered()
{
    std::stringstream s;
    lattice->status( s );
    QString status = QString( "<pre>" ) + QString::fromUtf8( s.str().c_str() ) + QString( "</pre>" );
    QMessageBox::about( this, "Wellenprogramm", status );
}

void Waveprogram2DPlot::on_actionShow_Slice_triggered(bool b)
{
    sliceWidget->setVisible( b );
}


void Waveprogram2DPlot::readSettings()
{
    QSettings settings;
    settings.beginGroup( "2dPlot" );
    restoreGeometry( settings.value( "geometry" ).toByteArray() );
    //resize(settings.value("size", QSize(400, 400)).toSize());
    //move(settings.value("pos", QPoint(200, 200)).toPoint());

    settings.endGroup();

}

void Waveprogram2DPlot::writeSettings()
{
    QSettings settings;
    settings.beginGroup( "2dPlot" );
    //    settings.setValue("size", size());
    //    settings.setValue("pos", pos());
    settings.setValue( "geometry", saveGeometry() );

    settings.endGroup();

}
