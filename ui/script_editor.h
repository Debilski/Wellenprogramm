/*
 * script_editor.h
 *
 *  Created on: 02.03.2009
 *      Author: rikebs
 */

#ifndef SCRIPT_EDITOR_H_
#define SCRIPT_EDITOR_H_

#include <QtGui>
#include <QtScript>

#include "lattice_scripter.h"

#include "ui_script_editor.h"

/*
 * TODO
 * Trennung von UI und „Maschinerie“. D.h., Engine ebenfalls auslagern.
 */

class ScriptEditor : public QDialog, private Ui::scriptEditor {
Q_OBJECT
public:
    ScriptEditor(QWidget* parent = 0);

public slots:
    void executeLoopScript();
    void executeOnceScript();

protected:
    void closeEvent ( QCloseEvent* event );

private:
    void readSettings();
    void writeSettings();
    void initEngine();

    LatticeScripter* latticeScripter_;
    QScriptEngine engine_;
};

#endif /* SCRIPT_EDITOR_H_ */
