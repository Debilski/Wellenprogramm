/*****************************************************************************
 * RDS Lattice Library / ‘Wellenprogramm’
 * Copyright (C) 2008–2009  Rike-Benjamin Schuppner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Public License, Version 2.0
 *****************************************************************************/
/*
 * preference_pager.h
 *
 *  Created on: 04.06.2009
 *      Author: rikebs
 */

#ifndef PREFERENCE_PAGER_H_
#define PREFERENCE_PAGER_H_

#include <QtGui/QMainWindow>

class QWidget;
class QString;
class QIcon;

/*!
 * Stellt eine OS-X-ähnliche Preferences-Umgebung her, mit einzelnen Seiten, welche durch Buttons aufgerufen werden können.
 */

class PreferencePager : public QMainWindow
{
    Q_OBJECT
public:
    PreferencePager(QWidget* parent = 0);

    int addPage(QWidget* page, const QString& label);
    int addPage(QWidget* page, const QIcon& icon, const QString& label);
public slots:
    void resizeToMinimalSize();
    QAction* toggleViewAction() const;
    void toggleView(bool);

signals:
    void pngChecked(QVector<QPair<bool, bool> >);
    void matlabChecked();
    void changed();

protected slots:
    void showEvent(QShowEvent* event);
    void closeEvent(QCloseEvent* event);

private:
    class PrivateData;
    PrivateData* d_data;
};

#endif /* PREFERENCE_PAGER_H_ */
