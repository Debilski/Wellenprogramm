/*****************************************************************************
 * RDS Lattice Library / ‘Wellenprogramm’
 * Copyright (C) 2008–2009  Rike-Benjamin Schuppner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Public License, Version 2.0
 *****************************************************************************/
// #include <iostream>

#include "plotwindow_2d.h"

#include <fstream>
#include <cmath>

#include <QtGui>
#include <QtCore>

#include "plot_layer.h"

#include "parameter_spin_box.h"
#include "diffusion_spin_box.h"
#include "rightclickable_tool_bar.h"

#include "spectrogram_data.h"

#include "export_preferences.h"
#include "preference_pager.h"

class Waveprogram2DPlot::PrivateData {
public:
    QList< QwtPlotMarker* > markers;
    QList< QwtPlotCurve* > curves;
    QTimer* timer;
    QMenu toolBarRightClickMenu;
};

void Waveprogram2DPlot::setTitle()
{
    if ( latticeController_->isValid() ) {
        setWindowTitle( QString::fromUtf8(
            "‘%1’: Simulate a field size of %2×%3 on a lattice of %4×%5 " ).arg(
            latticeController_->modelTitle() ).arg( latticeController_->sizeX() ).arg(
            latticeController_->sizeY() ).arg( latticeController_->latticeSizeX() ).arg(
            latticeController_->latticeSizeY() ) );
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

Waveprogram2DPlot::Waveprogram2DPlot(QMainWindow * parent) :
    QMainWindow( parent ), parent( parent )
{
    d_data = new PrivateData;

    // LatticeController vllt ohne Zeiger, aber auf jeden Fall besser einbauen als so!
    lc_.reset( new LatticeController() );
    latticeController_ = lc_.get();
    connect( latticeController_, SIGNAL( stopped() ), this, SLOT( replot() ) );
    connect( latticeController_, SIGNAL( stopped() ), this, SLOT( updateLabels() ) );

    connect( latticeController_, SIGNAL( changed() ), this, SLOT( replot() ) );
    connect( latticeController_, SIGNAL( changed() ), this, SLOT( updateLabels() ) );
    connect( this, SIGNAL( modelChanged() ), this, SLOT( updateLabels() ) );

    connect( latticeController_, SIGNAL( processed(int) ), this, SLOT( resizeWindowToForceUpdate() ) );
    connect( latticeController_, SIGNAL( processed(int) ), this, SLOT( updateLabels() ) );

    connect( latticeController_, SIGNAL( parametersChanged() ), this, SIGNAL( updateParameters() ) );

    connect( latticeController_, SIGNAL( processed(int) ), this, SLOT( movieExport() ) );

    colorMapMode = defaultColorMapMode;

    setupUi( this ); // this sets up GUI

    // setAttribute( Qt::WA_DeleteOnClose );
    //    sliceWidget->setVisible( false );

    //setAttribute( Qt::WA_MacMetalStyle );
    //statusBar();

    setTitle();

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

    connect( actionClose_Window, SIGNAL( triggered()), this, SLOT( close() ) );

    connect( startStopButton, SIGNAL( clicked() ), this, SLOT( toggleStartStop() ) );
    connect( actionStartStop, SIGNAL( triggered() ), this, SLOT( toggleStartStop() ) );

    connect( actionClear, SIGNAL( triggered() ), latticeController_, SLOT( clear() ) );
    connect( clearButton, SIGNAL( clicked() ), latticeController_, SLOT( clear() ) );

    connect(
        noiseSpinBox, SIGNAL( valueChanged(double) ), latticeController_,
        SLOT( setNoiseIntensity( double ) ) );
    connect(
        correlationSpinBox, SIGNAL( valueChanged(int) ), latticeController_,
        SLOT( setNoiseCorrelation( int ) ) );
    connect(
        timestepSpinBox, SIGNAL( valueChanged(double) ), latticeController_,
        SLOT( setTimeStep( double ) ) );

    setUpSizeMenu();
    updateSizeMenu();
    setUpActions();

    setUpColorSchemeMenu();

    updatePeriodTime_ = 100;
    setUpUpdatePeriodMenu();
    defectsEditor = 0;

    exportPreferences = new ExportPreferences( this );

    connect(
        this, SIGNAL( viewsChanged( const QStringList& )), exportPreferences,
        SLOT( setViewNames( const QStringList& ) ) );

    menuDock_Windows->addAction( simulationWidget->toggleViewAction() );

    parameterWidget->toggleViewAction()->setText( "Model Properties" );
    menuDock_Windows->addAction( parameterWidget->toggleViewAction() );
    adaptationParameterWidget->toggleViewAction()->setText( "Adaptation Properties" );
    menuDock_Windows->addAction( adaptationParameterWidget->toggleViewAction() );
    exportPreferences->toggleViewAction()->setText( "Export Properties" );
    menuDock_Windows->addAction( exportPreferences->toggleViewAction() );

    menuDock_Windows->addAction( extrasWidget->toggleViewAction() );
    menuDock_Windows->addAction( paintDockWidget->toggleViewAction() );

    QString lastUsedModel = config.option( "last_model" ).value().toString();
    qDebug() << lastUsedModel;
    initField(
        config.option( "last_size_x" ).value().toInt(),
        config.option( "last_size_y" ).value().toInt(),
        config.option( "last_lattice_size_x" ).value().toInt(), config.option(
            "last_lattice_size_y" ).value().toInt(), lastUsedModel );

    showClusterIds_ = false;

    replot();

    d_data->timer = new QTimer( this );
    connect( d_data->timer, SIGNAL(timeout()), this, SLOT(replot()) );

    clipboard = QApplication::clipboard();

    matlabExportFile_ = QString();
    matlabExportIndex_ = 0;

    readSettings();

    connect( simulationTimeLabel, SIGNAL( reset() ), this, SLOT( resetTime()) );

    setUpToolBars();
    setUnifiedTitleAndToolBarOnMac( true );
}

void Waveprogram2DPlot::setUpToolBars()
{
    toolBar = new RightclickableToolBar( tr( "Save Toolbar" ), this );

    toolBar->addAction( actionSave );
    toolBar->addAction( actionRecall );
    toolBar->addAction( actionDump );
    toolBar->addAction( actionUndump );
    toolBar->addAction( actionSave_as_Png );
    //toolBar->addAction( actionLoad_from_Png );
    toolBar->addAction( actionSave_as_Movie_Pngs );
    toolBar->addAction( actionExport_as_Matlab_Structure );
    this->addToolBar( toolBar );

    paintToolBar = new RightclickableToolBar( tr( "Paint Toolbar" ), this );
    paintToolBar->setToolButtonStyle( Qt::ToolButtonTextUnderIcon );

    exportPreferences->toggleViewAction()->setIcon( QIcon( QPixmap( QString::fromUtf8(
        ":/icons/icons/paint_pref.svg" ) ) ) );
    paintToolBar->addAction( exportPreferences->toggleViewAction() );
    QIcon icon;
    icon.addPixmap(
        QPixmap( QString::fromUtf8( ":/icons/icons/paint.svg" ) ), QIcon::Normal, QIcon::Off );
    paintDockWidget->toggleViewAction()->setIcon( icon );
    paintToolBar->addAction( paintDockWidget->toggleViewAction() );

    this->addToolBar( paintToolBar );
}

void Waveprogram2DPlot::resetTime()
{
    latticeController_->setTime( 0 );
    simulationTimeLabel->setNum( latticeController_->time() );
}

void Waveprogram2DPlot::closeEvent(QCloseEvent * event)
{
    latticeController_->stopLoop();
    writeSettings();
    writeParameterSets();
    event->accept();
}

void Waveprogram2DPlot::updateDefects()
{
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

    d_data->timer->setInterval( updatePeriodTime_ );
    if ( d_data->timer->isActive() )
        d_data->timer->start();
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
    disconnect( this, SIGNAL( colorMapChanged( const QwtColorMap& )), 0, 0 );
    disconnect( this, SIGNAL( colorMapChanged( const QwtColorMap&, ColorMapAdaptationModes )), 0, 0 );

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
    QString name = latticeController_->getModelName();
    int i1 = name.indexOf( QString( "FhnKLattice" ), 0, Qt::CaseSensitive );
    if ( i1 != -1 ) {
        qDebug() << "Yes";
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

    QStringList viewNames_;
    foreach( PlotView* view, plotViewVector_ )
        {
            viewNames_ << view->name();
        }

    emit viewsChanged( viewNames_ );
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
        DiffusionSpinBox* diffusionBox = new DiffusionSpinBox( component, parameterWidgetContents );

        //! Bisschen unsauber…
        diffusionBox->setValue( latticeController_->lattice()->getDiffusion( component ) );
        diffusionBox->setDecimals( 3 );
        diffusionBox->setMaximum( 1000 );
        QString name = "Diffusion " + QString::fromStdString(
            latticeController_->lattice()->componentInfos[ component ].shortName() );
        QLabel* label = new QLabel( name, parameterWidgetContents );
        parameterWidgetFormLayout->addRow( label, diffusionBox );
        connect(
            diffusionBox, SIGNAL( valueChanged(const int&, const double&) ), latticeController_,
            SLOT( setDiffusion(const int&, const double&) ) );
    }
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
    parameterWidgetContents->setSizePolicy( QSizePolicy(
        QSizePolicy::Preferred, QSizePolicy::Maximum ) );

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
    adaptationParameterWidgetContents->setSizePolicy( QSizePolicy(
        QSizePolicy::Preferred, QSizePolicy::Maximum ) );
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
        adaptationModeCheckBox, SIGNAL( clicked(bool) ), latticeController_,
        SLOT ( setAdaptationMode(bool) ) );
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
            parameterBox, SIGNAL( valueChanged(const QString&, const double&) ), this,
            SLOT( changeParameter(const QString&, const double&) ) );
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
            parameterBox, SIGNAL( valueChanged(const QString&, const double&) ), this,
            SLOT( changeParameter(const QString&, const double&) ) );

        connect( this, SIGNAL( updateParameters() ), parameterBox, SLOT ( updateValue() ) );
    }
    if ( latticeAdaptationParameters.size() == 0 ) {
        adaptationModeCheckBox->setCheckable( false );
    } else {
        adaptationModeCheckBox->setCheckable( true );
    }
}

