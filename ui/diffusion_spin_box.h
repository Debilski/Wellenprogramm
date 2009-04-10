/*
 * diffusion_spin_box.h
 *
 *  Created on: 06.03.2009
 *      Author: rikebs
 */

#ifndef DIFFUSION_SPIN_BOX_H_
#define DIFFUSION_SPIN_BOX_H_

#include <qspinbox.h>

class DiffusionSpinBox : public QDoubleSpinBox {
Q_OBJECT
public:
    DiffusionSpinBox(int diffusionIndex, QWidget* parent = 0);
    ~DiffusionSpinBox();
signals:
    void valueChanged(int, const double&);
private:
    int index_;
private slots:
    void emitChanged(const double& val);
};

#endif /* DIFFUSION_SPIN_BOX_H_ */
