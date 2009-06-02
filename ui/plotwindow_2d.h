/*****************************************************************************
 * RDS Lattice Library / ‘Wellenprogramm’
 * Copyright (C) 2008–2009  Rike-Benjamin Schuppner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Public License, Version 2.0
 *****************************************************************************/
#ifndef PLOTWINDOW_2D_H
#define PLOTWINDOW_2D_H


#include "ui_plotwindow_2d.h"

#include <memory>

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_spectrogram.h>

#include <qwt_scale_widget.h>
#include <qwt_scale_draw.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_layout.h>

#include <qwt_symbol.h>

#include "defects_editor.h"
#include "script_editor.h"
#include "plot_view.h"

#include "plot_single.h"

#include "configuration.h"

#include "color_maps.h"

class Waveprogram2DPlot : public QMainWindow, private Ui::plotWindow_2d {
Q_OBJECT
public:
    Waveprogram2DPlot(QMainWindow * parent = 0);
    ~Waveprogram2DPlot();

    void setUpDockWindows();

    void setUpSizeMenu();
    void setUpActions();
    void setUpColorMap();
    void reorderTabs();
    void setUpViews();
    void setUpRanges();

    void setUpModelProperties();
    void setUpDiffusion();
    void setUpParameters();
    void setUpAdaptationParameters();
    void setUpBoundaryConditionsSelector();

    void showSlicePlot();
    void showSinglePlot();

    QList< QwtPlotMarker* > plotMarkers();
    QList< QwtPlotCurve* > plotCurves();

    void updatePlotAnnotations();

public slots:

    void replot();

    void toggleStartStop();

    void updateSizeMenu();

    void updateDefects();
    void updateLabels();


    void changeSize( const QString& size );

    void on_noiseSpinBox_valueChanged(double d);

    void on_boundaryConditionsComboBox_currentIndexChanged(int i);

    void on_correlationSpinBox_valueChanged(int d);
    void on_timestepSpinBox_valueChanged(double d);


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

    void on_actionShow_Single_Plot_triggered();

    void on_numInitialPushButton_clicked();

    void on_actionCopy_to_Clipboard_triggered();

    void savePng(QString filename);

    void changeDiffusion(int component, double value);

    void changeParameter(const QString& paramName, double value);
    void changeParameter(Parameter< double >* p, double value);

    void changeModel(const QString& modelName);
    void resizeWindowToForceUpdate();

    void updateColorScheme(QAction* a);
    void updateColorSchemeMode(QAction* a);

    //void adaptValues();
    void updateUpdatePeriod(QAction* a);

    void exportAsMatlabStructure(QString fileName, QString structureName, int timeIndex,
                                 bool append = true);

    void setUpParameterSets();
    void updateParametersToSet(int setNum);
    void on_parameterSetsDropDown_currentIndexChanged(int index);
    void on_parameterSetsSave_clicked();
    void on_parameterSetsDelete_clicked();

    void resetTime();
    void paint(const uint& component, const QPointF& point) ;

    void movieExport();

signals:

    void updateParameters();

    void replotTab();

    void colorMapChanged(const QwtColorMap&);
    void colorMapModeChanged(ColorMapAdaptationModes);
    void colorMapChanged(const QwtColorMap&, ColorMapAdaptationModes);

    void modelChanged();
    void modelClosed();

    void replotAllChildren();

protected:
    void closeEvent(QCloseEvent * event);
    void saveViews(const QString& name, uint num);
    void saveViews(const QString& name);

private:
    bool showClusterIds_;

    // Plotting Elements


    QVector< QwtPlotCurve* > slice;//[ ModelLattice::number_of_Variables ];

    QMap< long int, QList< QPair< SurfacePoint, double > > > bufferMap; // + timestamp
    QMap< long int, QList< QPair< SurfacePoint, double > > > temporaryBufferMap;
    QMap< long int, SurfacePoint > lastStepClusters;
    double lastClustersUpdateTime;

    QMainWindow* parent;

    void setTitle();

    QStringList recentSizes;
    QSignalMapper* sizeMapper;

    // FHNModel *fhnmodel;

    std::auto_ptr< LatticeController > lc_;
    LatticeController* latticeController_;

    ColorMaps colorMaps_;

    QList< QAction* > actionLatticeModels;

    QVector< PlotView* > plotViewVector_;

    ScriptEditor* scriptEditor;
    QPointer< DefectsEditor > defectsEditor;
    QList< Defect< GeneralComponentSystem > > defectsList;

    typedef QMap< QString, Parameter< double >* > ParameterMap;
    typedef QMap< QString, double > ParameterValueMap;

    ParameterMap latticeParameters;
    ParameterMap latticeAdaptationParameters;

    QVector< ParameterValueMap > savedParameterSets;

    QQueue< QString > movieQueue;

    void initField(int realSizeX, int realSizeY, int latticeSizeX, int latticeSizeY, const QString& model);
    void killField();

    //QVector< QDoubleSpinBox* > diffusionBoxes;//[ ModelLattice::number_of_Variables ];
    QDoubleSpinBox* diffusionBox;
    //QTabWidget* plotTabWidget;

    QMap< int, QString > boundaryConditionIdentifier;

    void setUpColorSchemeMenu();

    QAction* createLatticeWithNewSize;

    QActionGroup* changeColorSchemeGroup;
    QActionGroup* changeColorSchemeModeGroup;

    QActionGroup* updatePeriodGroup;

    int updatePeriodTime_;
    void setUpUpdatePeriodMenu();

    ColorMapAdaptationModes colorMapMode;

    QCheckBox *adaptationModeCheckBox;

    QClipboard *clipboard;

    QString matlabExportFile_;
    int matlabExportIndex_;

    void writeSettings();
    void readSettings();

    void readParameterSets();
    void writeParameterSets();

    class PrivateData;
    PrivateData* d_data;

};

#endif

