#ifndef PLOTWINDOW_2D_H
#define PLOTWINDOW_2D_H

#include <QtGui>
#include <QtScript>

#include "ui_plotwindow_2d.h"

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_spectrogram.h>
#include <qwt_plot_spectrogram.h>
#include <qwt_color_map.h>

#include <qwt_scale_widget.h>
#include <qwt_scale_draw.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_layout.h>

#include <qwt_symbol.h>

#include "defects_editor.h"
#include "script_editor.h"

#include "lattice_interface.h"

#include "configuration.h"
//#include "rds_lattice.h"
/*
 #include "lattice_models.h"
 #include "fhn_lattice.h"
 #include "barkley_lattice.h"
 */

class ParameterSpinBox : public QDoubleSpinBox {
Q_OBJECT
public:
    ParameterSpinBox(Parameter< double >* parameterReference, QWidget* parent = 0) :
        QDoubleSpinBox( parent ), parameter_( parameterReference )
    {
        parameterName_ = QString::fromUtf8( parameter_->name.c_str() );
        update();
    }
    void updateReference(Parameter< double >* parameterReference)
    {
        parameter_ = parameterReference;
        parameterName_ = QString::fromUtf8( parameter_->name.c_str() );
        update();
    }
public slots:
    void updateValue()
    {
        double p = parameter_->get();
        if ( p != this->value() )
            this->setValue( p );
    }
    void update()
    {
        this->setProperty( "parameter", QVariant( parameterName_ ) );
        this->setDecimals( parameter_->decimals() );
        this->setMaximum( parameter_->max() );
        this->setMinimum( parameter_->min() );
        this->setSingleStep( parameter_->stepSizeHint() );
        this->setValue( parameter_->get() );
    }
private:
    Parameter< double >* parameter_;
    QString parameterName_;
};


#include "tiny_double_edit.h"
/**
 * Class that holds Spectrogram, ColorMap and Plot for each tab.
 */
class PlotView : public QWidget {
Q_OBJECT
public:
    QwtPlotSpectrogram* spectrogram_;
    QwtLinearColorMap* colorMap_;
    QwtPlot* plot_;
    QwtScaleWidget* rightAxis;

