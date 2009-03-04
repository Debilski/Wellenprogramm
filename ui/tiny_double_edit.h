/*
 * tiny_double_edit.h
 *
 *  Created on: 03.03.2009
 *      Author: rikebs
 */

#ifndef TINY_DOUBLE_EDIT_H_
#define TINY_DOUBLE_EDIT_H_
#include <stdexcept>
#include "ui_tiny_double_edit.h"

class TinyDoubleEdit : public QDialog, private Ui::tinyDoubleEdit {
Q_OBJECT
public:
    TinyDoubleEdit(QWidget* parent = 0, double value = 0);
    double value();
    double value( double defaultValue );

};

class UninitialisedValueException : public std::runtime_error {
public:
    UninitialisedValueException() :
        std::runtime_error( "Uninitialised Value" )
    {
    }
};

#endif /* TINY_DOUBLE_EDIT_H_ */
