/*
 * resetable_label.h
 *
 *  Created on: 04.04.2009
 *      Author: rikebs
 */

#ifndef RESETABLE_LABEL_H_
#define RESETABLE_LABEL_H_

#include <qlabel.h>

class ResetableLabel : public QLabel {
Q_OBJECT
public:
    ResetableLabel(QWidget * parent = 0, Qt::WindowFlags f = 0);
    ResetableLabel(const QString & text, QWidget * parent = 0, Qt::WindowFlags f = 0);
    ~ResetableLabel();
signals:
    void reset();
private:
    class PrivateData;
    PrivateData* d_data;

private slots:
    void showResetMenu(const QPoint& p);
};

#endif /* RESETTABLE_LABEL_H_ */
