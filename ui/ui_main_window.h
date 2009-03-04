/********************************************************************************
** Form generated from reading ui file 'main_window.ui'
**
** Created: Wed Mar 4 23:06:09 2009
**      by: Qt User Interface Compiler version 4.4.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_MAIN_WINDOW_H
#define UI_MAIN_WINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QStatusBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_settingsDLG
{
public:
    QAction *action_Quit;
    QAction *actionAbout;
    QWidget *centralwidget;
    QHBoxLayout *horizontalLayout;
    QGridLayout *gridLayout;
    QPushButton *button2d;
    QMenuBar *menubar;
    QMenu *menuEdit;
    QMenu *menuHelp;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *settingsDLG)
    {
    if (settingsDLG->objectName().isEmpty())
        settingsDLG->setObjectName(QString::fromUtf8("settingsDLG"));
    settingsDLG->resize(214, 100);
    action_Quit = new QAction(settingsDLG);
    action_Quit->setObjectName(QString::fromUtf8("action_Quit"));
    actionAbout = new QAction(settingsDLG);
    actionAbout->setObjectName(QString::fromUtf8("actionAbout"));
    centralwidget = new QWidget(settingsDLG);
    centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
    horizontalLayout = new QHBoxLayout(centralwidget);
    horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
    gridLayout = new QGridLayout();
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    button2d = new QPushButton(centralwidget);
    button2d->setObjectName(QString::fromUtf8("button2d"));

    gridLayout->addWidget(button2d, 0, 0, 1, 1);


    horizontalLayout->addLayout(gridLayout);

    settingsDLG->setCentralWidget(centralwidget);
    menubar = new QMenuBar(settingsDLG);
    menubar->setObjectName(QString::fromUtf8("menubar"));
    menubar->setGeometry(QRect(0, 0, 214, 22));
    menuEdit = new QMenu(menubar);
    menuEdit->setObjectName(QString::fromUtf8("menuEdit"));
    menuHelp = new QMenu(menubar);
    menuHelp->setObjectName(QString::fromUtf8("menuHelp"));
    settingsDLG->setMenuBar(menubar);
    statusBar = new QStatusBar(settingsDLG);
    statusBar->setObjectName(QString::fromUtf8("statusBar"));
    settingsDLG->setStatusBar(statusBar);

    menubar->addAction(menuEdit->menuAction());
    menubar->addAction(menuHelp->menuAction());
    menuEdit->addAction(action_Quit);
    menuHelp->addAction(actionAbout);

    retranslateUi(settingsDLG);
    QObject::connect(action_Quit, SIGNAL(triggered()), settingsDLG, SLOT(close()));

    QMetaObject::connectSlotsByName(settingsDLG);
    } // setupUi

    void retranslateUi(QMainWindow *settingsDLG)
    {
    settingsDLG->setWindowTitle(QApplication::translate("settingsDLG", "MainWindow", 0, QApplication::UnicodeUTF8));
    action_Quit->setText(QApplication::translate("settingsDLG", "&Quit", 0, QApplication::UnicodeUTF8));
    action_Quit->setShortcut(QApplication::translate("settingsDLG", "Ctrl+Q", 0, QApplication::UnicodeUTF8));
    actionAbout->setText(QApplication::translate("settingsDLG", "About", 0, QApplication::UnicodeUTF8));
    button2d->setText(QApplication::translate("settingsDLG", "2D Lattice", 0, QApplication::UnicodeUTF8));
    menuEdit->setTitle(QApplication::translate("settingsDLG", "Edit", 0, QApplication::UnicodeUTF8));
    menuHelp->setTitle(QApplication::translate("settingsDLG", "Help", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class settingsDLG: public Ui_settingsDLG {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAIN_WINDOW_H
