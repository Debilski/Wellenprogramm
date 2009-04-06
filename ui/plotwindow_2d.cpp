// #include <iostream>

#include "plotwindow_2d.h"

#include <fstream>
#include <cmath>

#include <QtGui>
#include <QtCore>

#include "plot_layer.h"

#include "spectrogram_data.h"

class Waveprogram2DPlot::PrivateData {
public:
    QList< QwtPlotMarker* > markers;
    QList< QwtPlotCurve* > curves;
};

void Waveprogram2DPlot::setTitle()
{
    if ( latticeController_->isValid() ) {
        setWindowTitle( QString::fromUtf8(
            "‘%1’: Simulate a field size of %2×%2 on a lattice of %3×%3 " ).arg(
            latticeController_->lattice()->modelName().c_str() ).arg(
            latticeController_->lattice()->sizeX() ).arg(
            latticeController_->lattice()->latticeSizeX() ) );
        if ( latticeController_->lattice()->modelInformation().empty() ) {
            informationGroupBox->hide();
            infoTextLabel->clear();
        } else {
            informationGroupBox->show();
            infoTextLabel->setText( QString::fromUtf8(
                latticeController_->lattice()->modelInformation().c_str() ) );
        }
    } else {
        setWindowTitle( QString::fromUtf8( "Wellenprogramm: No Model loaded" ) );
        informationGroupBox->hide();
        infoTextLabel->clear();
    }
}

Waveprogram2DPlot::Waveprogram2DPlot(QMainWindow * parent, int realSize, int latticeSize) :
    QMainWindow( parent ), realSize_( realSize ), latticeSize_( latticeSize ), parent( parent )
{
    d_data = new PrivateData;
    std::cout << realSize_ << "/" << latticeSize_ << std::endl;

    // LatticeController vllt ohne Zeiger, aber auf jeden Fall besser einbauen als so!
    lc_.reset( new LatticeController() );
    latticeController_ = lc_.get();
    connect( latticeController_, SIGNAL( changed() ), this, SLOT( replot() ) );
    connect( latticeController_, SIGNAL( changed() ), this, SLOT( updateLabels() ) );

    colorMapMode = defaultColorMapMode;

    setupUi( this ); // this sets up GUI

    // setAttribute( Qt::WA_DeleteOnClose );
    //    sliceWidget->setVisible( false );

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

    setUpColorSchemeMenu();

    QString lastUsedModel = config.option( "last_model" ).value().toString();

    initField( realSize, latticeSize, qPrintable(lastUsedModel) );

    boundaryConditionsComboBox->setCurrentIndex( latticeController_->lattice()->boundaryCondition() );

    correlationSpinBox->setValue( latticeController_->lattice()->noiseCorrelation() );
    timestepSpinBox->setValue( latticeController_->lattice()->timeStep() );

    midpoint_sizeValue->setValue( 0 );

    showClusterIds_ = false;

    //QwtValueList contourLevels;

    /*for (double level = 0.5; level < 10.0; level += 1.0)
     contourLevels += level;
     d_spectrogram->setContourLevels(contourLevels);
     d_curvature_spectrogram->setContourLevels(contourLevels);
     */

    updatePeriodTime_ = 100;
    setUpUpdatePeriodMenu();
    defectsEditor = 0;

    // menuWindow->addAction( simulationWidget->toggleViewAction() );

    parameterWidget->toggleViewAction()->setText( "Model Properties" );
    menuDock_Windows->addAction( parameterWidget->toggleViewAction() );
    adaptationParameterWidget->toggleViewAction()->setText( "Adaptation Properties" );
    menuDock_Windows->addAction( adaptationParameterWidget->toggleViewAction() );
    menuDock_Windows->addAction( extrasWidget->toggleViewAction() );

    setUnifiedTitleAndToolBarOnMac( true );

    setUpDockWindows();

    this->show();

    replot();

    timer = new QTimer( this );
    connect( timer, SIGNAL(timeout()), this, SLOT(replot()) );

    clipboard = QApplication::clipboard();

    matlabExportFile_ = QString();
    matlabExportIndex_ = 0;

    readSettings();

    connect( simulationTimeLabel, SIGNAL( reset() ), this, SLOT( resetTime()) );

}

void Waveprogram2DPlot::resetTime()
{
    latticeController_->lattice()->setTime( 0 );
    simulationTimeLabel->setNum( latticeController_->lattice()->time() );
}

void Waveprogram2DPlot::closeEvent(QCloseEvent * event)
{
    loopStop();
    writeSettings();
    writeParameterSets();
    event->accept();
}

void Waveprogram2DPlot::on_actionClose_Window_triggered()
{
    close();
}

void Waveprogram2DPlot::on_actionEdit_Script_triggered()
{
    scriptEditor = new ScriptEditor( this, latticeController_ );
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
    latticeController_->lattice()->removeDefects();
    for (int i = 0; i < defectsList.size(); ++i) {
        latticeController_->lattice()->addDefect( defectsList.at( i ) );
    }
}

void Waveprogram2DPlot::setUpColorSchemeMenu()
{
    changeColorSchemeGroup = new QActionGroup( this );
    foreach ( ColorMaps::T_identifier pair, colorMaps_.colorMapNames())
        {
            QAction* action = new QAction( pair.second, changeColorSchemeGroup );
            action->setCheckable( true );
            action->setData( pair.first );
            if ( pair.first == ColorMaps::standardColorMap ) {
                action->setChecked( true );
            }
            menuSelect_Color_Scheme->addAction( action );
            changeColorSchemeGroup->addAction( action );
        }
    connect(
        changeColorSchemeGroup, SIGNAL( triggered( QAction* ) ), this,
        SLOT( updateColorScheme( QAction* )) );

    menuSelect_Color_Scheme->addSeparator();
    menuSelect_Color_Scheme->addAction( actionDefault_Color_Scheme );
    menuSelect_Color_Scheme->addAction( actionAdaptive_Scheme );
    menuSelect_Color_Scheme->addAction( actionDelayed_Adaptive_Scheme );
    changeColorSchemeModeGroup = new QActionGroup( this );
    changeColorSchemeModeGroup->addAction( actionDefault_Color_Scheme );
    changeColorSchemeModeGroup->addAction( actionAdaptive_Scheme );
    changeColorSchemeModeGroup->addAction( actionDelayed_Adaptive_Scheme );
    actionDefault_Color_Scheme->setChecked( true );
    connect(
        changeColorSchemeModeGroup, SIGNAL( triggered( QAction* ) ), this,
        SLOT( updateColorSchemeMode( QAction* )) );
}

