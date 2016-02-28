/*****************************************************************************
 * RDS Lattice Library / ‘Wellenprogramm’
 * Copyright (C) 2008–2009  Rike-Benjamin Schuppner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Public License, Version 2.0
 *****************************************************************************/
/*
 * script_editor.h
 *
 *  Created on: 02.03.2009
 *      Author: rikebs
 */

#ifndef SCRIPT_EDITOR_H_
#define SCRIPT_EDITOR_H_

#include <QtGui>

#include "lattice_controller.h"
#include "lattice_scripter.h"

#include "ui_script_editor.h"

/*
 * TODO
 * Trennung von UI und „Maschinerie“. D.h., Engine ebenfalls auslagern.
 */

class ScriptEditor : public QDialog, private Ui::scriptEditor
{
    Q_OBJECT
public:
    ScriptEditor(QWidget* parent, LatticeController* latticeController);

public slots:
    void executeLoopScript();
    void executeOnceScript();
    void showResult(QScriptValue val);

protected:
    void closeEvent(QCloseEvent* event);

private:
    void readSettings();
    void writeSettings();
    void initEngine();

    LatticeScripter* latticeScripter_;
};

#endif /* SCRIPT_EDITOR_H_ */
