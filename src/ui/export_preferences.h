/*****************************************************************************
 * RDS Lattice Library / ‘Wellenprogramm’
 * Copyright (C) 2008–2009  Rike-Benjamin Schuppner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Public License, Version 2.0
 *****************************************************************************/
/*
 * export_preferences.h
 *
 *  Created on: 04.06.2009
 *      Author: rikebs
 */

#ifndef EXPORT_PREFERENCES_H_
#define EXPORT_PREFERENCES_H_

#include "preference_pager.h"

class QWidget;
class QAction;
class QShowEvent;

class ExportPreferences : public PreferencePager
{
    Q_OBJECT
public:
    ExportPreferences(QWidget* parent = 0);
    bool hasPngExportComponentChecked(uint component, bool raw) const;
    bool hasMatlabExportComponentChecked(uint component) const;
public slots:
    void setViewNames(const QStringList& names);

signals:
    void pngChecked(QVector<QPair<bool, bool>>);
    void matlabChecked(QVector<bool>);

private:
    class PrivateData;
    PrivateData* d_data;
    QWidget* pngExport;
};

#endif /* EXPORT_PREFERENCES_H_ */
