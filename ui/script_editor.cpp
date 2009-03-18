/*
 * script_editor.cpp
 *
 *  Created on: 02.03.2009
 *      Author: rikebs
 */

#include "script_editor.h"

ScriptEditor::ScriptEditor(QWidget* parent, LatticeController* latticeController) :
    QDialog( parent ), latticeScripter_( latticeController->getLatticeScripter() )
{
    setupUi( this );
    readSettings();
    connect( onceScriptPushButton, SIGNAL( clicked() ), this, SLOT( executeOnceScript() ) );
    connect(
        latticeScripter_, SIGNAL( result( QScriptValue )), this,
        SLOT( showResult( QScriptValue ) ) );
}

void ScriptEditor::showResult( QScriptValue val)
{
    outputScriptEdit->appendPlainText( val.toVariant().toString() );
}

void ScriptEditor::closeEvent(QCloseEvent* event)
{
    writeSettings();
    event->accept();
}

void ScriptEditor::executeLoopScript()
{
    if ( !loopScriptCheckBox->isChecked() )
        return;
    QString programm = loopScriptEdit->toPlainText();
    latticeScripter_->evaluate( programm );
}

void ScriptEditor::executeOnceScript()
{
    qDebug() << "Script Once";
    QString programm = onceScriptEdit->toPlainText();
    qDebug() << latticeScripter_->evaluate( programm ).toVariant();
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

    settings.beginGroup( "Scripts" );
    variant.clear();
    variant = settings.value( "executeLoop" );
    if ( variant.isValid() ) {
        QString text = variant.toString();
        loopScriptEdit->setPlainText( text );
    }

    variant.clear();
    variant = settings.value( "executeOnce" );
    if ( variant.isValid() ) {
        QString text = variant.toString();
        onceScriptEdit->setPlainText( text );
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

    settings.beginGroup( "Scripts" );
    settings.setValue( "executeLoop", loopScriptEdit->toPlainText() );
    settings.setValue( "executeOnce", onceScriptEdit->toPlainText() );

    settings.endGroup();

}
