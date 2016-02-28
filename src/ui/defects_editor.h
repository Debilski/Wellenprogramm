/*****************************************************************************
 * RDS Lattice Library / ‘Wellenprogramm’
 * Copyright (C) 2008–2009  Rike-Benjamin Schuppner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Public License, Version 2.0
 *****************************************************************************/
#ifndef DEFECTS_EDITOR_H
#define DEFECTS_EDITOR_H

#include "ui_defects_editor.h"

#include <QtCore>
#include <qitemdelegate.h>

#include "lattice_interface.h"

class QMainWindow;

class DefectsModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    QList<Defect<GeneralComponentSystem>> defectsList;
    QMap<int, QString> boundaryConditionsIdentifier;

    DefectsModel(const QList<Defect<GeneralComponentSystem>>& defects,
        const QMap<int, QString>& boundaryConditionsIdentifier, QObject* parent = 0);

    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    int columnCount(const QModelIndex& parent = QModelIndex()) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex& index) const;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);
    bool insertRows(int position, int rows, const QModelIndex& index = QModelIndex());
    bool removeRows(int position, int rows, const QModelIndex& index = QModelIndex());
    bool moveUpRow(int position, int rowsUp, const QModelIndex& index = QModelIndex());
    bool moveDownRow(int position, int rowsDown, const QModelIndex& index = QModelIndex());
};

class DefectsEditor : public QDialog, private Ui::defectsEditor
{
    Q_OBJECT
public:
    DefectsEditor(const QList<Defect<GeneralComponentSystem>>& defects,
        QMap<int, QString>& boundaryConditionsIdentifier, QMainWindow* parent = 0);
    ~DefectsEditor();

    QMap<int, QString> boundaryConditionsIdentifier;
    QList<Defect<GeneralComponentSystem>> defects;
    DefectsModel* defectsModel;
public slots:
    void updateData(const QModelIndex&, const QModelIndex&);
    void insertRow();
    void removeRow();
    void moveDown();
    void moveUp();
};

class TextSelectionDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    TextSelectionDelegate(const QMap<int, QString>& boundaryConditionsIdentifier, QObject* parent = 0);
    QMap<int, QString> boundaryConditionsIdentifier;
    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
        const QModelIndex& index) const;

    void setEditorData(QWidget* editor, const QModelIndex& index) const;
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const;

    void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option,
        const QModelIndex& index) const;
};

#endif