void Waveprogram2DPlot::updateColorSchemeMode(QAction* a)
{
    if ( a == actionDefault_Color_Scheme )
        colorMapMode = defaultColorMapMode;
    if ( a == actionAdaptive_Scheme )
        colorMapMode = adaptiveColorMapMode;
    if ( a == actionDelayed_Adaptive_Scheme )
        colorMapMode = delayedAdaptiveColorMapMode;
    emit
    colorMapModeChanged( colorMapMode );
    emit colorMapChanged( colorMaps_.getColorMap(), colorMapMode );
}

void Waveprogram2DPlot::updateColorScheme(QAction* a)
{
    // blöder Hack
    emit colorMapChanged( colorMaps_.getColorMap(
        static_cast< ColorMaps::ColorMapTypes > ( a->data().value< int > () ) ) );
    emit colorMapChanged( colorMaps_.getColorMap(
        static_cast< ColorMaps::ColorMapTypes > ( a->data().value< int > () ) ), colorMapMode );
}

void Waveprogram2DPlot::setUpUpdatePeriodMenu()
{
    updatePeriodGroup = new QActionGroup( this );
    updatePeriodGroup->addAction( action100_ms );
    updatePeriodGroup->addAction( action1_s );
    updatePeriodGroup->addAction( action25_ms );
    updatePeriodGroup->addAction( action500_ms );
    updatePeriodGroup->addAction( action5_s );
    action100_ms->setChecked( true );
    connect(
        updatePeriodGroup, SIGNAL( triggered( QAction* ) ), this,
        SLOT( updateUpdatePeriod( QAction* ) ) );
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

#include "parameter_dock_widget.h"

void Waveprogram2DPlot::setUpDockWindows()
{
    ParameterDockWidget* p = new ParameterDockWidget( this );
    p->setFloating( true );
    p->show();
}

void Waveprogram2DPlot::setUpColorMap()
{
    emit colorMapChanged( colorMaps_.getColorMap() );
    emit
    colorMapModeChanged( colorMapMode );
    emit colorMapChanged( colorMaps_.getColorMap(), colorMapMode );
}

void Waveprogram2DPlot::reorderTabs()
{
    plotTabWidget->setUpdatesEnabled( false );
    while (plotTabWidget->count() > 0) {
        plotTabWidget->removeTab( plotTabWidget->count() - 1 );
    }

    for (uint i = 0; i < plotViewVector_.size(); ++i) {
        plotTabWidget->addTab( plotViewVector_[ i ], plotViewVector_[ i ]->name() );
    }

    plotTabWidget->setUpdatesEnabled( true );
}

void Waveprogram2DPlot::setUpViews()
{
    //    disconnect( this, SIGNAL( replotTab() ), 0, 0 );
    //    disconnect( this, SIGNAL( colorMapChanged( const QwtColorMap& )), 0, 0 );

    for (uint i = 0; i < plotViewVector_.size(); ++i) {
        PlotView* w = plotViewVector_[ i ];
        disconnect( this, 0, w, 0 );
        w->deleteLater();
    }
    plotViewVector_.clear();

    for (uint component = 0; component < latticeController_->lattice()->numberOfVariables(); ++component)
    {
        QString label;
        if ( !latticeController_->lattice()->componentInfos[ component ].physicalQuantity().empty() )
        {
            label = QString( "%1 / %2" );
            label
                = label.arg(
                    latticeController_->lattice()->componentInfos[ component ].physicalQuantity().c_str(),
                    latticeController_->lattice()->componentInfos[ component ].physicalUnitSymbol().c_str() );
        } else {
            label = QString( "Intensity" );
        }
        //PlotView* tab = new PlotView(
        //    SpectrogramData( latticeController_, component, this ), colorMaps_.getColorMap(), component, label, plotTabWidget );


        PlotLayer* layer = new PlotLayer( latticeController_ );
        layer->spectrogram()->setData( SpectrogramData( latticeController_, component, layer ) );
        layer->spectrogram()->setColorMap( colorMaps_.getColorMap() );
        layer->component = component;

        PlotStack plotStack;
        plotStack.append( layer );

        PlotView* view = new PlotView( plotStack, label, plotTabWidget );

        plotViewVector_ << view;

        connect( this, SIGNAL( replotTab() ), view, SLOT( replot() ) );

        connect(
            this, SIGNAL( colorMapChanged( const QwtColorMap&, ColorMapAdaptationModes )), view,
            SLOT( setColorMap( const QwtColorMap&, ColorMapAdaptationModes )) );
        //! Sendet an potenziell zu viele Tabs…
        connect( plotTabWidget, SIGNAL( currentChanged(int) ), view, SLOT( replot(int) ) );

        connect(
            view, SIGNAL( selected(const uint&, const QPointF& )), this,
            SLOT(paint(const uint&, const QPointF&)) );
        QString name = QString( "%1 (%2)" ).arg(
            QString::fromStdString(
                latticeController_->lattice()->componentInfos[ component ].name() ),
            QString::fromStdString(
                latticeController_->lattice()->componentInfos[ component ].shortName() ) );
        view->setName( name );
    }

    // Sollte irgendwie so ausgearbeitet werden, dass es im Modell definierbar ist.
    if ( QString::fromStdString( latticeController_->lattice()->modelName() ).contains(
        "FHNK", Qt::CaseInsensitive ) )
    {
        QString name = "View";
        QString label;
        PlotLayer* layer;
        PlotStack plotStack;

        LinearTransparentColorMap colorMap = LinearTransparentColorMap(
            QColor( 0, 0, 189 ), QColor( 132, 0, 0 ) );
        double pos = 1.0 / 13.0 * 1.0;
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

        {
            uint component = 2;
            layer = new PlotLayer( latticeController_ );
            layer->spectrogram()->setData( SpectrogramData( latticeController_, component, layer ) );
            layer->spectrogram()->setColorMap( colorMap );
            layer->component = component;

            connect(
                this, SIGNAL( colorMapChanged( const QwtColorMap& )), layer,
                SLOT( setColorMap( const QwtColorMap& )) );

        }
        plotStack.append( layer );

        colorMap = LinearTransparentColorMap( Qt::black, Qt::white );
        {
            uint component = 0;
            layer = new PlotLayer( latticeController_ );
            layer->spectrogram()->setData( SpectrogramData( latticeController_, component, layer ) );
            layer->spectrogram()->setColorMap( colorMap );
            layer->component = component;
        }
        plotStack.append( layer );

        PlotView* view = new PlotView( plotStack, label, plotTabWidget );

        view->setName( "View" );
        plotViewVector_ << view;

        connect( this, SIGNAL( replotTab() ), view, SLOT( replot() ) );

        connect(
            this, SIGNAL( colorMapModeChanged( ColorMapAdaptationModes )), view,
            SLOT( setColorMapMode( ColorMapAdaptationModes )) );
        connect( this, SIGNAL( colorMapChanged( const QwtColorMap& )), view, SLOT( replot() ) );

        //! Sendet an potenziell zu viele Tabs…
        connect( plotTabWidget, SIGNAL( currentChanged(int) ), view, SLOT( replot(int) ) );

        //        connect(
        //            view, SIGNAL( selected(const uint&, const QPointF& )), latticeController_,
        //            SLOT(setToFixpoint(const uint&, const QPointF&)) );

    }
}

void Waveprogram2DPlot::paint(const uint& component, const QPointF& point)
{
    if ( paintDockWidget->isVisible() ) {
        if ( paintFixpointCheckbox->isChecked() ) {
            latticeController_->setToFixpoint( component, point, paintSizeSpinBox->value() );
        } else {
            latticeController_->setComponentAt(
                component, point, paintSizeSpinBox->value(), paintValueSpinBox->value() );
        }
    }
}

void Waveprogram2DPlot::setUpDiffusion()
{
    for (uint component = 0; component < latticeController_->lattice()->numberOfVariables(); ++component)
    {
        QDoubleSpinBox* diffusionBox = new QDoubleSpinBox( parameterWidgetContents );

        //! Bisschen unsauber…
        diffusionBox->setProperty( "component", QVariant( component ) );
        diffusionBox->setDecimals( 3 );
        diffusionBox->setMaximum( 1000 );
        diffusionBox->setValue( latticeController_->lattice()->getDiffusion( component ) );
        QString name = "Diffusion " + QString::fromStdString(
            latticeController_->lattice()->componentInfos[ component ].shortName() );
        QLabel* label = new QLabel( name, parameterWidgetContents );
        parameterWidgetFormLayout->addRow( label, diffusionBox );
        connect(
            diffusionBox, SIGNAL( valueChanged(const double&) ), this,
            SLOT( changeDiffusion(const double&) ) );
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
    latticeController_->lattice()->setDiffusion( component, value );
}

void Waveprogram2DPlot::setUpModelProperties()
{
    if ( parameterWidgetContents )
        delete parameterWidgetContents;
    //  if (parameterWidgetFormLayout)
    //    delete parameterWidgetFormLayout;
    if ( latticeController_->lattice() ) {
        parameterWidget->setWindowTitle( QString( "%1 Properties" ).arg(
            latticeController_->lattice()->modelName().c_str() ) );
    }
    parameterWidget->setUpdatesEnabled( false );

    parameterWidgetContents = new QWidget();
    parameterWidgetContents->setObjectName( QString::fromUtf8( "parameterWidgetContents" ) );
    verticalLayout = new QVBoxLayout( parameterWidgetContents );
    verticalLayout->setObjectName( QString::fromUtf8( "verticalLayout" ) );
    parameterWidgetFormLayout = new QFormLayout();
    parameterWidgetFormLayout->setObjectName( QString::fromUtf8( "parameterWidgetFormLayout" ) );

    verticalLayout->addLayout( parameterWidgetFormLayout );
    parameterWidgetContents->setSizePolicy( QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum) );

    parameterWidget->setWidget( parameterWidgetContents );

    if ( adaptationParameterWidgetContents )
        delete adaptationParameterWidgetContents;

    adaptationParameterWidget->setUpdatesEnabled( false );
    //  if (parameterWidgetFormLayout)
    //    delete parameterWidgetFormLayout;
    if ( latticeController_->lattice() ) {
        adaptationParameterWidget->setWindowTitle( QString( "%1 Adaptation Properties" ).arg(
            latticeController_->lattice()->modelName().c_str() ) );
    }
    adaptationParameterWidgetContents = new QWidget();
    adaptationParameterWidgetContents->setSizePolicy( QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum) );
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
    connect(
        adaptationModeCheckBox, SIGNAL( clicked(bool) ), this,
        SLOT ( adaptationModeCheckBox_clicked(bool) ) );
    adaptationParameterWidgetFormLayout->addRow( label, adaptationModeCheckBox );

    setUpDiffusion();
    setUpParameters();

    setUpAdaptationParameters();

    parameterWidget->setUpdatesEnabled( true );
    adaptationParameterWidget->setUpdatesEnabled( true );
}