    PlotView(const QwtRasterData& spectrogrammData, const QwtColorMap& colorMap, uint component,
             const QString label, QWidget* parent, bool isFft = false) :
        QWidget( parent ), component_( component ), isFft_( isFft )
    {
        QFont plotFont = QFont( "", 8 );
        QwtText labelIntensity( label );
        labelIntensity.setFont( plotFont );

        QHBoxLayout* horizontalLayout = new QHBoxLayout( this );
        plot_ = new QwtPlot( this );
        plot_->setAxisFont( QwtPlot::yLeft, plotFont );
        plot_->setAxisFont( QwtPlot::xBottom, plotFont );
        horizontalLayout->addWidget( plot_ );

        spectrogram_ = new QwtPlotSpectrogram();
        spectrogram_->setData( spectrogrammData );

        spectrogram_->setColorMap( colorMap );
        spectrogram_->attach( plot_ );

        rightAxis = plot_->axisWidget( QwtPlot::yRight );
        rightAxis->setTitle( labelIntensity );
        rightAxis->setColorBarEnabled( true );
        rightAxis->setColorMap( spectrogram_->data().range(), spectrogram_->colorMap() );

        plot_->setAxisScale(
            QwtPlot::yRight, spectrogram_->data().range().minValue(),
            spectrogram_->data().range().maxValue() );
        plot_->enableAxis( QwtPlot::yRight );
        plot_->setAxisFont( QwtPlot::yRight, plotFont );
        plot_->plotLayout()->setAlignCanvasToScales( true );

        rightClickMenu.addAction( QString( "Top Value: %1" ).arg(
            spectrogram_->data().range().maxValue() ), this, "changeTop()" );
        rightClickMenu.addAction( QString( "Bottom Value: %1" ).arg(
            spectrogram_->data().range().minValue() ), this, "changeBottom()" );

        rightAxis->setContextMenuPolicy( Qt::CustomContextMenu );
        connect( rightAxis, SIGNAL( customContextMenuRequested( const QPoint& ) ), this, SLOT( showMenu( const QPoint& ) ) );
    }
    QMenu rightClickMenu;
    ~PlotView()
    {
        //delete spectrogram_;
    }
    signals:
    void adaptComponent(uint& component, bool& isFft);

public slots:
    void replot(int)
    {
        replot();
    }
    void replot()
    {
        if ( isVisible() ) {
            emit adaptComponent( component_, isFft_ );
            rightAxis->setColorMap( spectrogram_->data().range(), spectrogram_->colorMap() );
            plot_->setAxisScale(
                QwtPlot::yRight, spectrogram_->data().range().minValue(),
                spectrogram_->data().range().maxValue() );

            plot_->replot();
        }
    }
    void setColorMap(const QwtColorMap& colorMap)
    {
        spectrogram_->setColorMap( colorMap );
        rightAxis->setColorMap( spectrogram_->data().range(), spectrogram_->colorMap() );
        replot();
    }
    void showMenu(const QPoint& p)
    {
        std::cout << p.x() << "," << p.y() << std::endl;

        rightClickMenu.popup( rightAxis->mapToGlobal( p ) );
    }
    void changeTop()
    {
        TinyDoubleEdit tEdit( this, spectrogram_->data().range().maxValue() );
        if ( tEdit.exec() ) {
            double c = tEdit.value();
        }
    }
    void changeBottom()
    {
        TinyDoubleEdit tEdit( this, spectrogram_->data().range().minValue() );
        if ( tEdit.exec() ) {
            double c = tEdit.value();
        }
    }
private:
    bool isFft_;
    uint component_;
};

/**
 * Action which stores the Modeltype it shall call
 */
class ModelAction : public QAction {
Q_OBJECT
public:
    ModelAction(const QString & text, std::string model, QObject * parent) :
        QAction( text, parent ), model_( model )
    {
        connect( this, SIGNAL( triggered( bool ) ), this, SLOT( modelTriggered( bool ) ) );
    }
private slots:
    void modelTriggered(bool /*b*/)
    {
        emit modelTriggered( model_ );
    }

private:
    std::string model_;
    signals:
    void modelTriggered(std::string);

};

class Waveprogram2DPlot : public QMainWindow, private Ui::plotWindow_2d {
Q_OBJECT
public:
    Waveprogram2DPlot(QMainWindow * parent = 0, int realSize = 128, int latticeSize = 128);
    ~Waveprogram2DPlot();

    typedef LatticeInterface ModelLattice;

    void loopStart();
    void loopStop();
    bool loopruns;
    void loop();
    QTimer *timer;

    void setUpActions();
    void setUpColorMap();
    void removeTabs();
    void setUpTabs();
    void setUpRanges();

    void setUpModelProperties();
    void setUpDiffusion();
    void setUpParameters();
    void setUpAdaptationParameters();
    void setUpBoundaryConditionsSelector();
    void setUpSlices();

public slots:

    void step();
    void step(int num);

    void replot();

    void toggleStartStop();

    void updateDefects();

    void on_clearButton_clicked();
    void on_noiseBox_valueChanged(double d);

    void on_boundaryConditionsComboBox_currentIndexChanged(int i);

    void adaptationModeCheckBox_clicked(bool b);

    void on_noise_correlationValue_valueChanged(int d);
    void on_timestepValue_valueChanged(double d);

    void on_midpoint_sizeValue_valueChanged(double d);

    void on_actionClose_Window_triggered();

    void on_actionEdit_Script_triggered();

    void on_actionEdit_defects_triggered();

    void on_actionShow_Curvature_triggered(bool b);
    void on_actionShow_Cluster_Ids_triggered(bool b);

