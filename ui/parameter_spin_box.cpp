/*
 * parameter_spin_box.cpp
 *
 *  Created on: 06.03.2009
 *      Author: rikebs
 */

#include "parameter_spin_box.h"

ParameterSpinBox::ParameterSpinBox(Parameter< double >* parameterReference, QWidget* parent /*= 0*/) :
    QDoubleSpinBox( parent ), parameter_( parameterReference )
{
    parameterName_ = QString::fromUtf8( parameter_->name.c_str() );
    update();
}
void ParameterSpinBox::updateReference(Parameter< double >* parameterReference)
{
    parameter_ = parameterReference;
    parameterName_ = QString::fromUtf8( parameter_->name.c_str() );
    update();
}

void ParameterSpinBox::updateValue()
{
    double p = parameter_->get();
    if ( p != this->value() )
        this->setValue( p );
}
void ParameterSpinBox::update()
{
    this->setProperty( "parameter", QVariant( parameterName_ ) );
    this->setDecimals( parameter_->decimals() );
    this->setMaximum( parameter_->max() );
    this->setMinimum( parameter_->min() );
    this->setSingleStep( parameter_->stepSizeHint() );
    this->setValue( parameter_->get() );
}
