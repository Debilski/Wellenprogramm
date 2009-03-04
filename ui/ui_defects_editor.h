/********************************************************************************
** Form generated from reading ui file 'defects_editor.ui'
**
** Created: Wed Mar 4 23:06:09 2009
**      by: Qt User Interface Compiler version 4.4.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_DEFECTS_EDITOR_H
#define UI_DEFECTS_EDITOR_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QTableView>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_defectsEditor
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QTableView *tableView;
    QVBoxLayout *verticalLayout_3;
    QPushButton *upButton;
    QPushButton *downButton;
    QPushButton *insertButton;
    QPushButton *deleteButton;
    QSpacerItem *verticalSpacer;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *defectsEditor)
    {
    if (defectsEditor->objectName().isEmpty())
        defectsEditor->setObjectName(QString::fromUtf8("defectsEditor"));
    defectsEditor->resize(642, 260);
    verticalLayout = new QVBoxLayout(defectsEditor);
    verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
    horizontalLayout = new QHBoxLayout();
    horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
    tableView = new QTableView(defectsEditor);
    tableView->setObjectName(QString::fromUtf8("tableView"));

    horizontalLayout->addWidget(tableView);

    verticalLayout_3 = new QVBoxLayout();
    verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
    upButton = new QPushButton(defectsEditor);
    upButton->setObjectName(QString::fromUtf8("upButton"));

    verticalLayout_3->addWidget(upButton);

    downButton = new QPushButton(defectsEditor);
    downButton->setObjectName(QString::fromUtf8("downButton"));

    verticalLayout_3->addWidget(downButton);

    insertButton = new QPushButton(defectsEditor);
    insertButton->setObjectName(QString::fromUtf8("insertButton"));

    verticalLayout_3->addWidget(insertButton);

    deleteButton = new QPushButton(defectsEditor);
    deleteButton->setObjectName(QString::fromUtf8("deleteButton"));

    verticalLayout_3->addWidget(deleteButton);

    verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    verticalLayout_3->addItem(verticalSpacer);


    horizontalLayout->addLayout(verticalLayout_3);


    verticalLayout->addLayout(horizontalLayout);

    buttonBox = new QDialogButtonBox(defectsEditor);
    buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Discard|QDialogButtonBox::Save);

    verticalLayout->addWidget(buttonBox);

    QWidget::setTabOrder(buttonBox, tableView);
    QWidget::setTabOrder(tableView, upButton);
    QWidget::setTabOrder(upButton, downButton);
    QWidget::setTabOrder(downButton, insertButton);
    QWidget::setTabOrder(insertButton, deleteButton);

    retranslateUi(defectsEditor);
    QObject::connect(buttonBox, SIGNAL(accepted()), defectsEditor, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), defectsEditor, SLOT(reject()));

    QMetaObject::connectSlotsByName(defectsEditor);
    } // setupUi

    void retranslateUi(QDialog *defectsEditor)
    {
    defectsEditor->setWindowTitle(QApplication::translate("defectsEditor", "Dialog", 0, QApplication::UnicodeUTF8));
    upButton->setText(QApplication::translate("defectsEditor", "up", 0, QApplication::UnicodeUTF8));
    downButton->setText(QApplication::translate("defectsEditor", "down", 0, QApplication::UnicodeUTF8));
    insertButton->setText(QApplication::translate("defectsEditor", "insert", 0, QApplication::UnicodeUTF8));
    deleteButton->setText(QApplication::translate("defectsEditor", "delete", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(defectsEditor);
    } // retranslateUi

};

namespace Ui {
    class defectsEditor: public Ui_defectsEditor {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DEFECTS_EDITOR_H
