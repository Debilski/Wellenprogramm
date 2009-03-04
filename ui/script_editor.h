/*
 * script_editor.h
 *
 *  Created on: 02.03.2009
 *      Author: rikebs
 */

#ifndef SCRIPT_EDITOR_H_
#define SCRIPT_EDITOR_H_

#include <QtGui>
#include "ui_script_editor.h"

class ScriptEditor : public QDialog, private Ui::scriptEditor {
Q_OBJECT
public:
    ScriptEditor(QWidget* parent = 0);

protected:
    void closeEvent ( QCloseEvent* event );

private:
    void readSettings();
    void writeSettings();
};

#endif /* SCRIPT_EDITOR_H_ */
