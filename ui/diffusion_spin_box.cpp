/*
 * diffusion_spin_box.cpp
 *
 *  Created on: 06.03.2009
 *      Author: rikebs
 */

#include "diffusion_spin_box.h"

DiffusionSpinBox::DiffusionSpinBox(int diffusionIndex, QWidget* parent /*= 0*/) :
    QDoubleSpinBox( parent ), index_( diffusionIndex )
{
    connect( this, SIGNAL(valueChanged(const double&)), this, SLOT(emitChanged(const double&)) );
}

DiffusionSpinBox::~DiffusionSpinBox()
{
}

void DiffusionSpinBox::emitChanged(const double& val)
{
    emit valueChanged(index_, val);
}