void Waveprogram2DPlot::setUpParameters()
{
    std::list< Parameter< double >* > params = latticeController_->lattice()->parameters();
    std::list< Parameter< double >* >::iterator param;

    latticeParameters.clear();
    for (param = params.begin(); param != params.end(); ++param) {
        QString paramName = QString::fromUtf8( (*param)->name.c_str() );
        latticeParameters.insert( paramName, *param );
        std::cout << paramName.toStdString() << ":" << " " << *param;

        ParameterSpinBox* parameterBox = new ParameterSpinBox( *param, parameterWidgetContents );

        QLabel* label = new QLabel( paramName, parameterWidgetContents );
        parameterWidgetFormLayout->addRow( label, parameterBox );
        connect(
            parameterBox, SIGNAL( valueChanged(const double&) ), this,
            SLOT( changeParameter(const double&) ) );
        connect( this, SIGNAL( updateParameters() ), parameterBox, SLOT ( updateValue() ) );
    }
}

void Waveprogram2DPlot::setUpAdaptationParameters()
{
    std::list< Parameter< double >* > params =
        latticeController_->lattice()->adaptationParameters();
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
        connect(
            parameterBox, SIGNAL( valueChanged(const double&) ), this,
            SLOT( changeParameter(const double&) ) );

        connect( this, SIGNAL( updateParameters() ), parameterBox, SLOT ( updateValue() ) );
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
        latticeController_->lattice()->setBoundaryCondition( FixedBoundary );
    if ( i == 1 )
        latticeController_->lattice()->setBoundaryCondition( PeriodicBoundary );
    if ( i == 2 )
        latticeController_->lattice()->setBoundaryCondition( NoFluxBoundary );
}

void Waveprogram2DPlot::on_noiseSpinBox_valueChanged(double d)
{
    latticeController_->lattice()->setNoiseIntensity( d );
}

void Waveprogram2DPlot::on_correlationSpinBox_valueChanged(int d)
{
    latticeController_->lattice()->setNoiseCorrelation( d );
}

