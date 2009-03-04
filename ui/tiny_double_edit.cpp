/*
 * tiny_double_edit.cpp
 *
 *  Created on: 03.03.2009
 *      Author: rikebs
 */

#include "tiny_double_edit.h"

TinyDoubleEdit::TinyDoubleEdit(QWidget* parent, double value) :
    QDialog( parent )
{
    setupUi( this );
    doubleEditSpinBox->setValue( value );
}

double TinyDoubleEdit::value()
{
    if ( result() != QDialog::Accepted ) {
        throw UninitialisedValueException();
    }
    return doubleEditSpinBox->value();
}

double TinyDoubleEdit::value(double defaultValue)
{
    if ( result() != QDialog::Accepted ) {
        return defaultValue;
    }
    return doubleEditSpinBox->value();
}
