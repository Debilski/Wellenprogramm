/*
 * lattice_scripter.h
 *
 *  Created on: 12.03.2009
 *      Author: rikebs
 */

#ifndef LATTICE_SCRIPTER_H_
#define LATTICE_SCRIPTER_H_

#include <QtCore>
#include <QtScript>

class LatticeScripter : public QObject {
Q_OBJECT
public:
    LatticeScripter(QObject* latticeController);
public slots:
    QScriptValue evaluate(const QString& program);
    signals:
    void result( QScriptValue );
private:
    QScriptEngine engine_;
};

#endif /* LATTICE_SCRIPT_H_ */