void Waveprogram2DPlot::on_timestepSpinBox_valueChanged(double d)
{
    latticeController_->lattice()->setTimeStep( d );
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
    latticeController_->lattice()->addDefect( midpointDefect );

    // double s = 0;//! = latticeController_->lattice()->getSpotSize(d, true);
    // QString text = QString( "Area: %1 FE" ).arg( s );
    // statusBar()->showMessage( text );
}

QList< QwtPlotMarker* > Waveprogram2DPlot::plotMarkers()
{
    return d_data->markers;
}

QList< QwtPlotCurve* > Waveprogram2DPlot::plotCurves()
{
    return d_data->curves;
}

void Waveprogram2DPlot::updatePlotAnnotations()
{
    qDeleteAll( d_data->markers );
    d_data->markers.clear();
    //    d_data->markers.reserve( latticeController_->lattice()->numberOfClusters() );
    std::vector< Cluster > clusterVector = latticeController_->lattice()->getClusters();

    // Marker
    for (uint i = 0; i < clusterVector.size(); ++i) {
        QwtPlotMarker* marker = new QwtPlotMarker;
        marker->setValue( clusterVector[ i ].position.x, clusterVector[ i ].position.y );
        QString label = "#%1\nSize: %2\nSpeed: %3 (est.)";
        label = label.arg( clusterVector[ i ].clusterId ).arg( clusterVector[ i ].size );

        // Calculate speed
        if ( lastStepClusters.contains( clusterVector[ i ].clusterId ) ) {
            double len = latticeController_->lattice()->euklidianDistance(
                lastStepClusters[ clusterVector[ i ].clusterId ], clusterVector[ i ].position );
            double speed = len / (-lastClustersUpdateTime + latticeController_->lattice()->time());
            label = label.arg( speed );
        } else {
            label = label.arg( QString::fromUtf8( "–" ) );
        }

        marker->setLabel( label );

        d_data->markers << marker;
    }

    lastClustersUpdateTime = latticeController_->lattice()->time();

    // Curves

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
        temporaryBufferMap[ cId ].append( QPair< SurfacePoint, double > (
            sp, latticeController_->lattice()->time() ) );
    }

    typedef QMap< long int, QList< QPair< SurfacePoint, double > > > T_bufferMap;
    typedef QList< QPair< SurfacePoint, double > > T_listPairs;
    typedef QPair< SurfacePoint, double > T_pair;

    // Entfernt alles, was älter als tdiff ist
    const int tdiff = 5;
    for (T_bufferMap::Iterator i = temporaryBufferMap.begin(); i != temporaryBufferMap.end(); ++i) {
        for (T_listPairs::Iterator j = (*i).begin(); j != (*i).end(); ++j) {
            if ( (*j).second < latticeController_->lattice()->time() - tdiff ) {
                (*i).erase( j );
            }
        }
    }
    bufferMap = temporaryBufferMap;

    QBrush brush( Qt::blue );
    QPen pen( brush, 2, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin );

    qDeleteAll( d_data->curves );
    d_data->curves.clear();
    foreach( T_listPairs cb, bufferMap )
        {
            QwtArray< double > qv1, qv2;
            double lastx = -1;
            double lasty = -1;
            foreach( T_pair sp , cb )
                {

                    if ( lastx > 0 && (abs( lastx - sp.first.x ) > 25 || abs( lasty - sp.first.y )
                        > 25) )
                    {

                        QwtPlotCurve* curve = new QwtPlotCurve();
                        curve->setPen( pen );
                        curve->setData( qv1, qv2 );
                        d_data->curves << curve;
                        qv1.clear();
                        qv2.clear();
                    }
                    qv1 << sp.first.x;
                    qv2 << sp.first.y;
                    lastx = sp.first.x;
                    lasty = sp.first.y;
                }
            QwtPlotCurve* curve = new QwtPlotCurve();
            curve->setPen( pen );
            curve->setData( qv1, qv2 );

            d_data->curves << curve;
        }
}

void Waveprogram2DPlot::replot()
{

    if ( !latticeController_->lattice() )
        return;

    emit
    replotAllChildren();

    updatePlotAnnotations();

    // Check, if we have a PlotView in the current tab.
    if ( showClusterIds_ && QLatin1String(
        plotTabWidget->currentWidget()->metaObject()->className() ) == QLatin1String( "PlotView" ) )
    {
        PlotView* currentView = static_cast< PlotView* > ( plotTabWidget->currentWidget() );

        foreach( QwtPlotMarker* m, plotMarkers() )
                currentView->attachItem( m );
        foreach( QwtPlotCurve* c, plotCurves() )
                currentView->attachItem( c );

        emit
        replotTab();

    } else {
        emit replotTab();
    }

    waveSizeLabel->setNum( latticeController_->lattice()->currentWavesize() );
    if ( latticeParameters.value( "gamma" ) )
        gammaLabel->setNum( latticeParameters.value( "gamma" )->get() );
}

Waveprogram2DPlot::~Waveprogram2DPlot()
{
    killField();
    delete timer;
    delete d_data;
}

void Waveprogram2DPlot::setUpActions()
{
    QList< QAction * > availableActions = menuMode->actions();

    ModelAction * action;

    std::list< std::string > names = latticeController_->getModelNames();
    for (std::list< std::string >::const_iterator it = names.begin(); it != names.end(); ++it) {
        action = new ModelAction( QString::fromStdString( *it ), *it, this );
        menuMode->insertAction( availableActions.first(), action );
        connect(
            action, SIGNAL( modelTriggered(std::string) ), this, SLOT( changeModel(std::string) ) );
    }
}

void Waveprogram2DPlot::changeModel(std::string m)
{
    loopStop();
    emit
    modelClosed();

    killField();

    //removeTabs();
    initField( realSize_, latticeSize_, m );
    emit modelChanged();
}

void Waveprogram2DPlot::initField(int realSize, int latticeSize, std::string model)
{

    if ( parent != 0 ) {
        if ( QLatin1String( parent->metaObject()->className() ) == QLatin1String( "mainWin" ) ) {
            parent->statusBar()->showMessage( QString(
                "Creating a lattice of %1 x %1. This might take some time" ).arg( latticeSize_ ) );
        }
    }
    QCoreApplication::processEvents();

    latticeController_->load( model, realSize, realSize, latticeSize, latticeSize );

    config( "last_model" ).setValue( QVariant( QString::fromStdString( model ) ) );
    qDebug() << config( "last_model" ).value();
    config.write();

    latticeController_->lattice()->setDiffusion( 0, 1. );
    latticeController_->lattice()->clear();
    latticeController_->lattice()->toInitial( 0 );

    if ( parent != 0 ) {
        if ( QLatin1String( parent->metaObject()->className() ) == QLatin1String( "mainWin" ) ) {
            parent->statusBar()->clearMessage();
        }
    }

    setUpViews();
    reorderTabs();
    setUpColorMap();
    setUpModelProperties();
    setUpBoundaryConditionsSelector();
    setTitle();

    readParameterSets();
}

