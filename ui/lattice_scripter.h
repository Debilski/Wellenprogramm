/*
 * lattice_scripter.h
 *
 *  Created on: 12.03.2009
 *      Author: rikebs
 */

#ifndef LATTICE_SCRIPTER_H_
#define LATTICE_SCRIPTER_H_

#include <qobject.h>
#include <qscriptvalue.h>

class QString;

class LatticeScripter : public QObject {
Q_OBJECT
public:
    LatticeScripter(QObject* latticeController);
    ~LatticeScripter();
public slots:
    QScriptValue evaluate(const QString& program);
    signals:
    void result( QScriptValue );
private:
    class PrivateData;
    PrivateData* d_data;
};

#endif /* LATTICE_SCRIPT_H_ */
