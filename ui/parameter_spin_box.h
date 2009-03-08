/*
 * parameter_spin_box.h
 *
 *  Created on: 06.03.2009
 *      Author: rikebs
 */

#ifndef PARAMETER_SPIN_BOX_H_
#define PARAMETER_SPIN_BOX_H_

#include <QtGui>

#include "lattice_interface.h"

class ParameterSpinBox : public QDoubleSpinBox {
Q_OBJECT
public:
    ParameterSpinBox(Parameter< double >* parameterReference, QWidget* parent = 0);
    void updateReference(Parameter< double >* parameterReference);
public slots:
    void updateValue();
    void update();

private:
    Parameter< double >* parameter_;
    QString parameterName_;
};

#endif /* PARAMETER_SPIN_BOX_H_ */