void Waveprogram2DPlot::killField()
{
    writeParameterSets();

    if ( latticeController_->lattice() && !latticeIdentifier_.empty() ) {
        //LatticePluginRegistration::instance()->getDestroyerByName( latticeIdentifier_ )( lattice );
        latticeController_->destroy();
        latticeIdentifier_.clear();
    }
    setTitle();
}

void Waveprogram2DPlot::loopStart()
{
    loopruns = true;
    // Damit während der Schleife nur die Shots ein Update bewirken.
    // Drinlassen bis zur Auslagerung.
    disconnect( latticeController_, SIGNAL( changed() ), this, SLOT( replot() ) );

    loop();
}

void Waveprogram2DPlot::loopStop()
{
    loopruns = false;
    QTimer::singleShot( 10, this, SLOT(replot()) );
    connect( latticeController_, SIGNAL( changed() ), this, SLOT( replot() ) );
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
    latticeController_->lattice()->clear();
    replot();
}

void Waveprogram2DPlot::on_actionSave_triggered()
{
    timer->stop();
    QString fileName = QFileDialog::getSaveFileName( this, tr( "Save to binary format" ), ".", tr(
        "Binary (*.bin)" ) );
    if ( !fileName.isEmpty() ) {
        latticeController_->lattice()->save( false, fileName.toStdString() );
    }
    timer->start();
}

void Waveprogram2DPlot::on_actionRecall_triggered()
{
    timer->stop();
    //latticeController_->lattice()->recall();
    QString fileName = QFileDialog::getOpenFileName(
        this, tr( "Load from binary format" ), ".", tr( "Binary (*.bin);; All (*)" ) );
    //std::cout << fileName;
    if ( !fileName.isEmpty() ) {
        latticeController_->lattice()->recall( fileName.toStdString() );
        replot();
    }
    timer->start();
}

void Waveprogram2DPlot::on_actionDump_triggered()
{
    QString fileName = QFileDialog::getSaveFileName( this, tr( "Save to text format" ), ".", tr(
        "Data (*.dat)" ) );
    if ( !fileName.isEmpty() ) {
        std::ofstream ofile;
        ofile.open( fileName.toLatin1() );
        latticeController_->lattice()->dump( ofile, 1 );
        ofile.close();
    }
}

void Waveprogram2DPlot::on_actionUndump_triggered()
{
    QString fileName = QFileDialog::getOpenFileName( this, tr( "Load from text format" ), ".", tr(
        "Data (*.dat);; All (*)" ) );

    if ( !fileName.isEmpty() ) {
        std::ifstream ifile;
        ifile.open( fileName.toLatin1() );
        latticeController_->lattice()->undump( ifile, 1 );
        ifile.close();
        replot();
    }
}

/**
 * Schreibt den aktuellen Plot in einem Matlab-lesbaren Format in die Zwischenablage.
 */
void Waveprogram2DPlot::on_actionCopy_to_Clipboard_triggered()
{
    int component = plotTabWidget->currentIndex();
    component = (component < latticeController_->lattice()->numberOfVariables()) ? component : 0;

    QString text;
    text += QString( "[" );
    for (int j = 0; j < latticeController_->lattice()->latticeSizeY(); ++j) {
        text += QString( "[" );
        for (int i = 0; i < latticeController_->lattice()->latticeSizeX(); ++i) {
            double value = latticeController_->lattice()->getComponentAt( component, i, j );
            text += QString( "%1" ).arg( value, 0, 'f', 4 );
            if ( i != latticeController_->lattice()->latticeSizeX() - 1 )
                text += QString( ", " );
        }
        text += QString( "]" );
        if ( j != latticeController_->lattice()->latticeSizeY() - 1 )
            text += QString( "; " );
    }
    text += QString( "]" );
    clipboard->setText( text );
}

/**
 * Exportiert eine Zeitreihe in eine matlab-lesbare Datei
 */
void Waveprogram2DPlot::exportAsMatlabStructure(QString fileName, QString structureName,
                                                int timeIndex, bool append /*= true*/)
{
    QString text;
    text += QString( "Time_%1{%2} = %3\n" ).arg( structureName ).arg( timeIndex ).arg(
        latticeController_->lattice()->time(), 0, 'f', 4 );
    for (int component = 0; component < latticeController_->lattice()->numberOfVariables(); ++component)
    {
        text += QString( "%1{%2,%3} = " ).arg( structureName ).arg( timeIndex ).arg( component + 1 );
        text += QString( "[" );

        double value = latticeController_->lattice()->getComponentAt( component, 16, 16 );
        text += QString( "%1, " ).arg( value, 0, 'f', 4 );
        value = latticeController_->lattice()->getComponentAt( component, 17, 16 );
        text += QString( "%1, " ).arg( value, 0, 'f', 4 );
        value = latticeController_->lattice()->getComponentAt( component, 18, 16 );
        text += QString( "%1, " ).arg( value, 0, 'f', 4 );

        value = latticeController_->lattice()->getComponentAt( component, 30, 32 );
        text += QString( "%1, " ).arg( value, 0, 'f', 4 );
        value = latticeController_->lattice()->getComponentAt( component, 31, 32 );
        text += QString( "%1, " ).arg( value, 0, 'f', 4 );
        value = latticeController_->lattice()->getComponentAt( component, 32, 32 );
        text += QString( "%1" ).arg( value, 0, 'f', 4 );

        text += QString( "];\n" );

        /*
         for (int j = 0; j < latticeController_->lattice()->latticeSizeY(); ++j) {
         text += QString( "[" );
         for (int i = 0; i < latticeController_->lattice()->latticeSizeX(); ++i) {
         double value = latticeController_->lattice()->getComponentAt( component, i, j );

         if ( i!= 13 && j!=13) continue;

         text += QString( "%1" ).arg( value, 0, 'f', 4 );
         if ( i != latticeController_->lattice()->latticeSizeX() - 1 )
         text += QString( ", " );
         }
         text += QString( "]" );
         if ( j != latticeController_->lattice()->latticeSizeY() - 1 )
         text += QString( "; " );
         }
         text += QString( "];\n" );
         */
    }
    QFile data( fileName );
    if ( append ? data.open( QFile::Append ) : data.open( QFile::WriteOnly | QFile::Truncate ) ) {
        QTextStream out( &data );
        out << text;
    }
}

