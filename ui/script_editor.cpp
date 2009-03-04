/*
 * script_editor.cpp
 *
 *  Created on: 02.03.2009
 *      Author: rikebs
 */

#include "script_editor.h"

ScriptEditor::ScriptEditor(QWidget* parent) :
    QDialog( parent )
{
    setupUi( this );
    readSettings();
}

void ScriptEditor::closeEvent(QCloseEvent * event)
{
    writeSettings();
    event->accept();
}

void ScriptEditor::readSettings()
{
    QSettings settings;
    settings.beginGroup( "ScriptEditor" );
    restoreGeometry( settings.value( "geometry" ).toByteArray() );
    //resize(settings.value("size", QSize(400, 400)).toSize());
    //move(settings.value("pos", QPoint(200, 200)).toPoint());

    // Restore splitter sizes
    QVariant variant = settings.value( "inputSplitter" );
    if ( variant.isValid() ) {
        QVariantList splitters = variant.toList();
        QList< int > splittersL;
        foreach (QVariant var, splitters)
                splittersL += var.toInt();

        inputSplitter->setSizes( splittersL );
    }

    variant.clear();
    variant = settings.value( "outputSplitter" );
    if ( variant.isValid() ) {
        QVariantList splitters = variant.toList();
        QList< int > splittersL;
        foreach (QVariant var, splitters)
                splittersL += var.toInt();

        outputSplitter->setSizes( splittersL );
    }

    settings.endGroup();

}

void ScriptEditor::writeSettings()
{
    QSettings settings;
    settings.beginGroup( "ScriptEditor" );
    //    settings.setValue("size", size());
    //    settings.setValue("pos", pos());
    settings.setValue( "geometry", saveGeometry() );

    // Save splitter sizes
    QVariantList splitters;
    foreach (int i, inputSplitter->sizes())
            splitters.append( i );
    settings.setValue( "inputSplitter", splitters );
    splitters.clear();
    foreach (int i, outputSplitter->sizes())
            splitters.append( i );
    settings.setValue( "outputSplitter", splitters );
    splitters.clear();

    settings.endGroup();

}
