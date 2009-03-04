/********************************************************************************
** Form generated from reading ui file 'script_editor.ui'
**
** Created: Wed Mar 4 23:06:09 2009
**      by: Qt User Interface Compiler version 4.4.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_SCRIPT_EDITOR_H
#define UI_SCRIPT_EDITOR_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QDialog>
#include <QtGui/QHBoxLayout>
#include <QtGui/QPlainTextEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QSplitter>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_scriptEditor
{
public:
    QVBoxLayout *verticalLayout;
    QSplitter *outputSplitter;
    QSplitter *inputSplitter;
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout_2;
    QPlainTextEdit *loopScriptEdit;
    QCheckBox *loopScriptCheckBox;
    QWidget *layoutWidget1;
    QVBoxLayout *verticalLayout_3;
    QPlainTextEdit *onceScriptEdit;
    QHBoxLayout *horizontalLayout_4;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *onceScriptPushButton;
    QPlainTextEdit *outputScriptEdit;

    void setupUi(QDialog *scriptEditor)
    {
    if (scriptEditor->objectName().isEmpty())
        scriptEditor->setObjectName(QString::fromUtf8("scriptEditor"));
    scriptEditor->resize(667, 302);
    verticalLayout = new QVBoxLayout(scriptEditor);
    verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
    outputSplitter = new QSplitter(scriptEditor);
    outputSplitter->setObjectName(QString::fromUtf8("outputSplitter"));
    outputSplitter->setOrientation(Qt::Vertical);
    inputSplitter = new QSplitter(outputSplitter);
    inputSplitter->setObjectName(QString::fromUtf8("inputSplitter"));
    inputSplitter->setLineWidth(5);
    inputSplitter->setMidLineWidth(5);
    inputSplitter->setOrientation(Qt::Horizontal);
    layoutWidget = new QWidget(inputSplitter);
    layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
    verticalLayout_2 = new QVBoxLayout(layoutWidget);
    verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
    verticalLayout_2->setContentsMargins(0, 0, 0, 0);
    loopScriptEdit = new QPlainTextEdit(layoutWidget);
    loopScriptEdit->setObjectName(QString::fromUtf8("loopScriptEdit"));

    verticalLayout_2->addWidget(loopScriptEdit);

    loopScriptCheckBox = new QCheckBox(layoutWidget);
    loopScriptCheckBox->setObjectName(QString::fromUtf8("loopScriptCheckBox"));

    verticalLayout_2->addWidget(loopScriptCheckBox);

    inputSplitter->addWidget(layoutWidget);
    layoutWidget1 = new QWidget(inputSplitter);
    layoutWidget1->setObjectName(QString::fromUtf8("layoutWidget1"));
    verticalLayout_3 = new QVBoxLayout(layoutWidget1);
    verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
    verticalLayout_3->setContentsMargins(0, 0, 0, 0);
    onceScriptEdit = new QPlainTextEdit(layoutWidget1);
    onceScriptEdit->setObjectName(QString::fromUtf8("onceScriptEdit"));

    verticalLayout_3->addWidget(onceScriptEdit);

    horizontalLayout_4 = new QHBoxLayout();
    horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
    horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    horizontalLayout_4->addItem(horizontalSpacer_2);

    onceScriptPushButton = new QPushButton(layoutWidget1);
    onceScriptPushButton->setObjectName(QString::fromUtf8("onceScriptPushButton"));

    horizontalLayout_4->addWidget(onceScriptPushButton);


    verticalLayout_3->addLayout(horizontalLayout_4);

    inputSplitter->addWidget(layoutWidget1);
    outputSplitter->addWidget(inputSplitter);
    outputScriptEdit = new QPlainTextEdit(outputSplitter);
    outputScriptEdit->setObjectName(QString::fromUtf8("outputScriptEdit"));
    outputScriptEdit->setReadOnly(true);
    outputSplitter->addWidget(outputScriptEdit);

    verticalLayout->addWidget(outputSplitter);


    retranslateUi(scriptEditor);

    QMetaObject::connectSlotsByName(scriptEditor);
    } // setupUi

    void retranslateUi(QDialog *scriptEditor)
    {
    scriptEditor->setWindowTitle(QApplication::translate("scriptEditor", "Dialog", 0, QApplication::UnicodeUTF8));
    loopScriptCheckBox->setText(QApplication::translate("scriptEditor", "Loop", 0, QApplication::UnicodeUTF8));
    onceScriptPushButton->setText(QApplication::translate("scriptEditor", "Once", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(scriptEditor);
    } // retranslateUi

};

namespace Ui {
    class scriptEditor: public Ui_scriptEditor {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SCRIPT_EDITOR_H