void Waveprogram2DPlot::saveViews(const QString& name)
{
    for (uint i = 0; i < plotViewVector_.size(); ++i) {
        saveViews( name, i );
    }
}

void Waveprogram2DPlot::saveViews(const QString& name, uint num)
{
    //QImage pixmap( 645, 600, QImage::Format_ARGB32 );
    QImage pixmap(
        latticeController_->latticeSizeX() + 0, latticeController_->latticeSizeY() + 0,
        QImage::Format_ARGB32 );
    pixmap.fill( Qt::white ); // Qt::transparent ?

    plotViewVector_[ num ]->print( pixmap, true );

    pixmap.save( name + QString( ".%1.png" ).arg( num ), "PNG" );
}

void Waveprogram2DPlot::on_actionSave_as_Png_triggered()
{
    QString fileName = QFileDialog::getSaveFileName( this, tr( "Image File" ), "img.png", tr(
        "Images (*.png)" ) );

    if ( !fileName.isEmpty() ) {
        saveViews( fileName );
    }

    /*
     QImage image = QImage(graph->width(), graph->height(), QImage::Format_ARGB32);
     for (int i = 0; i <= latticeController_->lattice()->sizeX() + 1; ++i) {
     for (int j = 0; j <= latticeController_->lattice()->sizeY() + 1; ++j) {
     image.setPixel(i, j, QColor("white").rgb() );
     }
     }
     graph->print(image);
     */
#if 0
    foreach( PlotView* view, plotViewVector_ ) {
        static int i=0;

        QImage pixmap(600, 600, QImage::Format_ARGB32);
        pixmap.fill(Qt::white); // Qt::transparent ?

        QwtPlotPrintFilter filter;
        int options = 0;//= QwtPlotPrintFilter::PrintAll;
        options &= ~QwtPlotPrintFilter::PrintBackground;
        options &= ~QwtPlotPrintFilter::PrintFrameWithScales;
        options &= ~QwtPlotPrintFilter::PrintMargin;
        options &= ~QwtPlotPrintFilter::PrintTitle;
        options &= ~QwtPlotPrintFilter::PrintLegend;
        options &= ~QwtPlotPrintFilter::PrintGrid;

        filter.setOptions(options);

        view->plot_->print(pixmap, filter);

        pixmap.save( fileName + QString( ".%1.new.png" ).arg( i ), "PNG" );
#if 0
        QImage image = QImage(
            latticeController_->lattice()->latticeSizeX()*10, latticeController_->lattice()->latticeSizeY()*10, QImage::Format_ARGB32 );
        QwtPlotPrintFilter filter;
        filter.setOptions(QwtPlotPrintFilter::PrintBackground);

        view->plot_->print(image, filter);

        image.save( fileName + QString( ".%1.new.png" ).arg( i ), "PNG" );

        QSvgGenerator generator;
        generator.setFileName(fileName + QString( ".%1.new.svg" ).arg( i ));
        generator.setSize(QSize(800, 600));

        view->plot_->canvas()->->print(generator);
#endif
        ++i;
    }

    for (uint component = 0; component < latticeController_->lattice()->numberOfVariables(); ++component) {
        QImage image = QImage(
            latticeController_->lattice()->latticeSizeX(), latticeController_->lattice()->latticeSizeY(), QImage::Format_ARGB32 );

        for (int i = 0; i < latticeController_->lattice()->latticeSizeX(); ++i) {
            for (int j = 0; j < latticeController_->lattice()->latticeSizeY(); ++j) {
                image.setPixel(
                    i, j, colorMaps_.getColorMap().color( QwtDoubleInterval( -2.2, 2.5 ), latticeController_->lattice()->getComponentAt(
                            component, i, j ) ).rgb() );
            }
        }
        image.mirrored( false, true ).save( fileName + QString( ".%1.png" ).arg( component ), "PNG" );

    }
#endif
    //std::cout << (fileName.append("-inh"));
}

void Waveprogram2DPlot::on_actionLoad_from_Png_triggered()
{
    QMessageBox msgBox( this );
    msgBox.setWindowModality( Qt::WindowModal );
    msgBox.setText( "This function has been deprecated." );
    msgBox.setIcon( QMessageBox::Information );
    msgBox.exec();

#if 0
    QString fileName = QFileDialog::getOpenFileName( this, tr( "Image File" ), ".", tr(
            "Images (*.png)" ) );
    for (uint component = 0; component < 1 /*latticeController_->lattice()->numberOfVariables()*/; ++component) {
        QImage image = QImage( fileName ).mirrored( true, false );
        //QImage image_inhib = QImage(fileName.append("-inh.png")).mirrored(true, false);

        QHash< QRgb, double > reversableMap;
        for (int i = 0; i < 100000; ++i) {
            double d = -2.5 + (2.5 - (-2.5)) / 100000. * i;
            QRgb rgbval = plotViewVector_[ 0 ]->firstSpectrogram()->colorMap().color(
                plotViewVector_[ 0 ]->firstSpectrogram()->data().range(), d ).rgb();
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
                latticeController_->lattice()->setComponentAt( component, i, j, reversableMap.value( rgb ) );
                //latticeController_->lattice()->setV(i,j, reversableMap.value(rgb_inhib) );
            }
        }
    }
    replot();
#endif
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

    if ( !fileName.isEmpty() ) {
        for (int i = 1; i <= 999; ++i) {
            QString path = QFileInfo( fileName ).path();
            QString baseName = QFileInfo( fileName ).completeBaseName();
            QString suffix = QFileInfo( fileName ).suffix();
            if ( i < 10 ) {
                movieQueue.enqueue( QString( path.append( "/" )
                    + baseName.append( "00%1." ).arg( i ) + suffix ) );
            } else if ( i < 100 ) {
                movieQueue.enqueue( QString( path.append( "/" ) + baseName.append( "0%1." ).arg( i )
                    + suffix ) );
            } else {
                movieQueue.enqueue( QString( path.append( "/" ) + baseName.append( "%1." ).arg( i )
                    + suffix ) );
            }
        }
        actionSave_as_Movie_Pngs->setText( QString( "stop" ).append( " (%1)" ).arg(
            movieQueue.size() ) );
    }
}