void Waveprogram2DPlot::changeParameter(const QString& paramName, double value)
{
    Parameter< double >* p = latticeParameters.value( paramName );
    if ( p != 0 ) {
        changeParameter( p, value );
        return;
    }
    p = latticeAdaptationParameters.value( paramName );
    if ( p != 0 ) {
        changeParameter( p, value );
        return;
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
            {
                currentView->attachItem( m );
            }
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
    delete d_data->timer;
    delete d_data;
}

void Waveprogram2DPlot::setUpSizeMenu()
{
    recentSizes << QString::fromStdString( LatticeGeometry::stringFromSize( 4, 4, 8, 8 ) )
        << QString::fromStdString( LatticeGeometry::stringFromSize( 8, 8, 8, 8 ) )
        << QString::fromStdString( LatticeGeometry::stringFromSize( 8, 8, 512, 512 ) )
        << QString::fromStdString( LatticeGeometry::stringFromSize( 16, 16, 32, 32 ) )
        << QString::fromStdString( LatticeGeometry::stringFromSize( 16, 16, 1024, 1024 ) )
        << QString::fromStdString( LatticeGeometry::stringFromSize( 32, 32, 32, 32 ) )
        << QString::fromStdString( LatticeGeometry::stringFromSize( 64, 64, 64, 64 ) )
        << QString::fromStdString( LatticeGeometry::stringFromSize( 128, 128, 128, 128 ) )
        << QString::fromStdString( LatticeGeometry::stringFromSize( 128, 128, 256, 256 ) )
        << QString::fromStdString( LatticeGeometry::stringFromSize( 128, 128, 512, 512 ) )
        << QString::fromStdString( LatticeGeometry::stringFromSize( 256, 256, 256, 256 ) )
        << QString::fromStdString( LatticeGeometry::stringFromSize( 256, 256, 512, 512 ) );

    createLatticeWithNewSize = new QAction( tr( "Create Lattice with new Size" ), this );

    sizeMapper = new QSignalMapper( this );
    connect( sizeMapper, SIGNAL(mapped( const QString&)), this, SLOT( changeSize(const QString&)) );
}

void Waveprogram2DPlot::updateSizeMenu()
{
    menuSize->clear();
    foreach( QString size, recentSizes )
        {
            QAction* action = new QAction( size, this );
            menuSize->addAction( action );
            connect( action, SIGNAL( triggered() ), sizeMapper, SLOT( map() ) );
            sizeMapper->setMapping( action, size );
        }
    menuSize->addSeparator();
    menuSize->addAction( createLatticeWithNewSize );
}

void Waveprogram2DPlot::setUpActions()
{
    QList< QAction * > availableActions = menuMode->actions();
    QSignalMapper* mapper = new QSignalMapper( this );

    QStringList names = latticeController_->getModelNames();
    foreach( QString name, names )
        {
            QAction* action = new QAction( name, this );
            menuMode->insertAction( availableActions.first(), action );
            connect( action, SIGNAL( triggered() ), mapper, SLOT(map()) );
            mapper->setMapping( action, name );
        }

    connect( mapper, SIGNAL(mapped(const QString&)), this, SLOT(changeModel(const QString&) ) );
}

void Waveprogram2DPlot::changeSize(const QString& size)
{
    QString modelName = latticeController_->getModelName();
    emit
    modelClosed();

    qDebug() << QString::fromStdString( LatticeGeometry::stringFromSize(
        LatticeGeometry::sizeFromString( size.toStdString() ) ) );
    LatticeGeometry s = LatticeGeometry::sizeFromString( size.toStdString() );
    if ( s != LatticeGeometry() ) {
        killField();
        initField( s.sizeX(), s.sizeY(), s.latticeSizeX(), s.latticeSizeY(), modelName );
    }
}

void Waveprogram2DPlot::changeModel(const QString& modelName)
{
    emit
    modelClosed();

    int x = latticeController_->sizeX();
    int y = latticeController_->sizeY();
    int lx = latticeController_->latticeSizeX();
    int ly = latticeController_->latticeSizeY();

    killField();

    initField( x, y, lx, ly, modelName );
}

void Waveprogram2DPlot::initField(int realSizeX, int realSizeY, int latticeSizeX, int latticeSizeY,
                                  const QString& model)
{
    qDebug() << model;
    QString delayMsg = QString( "Creating a lattice of %1 x %2. This might take some time." ).arg(
        latticeSizeX ).arg( latticeSizeY );

    Ui::plotWindow_2d::statusBar->showMessage( delayMsg );
    // Schnell letzte Updates ausführen, bevor das Modell geladen wird
    QCoreApplication::processEvents();

    latticeController_->load( model, realSizeX, realSizeY, latticeSizeX, latticeSizeY );

    config( "last_model" ).setValue( QVariant( model ) );
    config( "last_size_x" ).setValue( realSizeX );
    config( "last_size_y" ).setValue( realSizeY );
    config( "last_lattice_size_x" ).setValue( latticeSizeX );
    config( "last_lattice_size_y" ).setValue( latticeSizeY );

    config.write();

    //latticeController_->lattice()->setDiffusion( 0, 1. );
    latticeController_->lattice()->clear();
    latticeController_->lattice()->toInitial( 0 );

    Ui::plotWindow_2d::statusBar->clearMessage();

    setUpViews();
    reorderTabs();
    setUpColorMap();
    setUpModelProperties();

    boundaryConditionsComboBox->setCurrentIndex( latticeController_->lattice()->boundaryCondition() );
    correlationSpinBox->setValue( latticeController_->lattice()->noiseCorrelation() );
    timestepSpinBox->setValue( latticeController_->lattice()->timeStep() );

    setUpBoundaryConditionsSelector();
    setTitle();

    readParameterSets();
    emit modelChanged();
}

void Waveprogram2DPlot::killField()
{
    writeParameterSets();

    plotViewVector_.clear();

    latticeController_->closeLattice();

    setTitle();
}

void Waveprogram2DPlot::toggleStartStop()
{
    if ( latticeController_->loopRuns() == false ) {
        d_data->timer->start( updatePeriodTime_ );
        startStopButton->setText( "Stop" );
        actionStartStop->setText( "Stop" );
        latticeController_->startLoop();
    } else {
        latticeController_->stopLoop();
        d_data->timer->stop();
        startStopButton->setText( "Start" );
        actionStartStop->setText( "Start" );
    }
}

/**
 * Schreibt den aktuellen Plot in einem Matlab-lesbaren Format in die Zwischenablage.
 */
/**
 * Exportiert eine Zeitreihe in eine matlab-lesbare Datei
 */
void Waveprogram2DPlot::exportAsMatlabStructure(QString fileName, QString structureName,
                                                int timeIndex, bool append /*= true*/)
{
    QString text;
    text += QString( "Time_%1{%2} = %3;\n" ).arg( structureName ).arg( timeIndex ).arg(
        latticeController_->lattice()->time(), 0, 'f', 4 );
    for (int component = 0; component < latticeController_->lattice()->numberOfVariables(); ++component)
    {
        text += QString( "%1{%2,%3} = " ).arg( structureName ).arg( timeIndex ).arg( component + 1 );
        /*        text += QString( "[" );

        double value = latticeController_->lattice()->getComponentAt( component, 16, 16 );
        text += QString( "%1, " ).arg( value, 0, 'f', 4 );
        //value = latticeController_->lattice()->getComponentAt( component, 17, 16 );
         //text += QString( "%1, " ).arg( value, 0, 'f', 4 );
         //value = latticeController_->lattice()->getComponentAt( component, 18, 16 );
         //text += QString( "%1, " ).arg( value, 0, 'f', 4 );
         //
        value = latticeController_->lattice()->getComponentAt( component, 30, 32 );
        text += QString( "%1, " ).arg( value, 0, 'f', 4 );
        //        value = latticeController_->lattice()->getComponentAt( component, 31, 32 );
        //       text += QString( "%1, " ).arg( value, 0, 'f', 4 );
        value = latticeController_->lattice()->getComponentAt( component, 32, 32 );
        text += QString( "%1," ).arg( value, 0, 'f', 4 );
        //        value = latticeController_->lattice()->getComponentAt( component, 33, 32 );
        //        text += QString( "%1, " ).arg( value, 0, 'f', 4 );
        value = latticeController_->lattice()->getComponentAt( component, 34, 32 );
        text += QString( "%1 " ).arg( value, 0, 'f', 4 );

        text += QString( "];\n" );
*/

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
        text += QString( "];\n" );

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
    if ( exportPreferences->hasPngExportComponentChecked( num, true ) ) {
        QImage image( latticeController_->latticeSizeX() + 0, latticeController_->latticeSizeY()
            + 0, QImage::Format_ARGB32 );
        image.fill( Qt::white ); // Qt::transparent ?

        plotViewVector_[ num ]->print( image, true );

        image.save( name + QString( ".%1.png" ).arg( num ), "PNG" );
    }
    if ( exportPreferences->hasPngExportComponentChecked( num, false ) ) {
        QImage image( latticeController_->latticeSizeX() + 240, latticeController_->latticeSizeY()
            + 200, QImage::Format_ARGB32 );
        image.fill( Qt::white ); // Qt::transparent ?

        plotViewVector_[ num ]->print( image, false );

        image.save( name + QString( "-large.%1.png" ).arg( num ), "PNG" );
    }
}

void Waveprogram2DPlot::savePng(QString filename)
{
    saveViews( filename );
}

void Waveprogram2DPlot::resizeWindowToForceUpdate()
{
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
}

void Waveprogram2DPlot::updateLabels()
{
    simulationTimeLabel->setNum( latticeController_->lattice()->time() );
    clusterNumberLabel->setNum( latticeController_->lattice()->numberOfClusters() );

    QString minMax = "%1 [%2, %3]";
    QString s;
    for (uint component = 0; component < latticeController_->lattice()->numberOfVariables(); ++component)
    {
        QString name = QString::fromStdString(
            latticeController_->lattice()->componentInfos[ component ].shortName() );
        double min = latticeController_->lattice()->getMin( component );
        double max = latticeController_->lattice()->getMax( component );
        s += minMax.arg( name ).arg( min , 0, 'G', 2 ).arg( max, 0, 'G', 2 );
        s.append(", ");
    }

    Ui::plotWindow_2d::statusBar->showMessage(s);
    if (! latticeController_->lattice()->isAlive() ) {
        Ui::plotWindow_2d::statusBar->showMessage("Lattice crashed.");
    } else {
    //    Ui::plotWindow_2d::statusBar->clearMessage();
    }
}


void Waveprogram2DPlot::movieExport()
{
    if ( !movieQueue.empty() ) {
        savePng( movieQueue.head() );
        movieQueue.dequeue();
        actionSave_as_Movie_Pngs->setText( tr( "Stop" ).append( " (%1)" ).arg( movieQueue.size() ) );

        QFile file( ":/icons/icons/photos_overlay.svg" );
        if ( !file.open( QIODevice::ReadOnly | QIODevice::Text ) )
            return;
        QByteArray whole;
        while (!file.atEnd()) {
            QByteArray line = file.readLine();
            whole.append( line );
        }

        whole.replace(
            "$REPLACE_THIS_WITH_NUMBER$", QString( "%1" ).arg( movieQueue.size() ).toAscii() );
        QPixmap p;
        if ( p.loadFromData( whole, "SVG" ) ) {
            QIcon icon = QIcon( p );
            actionSave_as_Movie_Pngs->setIcon( icon );
        }

        if ( movieQueue.empty() ) {
            actionSave_as_Movie_Pngs->setText( tr( "Save as Movie PNGs" ) );
        }
    }

    if ( !matlabExportFile_.isEmpty() ) {
        QString modelName( latticeController_->lattice()->modelName().c_str() );
        modelName = modelName.remove( QRegExp( "[^A-Za-z]" ) );
        exportAsMatlabStructure( matlabExportFile_, modelName, matlabExportIndex_, true );
        ++matlabExportIndex_;
    }
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

void Waveprogram2DPlot::showSinglePlot()
{

}

void Waveprogram2DPlot::showSlicePlot()
{

}

void Waveprogram2DPlot::on_actionSave_triggered()
{
    d_data->timer->stop();
    QString fileName = QFileDialog::getSaveFileName( this, tr( "Save to binary format" ), ".", tr(
        "Binary (*.bin)" ) );
    if ( !fileName.isEmpty() ) {
        latticeController_->lattice()->save( false, fileName.toStdString() );
    }
    d_data->timer->start();
}

void Waveprogram2DPlot::on_actionRecall_triggered()
{
    d_data->timer->stop();
    //latticeController_->lattice()->recall();
    QString fileName = QFileDialog::getOpenFileName(
        this, tr( "Load from binary format" ), ".", tr( "Binary (*.bin);; All (*)" ) );
    //std::cout << fileName;
    if ( !fileName.isEmpty() ) {
        latticeController_->lattice()->recall( fileName.toStdString() );
        replot();
    }
    d_data->timer->start();
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

void Waveprogram2DPlot::on_actionSave_as_Png_triggered()
{
    QString fileName = QFileDialog::getSaveFileName( this, tr( "Image File" ), "img.png", tr(
        "Images (*.png)" ) );

    if ( !fileName.isEmpty() ) {
        saveViews( fileName );
    }
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
        actionSave_as_Movie_Pngs->setText( QString( "Save as Movie PNGs" ) );
        actionSave_as_Movie_Pngs->setIcon( QPixmap( ":/icons/icons/photos.svg" ) );
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

        QFile file( ":/icons/icons/photos_overlay.svg" );
        if ( !file.open( QIODevice::ReadOnly | QIODevice::Text ) )
            return;
        QByteArray whole;
        while (!file.atEnd()) {
            QByteArray line = file.readLine();
            whole.append( line );
        }

        whole.replace(
            "$REPLACE_THIS_WITH_NUMBER$", QString( "%1" ).arg( movieQueue.size() ).toAscii() );
        QPixmap p;
        if ( p.loadFromData( whole, "SVG" ) ) {
            QIcon icon = QIcon( p );
            actionSave_as_Movie_Pngs->setIcon( icon );
        }
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

void Waveprogram2DPlot::on_actionShow_Single_Plot_triggered()
{
    PlotSingle* p = new PlotSingle( latticeController_->lattice(), this );
    p->show();
    connect( this, SIGNAL( modelClosed() ), p, SLOT( close() ) );
    connect( this, SIGNAL( replotAllChildren() ), p, SLOT( update() ) );
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
    replot();
}

void Waveprogram2DPlot::on_actionEdit_Script_triggered()
{
    if ( !scriptEditor ) {
        scriptEditor = new ScriptEditor( this, latticeController_ );
    }
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

void Waveprogram2DPlot::on_actionPreferences_triggered()
{
    if ( !globalPreferences )
        globalPreferences = new PreferencePager( this );
    if ( globalPreferences )
        globalPreferences->show();
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
            // qDebug() << it.key() << it.value();
        }

    }
    settings.endArray();
    settings.endGroup();
    settings.endGroup();
}