    void on_actionSave_triggered();
    void on_actionRecall_triggered();
    void on_actionDump_triggered();
    void on_actionUndump_triggered();
    void on_actionSave_as_Png_triggered();
    void on_actionLoad_from_Png_triggered();
    void on_actionSave_as_Movie_Pngs_triggered();

    void on_actionExport_as_Matlab_Structure_triggered();

    void on_actionShow_Slice_triggered(bool b);
    void on_actionAbout_triggered();

    void on_numInitialPushButton_clicked();

    void on_actionCopy_to_Clipboard_triggered();

    void savePng(QString filename);

    void changeDiffusion(int component, double value);
    void changeDiffusion(double value);

    void changeParameter(double value);
    void changeParameter(Parameter< double >* p, double value);

    void changeModel(std::string m);
    bool adaptationMode();
    void adaptationMode(bool b);

    void updateColourScheme(QAction* a);
    void updateColourSchemeMode(QAction* a);

    void adaptRange(uint& component, bool& isFft);

    //void adaptValues();
    void updateUpdatePeriod(QAction* a);

    void exportAsMatlabStructure(QString fileName, QString structureName, int timeIndex,
                                 bool append = true);
    signals:
    void updateParameters();
    void replotTab();
    void colorMapChanged(const QwtColorMap&);

private:
    bool showClusterIds_;

    bool adaptationMode_;

    // Plotting Elements

    QList< QwtPlotCurve* > curves;

    QVector< QwtPlotCurve* > slice;//[ ModelLattice::number_of_Variables ];
    QVector< QwtPlotMarker* > qMarkerVector; // Markiert die Cluster
    QMap< long int, QList< QPair< SurfacePoint, double > > > bufferMap; // + timestamp
    QMap< long int, QList< QPair< SurfacePoint, double > > > temporaryBufferMap;
    QMap< long int, SurfacePoint > lastStepClusters;
    double lastClustersUpdateTime;

    QMainWindow* parent;

    QwtLinearColorMap colorMap;

    void setTitle();

    // FHNModel *fhnmodel;

    ModelLattice * lattice;
    std::string latticeIdentifier_;

protected:
    void closeEvent(QCloseEvent * event);
public:

    QVector< QwtDoubleInterval > plotRanges_real, plotRanges_fft;
private:
    QList< QAction* > actionLatticeModels;

    QVector< PlotView* > plotViewVector_;

    ScriptEditor* scriptEditor;
    DefectsEditor* defectsEditor;
    QList< Defect< GeneralComponentSystem > > defectsList;

    QMap< QString, Parameter< double >* > latticeParameters;
    QMap< QString, Parameter< double >* > latticeAdaptationParameters;

    QQueue< QString > movieQueue;
    int realSize_, latticeSize_;

    void initField(int realSize, int latticeSize, std::string model);
    void killField();

    //QVector< QDoubleSpinBox* > diffusionBoxes;//[ ModelLattice::number_of_Variables ];
    QDoubleSpinBox* diffusionBox;
    //QTabWidget* plotTabWidget;

    QMap< int, QString > boundaryConditionIdentifier;

    void setUpColourSchemeMenu();

    QActionGroup* changeColourSchemeGroup;
    QActionGroup* changeColourSchemeModeGroup;

    QActionGroup* updatePeriodGroup;

    int updatePeriodTime_;
    void setUpUpdatePeriodMenu();

    enum ColourMapModes {
        defaultColourMapMode, adaptiveColourMapMode, delayedAdaptiveColourMapMode
    };

    ColourMapModes colourMapMode;
    enum ColourMapTypes {
        standardColourMap, greyColourMap, jetColourMap
    };

    ColourMapTypes colourMapType;
    QCheckBox *adaptationModeCheckBox;

    QClipboard *clipboard;

    QString matlabExportFile_;
    int matlabExportIndex_;

    void writeSettings();
    void readSettings();

};

#endif