void Waveprogram2DPlot::on_actionExport_as_Matlab_Structure_triggered()
{
    if ( matlabExportFile_.isEmpty() ) {
        matlabExportFile_ = QFileDialog::getSaveFileName(
            this, tr( "Matlab File" ), "matlabexport.m", tr( "Matlab Files (*.m)" ) );
        if ( !matlabExportFile_.isEmpty() ) {
            matlabExportIndex_ = 1;
            actionExport_as_Matlab_Structure->setText( "is Exporting" );
        }
    } else {
        matlabExportFile_.clear();
        actionExport_as_Matlab_Structure->setText( "Export as Matlab Structure" );
    }
}

void Waveprogram2DPlot::savePng(QString filename)
{
    saveViews( filename );
#if 0
    for (uint component = 0; component < latticeController_->lattice()->numberOfVariables(); ++component) {
        QImage image = QImage(
            latticeController_->lattice()->latticeSizeX(), latticeController_->lattice()->latticeSizeY(), QImage::Format_ARGB32 );
        uint save_component = component;

        for (int i = 0; i < latticeController_->lattice()->latticeSizeX(); ++i) {
            for (int j = 0; j < latticeController_->lattice()->latticeSizeY(); ++j) {

                if ( plotViewVector_.size() > 0 ) {
                    image.setPixel(
                        i, j, plotViewVector_[ save_component ]->firstSpectrogram()->colorMap().color(
                            plotViewVector_[ save_component ]->firstSpectrogram()->data().range(),
                            latticeController_->lattice()->getComponentAt( save_component, i, j ) ).rgb() );
                } else {
                    image.setPixel( i, j, colorMaps_.getColorMap().color(
                            QwtDoubleInterval( -2.2, 2.5 ), latticeController_->lattice()->getComponentAt(
                                save_component, i, j ) ).rgb() );
                }
            }
        }
        image.mirrored( false, true ).save(
            filename + QString( ".%1" ).arg( save_component ), "PNG" );
    }
#endif
}

void Waveprogram2DPlot::adaptationModeCheckBox_clicked(bool b)
{
    adaptationMode( b );
}

/*
 void Waveprogram2DPlot::makeStableStart()
 {
 for (int j = 0; j < 10000; j++) {

 //        if (latticeController_->lattice()->getAt(170,30).getX() > 0)
 //            latticeController_->lattice()->copy(100,1,100,100,1,100);
 // if ((j % 400 == 0) && (latticeController_->lattice()->epsilon > 0.08)) { latticeController_->lattice()->epsilon += -0.0001; }
 if ( j % 100 == 0 ) {
 if ( latticeController_->lattice()->currentWavesize() > wavesize_ ) {
 low = low + 0.01 * (high - low);
 } else {
 high = high - 0.01 * (high - low);
 }
 //      std::cout << "\nNew high: " << high << ", New low: " << low << ", Epsilon: " << latticeController_->lattice()->epsilon() << "\n" << std::flush;
 }
 if ( latticeController_->lattice()->currentWavesize() < wavesize_ ) {
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
 std::cout << latticeController_->lattice()->currentWavesize();
 if ( latticeController_->lattice()->currentWavesize() > wavesize_ ) {
 low = low + gammaspacing * (high - low);
 gamma_lowValue->setValue( low );
 } else {
 high = high - gammaspacing * (high - low);
 gamma_highValue->setValue( high );
 }
 //!    SurfacePoint po = latticeController_->lattice()->centreOfExcitation();
 //!    std::cout << "\nNew high: " << high << ", New low: " << low << ", Epsilon: " << latticeController_->lattice()->epsilon() << " " << po.x <<"\n" << std::flush;
 }
 if ( latticeController_->lattice()->currentWavesize() < wavesize_ ) {
 latticeParameters.value( "gamma" )->set( low );
 // std::cout << " g set to " << Model::g << " " << std::endl << std::flush;
 } else {
 latticeParameters.value( "gamma" )->set( high );
 // std::cout << " g set to " << Model::g << " " << std::endl << std::flush;
 }
 j++;
 }
 */

void Waveprogram2DPlot::updateLabels()
{
    simulationTimeLabel->setNum( latticeController_->lattice()->time() );
    clusterNumberLabel->setNum( latticeController_->lattice()->numberOfClusters() );
}

