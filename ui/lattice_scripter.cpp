/*
 * lattice_scripter.cpp
 *
 *  Created on: 13.03.2009
 *      Author: rikebs
 */

#include "lattice_scripter.h"

LatticeScripter::LatticeScripter(QObject* latticeController) :
    QObject( latticeController )
{
    QScriptValue objectValue = engine_.newQObject( latticeController );
    engine_.globalObject().setProperty( "lattice", objectValue );
}

QScriptValue LatticeScripter::evaluate(const QString& program)
{
    QScriptValue res = engine_.evaluate( program );
    emit result(res);
}
