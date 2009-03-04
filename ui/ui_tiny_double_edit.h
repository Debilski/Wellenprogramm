/********************************************************************************
** Form generated from reading ui file 'tiny_double_edit.ui'
**
** Created: Wed Mar 4 23:06:09 2009
**      by: Qt User Interface Compiler version 4.4.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_TINY_DOUBLE_EDIT_H
#define UI_TINY_DOUBLE_EDIT_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_tinyDoubleEdit
{
public:
    QVBoxLayout *verticalLayout;
    QDoubleSpinBox *doubleEditSpinBox;

    void setupUi(QDialog *tinyDoubleEdit)
    {
    if (tinyDoubleEdit->objectName().isEmpty())
        tinyDoubleEdit->setObjectName(QString::fromUtf8("tinyDoubleEdit"));
    tinyDoubleEdit->setWindowModality(Qt::WindowModal);
    tinyDoubleEdit->resize(127, 44);
    verticalLayout = new QVBoxLayout(tinyDoubleEdit);
    verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
    doubleEditSpinBox = new QDoubleSpinBox(tinyDoubleEdit);
    doubleEditSpinBox->setObjectName(QString::fromUtf8("doubleEditSpinBox"));
    doubleEditSpinBox->setDecimals(3);
    doubleEditSpinBox->setMinimum(-99.999);
    doubleEditSpinBox->setMaximum(99.999);
    doubleEditSpinBox->setSingleStep(0.5);

    verticalLayout->addWidget(doubleEditSpinBox);


    retranslateUi(tinyDoubleEdit);
    QObject::connect(doubleEditSpinBox, SIGNAL(editingFinished()), tinyDoubleEdit, SLOT(accept()));

    QMetaObject::connectSlotsByName(tinyDoubleEdit);
    } // setupUi

    void retranslateUi(QDialog *tinyDoubleEdit)
    {
    tinyDoubleEdit->setWindowTitle(QApplication::translate("tinyDoubleEdit", "Dialog", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(tinyDoubleEdit);
    } // retranslateUi

};

namespace Ui {
    class tinyDoubleEdit: public Ui_tinyDoubleEdit {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TINY_DOUBLE_EDIT_H