void Waveprogram2DPlot::loop()
{
    while (loopruns) {
        //for(int j=0; j < 5; j++) {
        {

            if ( adaptationMode() ) {
                latticeController_->lattice()->adaptParameters();
                emit updateParameters();
            }
            latticeController_->stepNum( 5 );
            /*
             std::cout << latticeController_->lattice()->getMax( 0 ) << latticeController_->lattice()->getMax( 1 ) << std::endl;
             this->step(  );
             std::cout << latticeController_->lattice()->getMax( 0 ) << latticeController_->lattice()->getMax( 1 ) << std::endl;
             this->step(  );
             std::cout << latticeController_->lattice()->getMax( 0 ) << latticeController_->lattice()->getMax( 1 ) << std::endl;
             this->step(  );
             std::cout << latticeController_->lattice()->getMax( 0 ) << latticeController_->lattice()->getMax( 1 ) << std::endl;
             this->step(  );
             std::cout << latticeController_->lattice()->getMax( 0 ) << latticeController_->lattice()->getMax( 1 ) << std::endl;
             */
            //std::cout << latticeController_->lattice()->time() << std::endl;

            updateLabels();
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
        //this->resize(this->size().rwidth(), this->size().rheight() + (( (int) (latticeController_->lattice()->time() * 10) % 2)*2 - 1));
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
         buffer.push_front( latticeController_->lattice()->getMaxU() );
         std::cout << buffer.front() << "\t";
         double deriv1, deriv2;
         if (buffer.size() > 4) {
         deriv1 = (3.*buffer[0] - 4.*buffer[1] + 1.*buffer[2]) / (2.0 * latticeController_->lattice()->timeStep() );
         deriv2 = (2*buffer[0] - 5*buffer[1] + 4*buffer[2] -1* buffer[3] ) / (latticeController_->lattice()->timeStep() * latticeController_->lattice()->timeStep() );
         std::cout << deriv1 << "\t" << deriv2;
         }
         std::cout << "\n";
         */
        /*
         std::pair<double, double> curv = latticeController_->lattice()->getCurvature();
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
            QString modelName( latticeController_->lattice()->modelName().c_str() );
            modelName = modelName.remove( QRegExp( "[^A-Za-z]" ) );
            exportAsMatlabStructure( matlabExportFile_, modelName, matlabExportIndex_, true );
            ++matlabExportIndex_;
        }

        /*        QScriptEngine engine;

         QScriptValue objectValue = engine.newQObject(this);
         engine.globalObject().setProperty("myObject", objectValue);
         qDebug() << engine.evaluate( "myObject.changeDiffusion( 10 ) " ).toNumber();

         */

    }
}

void Waveprogram2DPlot::on_numInitialPushButton_clicked()
{
    int i = numInitialComboBox->currentIndex();
    latticeController_->lattice()->toInitial( i );
    replot();
}

void Waveprogram2DPlot::on_actionAbout_triggered()
{
    std::stringstream s;
    latticeController_->lattice()->status( s );
    QString status = QString( "<pre>" ) + QString::fromUtf8( s.str().c_str() ) + QString( "</pre>" );
    QMessageBox::about( this, "Wellenprogramm", status );
}

void Waveprogram2DPlot::on_actionShow_Slice_triggered(bool b)
{
    //    sliceWidget->setVisible( b );
}

void Waveprogram2DPlot::readSettings()
{
    QSettings settings;
    settings.beginGroup( "2dPlot" );

    restoreState( settings.value( "state" ).toByteArray(), 1 );
    restoreGeometry( settings.value( "geometry" ).toByteArray() );
    //resize(settings.value("size", QSize(400, 400)).toSize());
    //move(settings.value("pos", QPoint(200, 200)).toPoint());
    /*
     QString s = settings.value("dockPositions").toString();
     QTextStream stream;
     stream << s;
     stream >> *mainWindow;
     */

    settings.endGroup();

}

void Waveprogram2DPlot::writeSettings()
{
    QSettings settings;
    settings.beginGroup( "2dPlot" );
    //    settings.setValue("size", size());
    //    settings.setValue("pos", pos());
    settings.setValue( "geometry", saveGeometry() );
    settings.setValue( "state", saveState( 1 ) );
    /*
     QTextStream stream;

     stream << *this;
     QString s;
     stream >> s;

     settings.setValue("dockPositions", s);
     */
    settings.endGroup();
    config.write();
}

void Waveprogram2DPlot::setUpParameterSets()
{
    //    parameterSetsDropDown->clear();
    //    parameterSetsDropDown->addItem(QString("add new..."));


    //if ( savedParameterSets.size() + 1 > parameterSetsDropDown->count() ) {
    for (int i = parameterSetsDropDown->count(); i < savedParameterSets.size() + 1; ++i) {
        parameterSetsDropDown->addItem( QString( "Set #%1" ).arg( i ) );
    }

    //} else if ( savedParameterSets.size() + 1 < parameterSetsDropDown->count() ) {

    for (int i = parameterSetsDropDown->count(); i > savedParameterSets.size() + 1; --i) {
        parameterSetsDropDown->removeItem( i - 1 );
    }
    //}
}

void Waveprogram2DPlot::updateParametersToSet(int setNum)
{

}

void Waveprogram2DPlot::on_parameterSetsDropDown_currentIndexChanged(int i)
{
    if ( i <= 0 ) {
        return;
    } else if ( i <= savedParameterSets.size() ) {
        ParameterValueMap p = savedParameterSets.at( i - 1 );
        ParameterValueMap::const_iterator it = p.constBegin();
        for (; it != p.constEnd(); ++it) {
            QString k = it.key();
            double val = it.value();
            latticeParameters.value( k )->set( val );
            emit updateParameters();
        }
        //latticeParameters = savedParameterSets.at( i - 1 );
    }
}

void Waveprogram2DPlot::on_parameterSetsSave_clicked()
{
    ParameterValueMap p;
    ParameterMap::const_iterator it = latticeParameters.constBegin();
    for (; it != latticeParameters.constEnd(); ++it) {
        QString k = it.key();
        double val = it.value()->get();
        p.insert( k, val );
    }

    int i = parameterSetsDropDown->currentIndex();
    if ( i == 0 ) {
        // Neuen Index anlegen
        savedParameterSets << p;
        setUpParameterSets();
        parameterSetsDropDown->setCurrentIndex( parameterSetsDropDown->count() - 1 );
    } else if ( i <= savedParameterSets.size() ) {
        // Alten Index überschreiben
        savedParameterSets.replace( i - 1, p );
    }
}

void Waveprogram2DPlot::on_parameterSetsDelete_clicked()
{
    int i = parameterSetsDropDown->currentIndex();
    if ( i == 0 ) {
        // Kann ich nicht löschen
        return;
    } else if ( i - 1 < savedParameterSets.size() ) {
        // Entferne gewählten Index.
        savedParameterSets.remove( i - 1 );
        setUpParameterSets();
        parameterSetsDropDown->setCurrentIndex( 0 );
    }
}

void Waveprogram2DPlot::readParameterSets()
{
    qDebug() << "read Parameter Sets for" << QString(
        latticeController_->lattice()->modelName().c_str() );
    QSettings settings;
    settings.beginGroup( "Models" );
    settings.beginGroup( QString( latticeController_->lattice()->modelName().c_str() ) );

    savedParameterSets.clear();
    int size = settings.beginReadArray( "parameter_sets" );
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex( i );
        ParameterValueMap p;

        QStringList keys = settings.allKeys();
        foreach( QString k, keys )
            {
                p.insert( k, settings.value( k ).toDouble() );
            }

        savedParameterSets << p;
    }
    settings.endArray();
    settings.endGroup();
    settings.endGroup();

    setUpParameterSets();
}

void Waveprogram2DPlot::writeParameterSets()
{
    qDebug() << "write Parameter Sets for" << QString(
        latticeController_->lattice()->modelName().c_str() );
    QSettings settings;
    settings.beginGroup( "Models" );
    settings.beginGroup( QString( latticeController_->lattice()->modelName().c_str() ) );

    settings.beginWriteArray( "parameter_sets" );
    for (int i = 0; i < savedParameterSets.size(); ++i) {
        settings.setArrayIndex( i );
        qDebug() << "set #" << i;
        const ParameterValueMap p = savedParameterSets.at( i );
        ParameterValueMap::const_iterator it = p.constBegin();
        for (; it != p.constEnd(); ++it) {
            settings.setValue( it.key(), it.value() );
            qDebug() << it.key() << it.value();
        }

    }
    settings.endArray();
    settings.endGroup();
    settings.endGroup();
}

void Waveprogram2DPlot::on_actionShow_Single_Plot_triggered()
{
    PlotSingle* p = new PlotSingle( latticeController_->lattice(), this );

    p->show();
    connect( this, SIGNAL( modelClosed() ), p, SLOT( close() ) );
    connect( this, SIGNAL( replotAllChildren() ), p, SLOT( update() ) );
}

void Waveprogram2DPlot::showSinglePlot() {

}

void Waveprogram2DPlot::showSlicePlot() {

}

