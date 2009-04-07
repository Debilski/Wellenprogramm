/*
 * lattice_scripter.cpp
 *
 *  Created on: 13.03.2009
 *      Author: rikebs
 */

#include "lattice_scripter.h"

#include <qscriptengine.h>

class LatticeScripter::PrivateData {
public:
    QScriptEngine engine;
};

LatticeScripter::LatticeScripter(QObject* latticeController) :
    QObject( latticeController ), d_data( new PrivateData )
{
    QScriptValue objectValue = d_data->engine.newQObject( latticeController );
    d_data->engine.globalObject().setProperty( "lattice", objectValue );
}

LatticeScripter::~LatticeScripter()
{
    delete d_data;
}

QScriptValue LatticeScripter::evaluate(const QString& program)
{
    QScriptValue res = d_data->engine.evaluate( program );
    emit result(res);
    return res;
}
