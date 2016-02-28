/*****************************************************************************
 * RDS Lattice Library / ‘Wellenprogramm’
 * Copyright (C) 2008–2009  Rike-Benjamin Schuppner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Public License, Version 2.0
 *****************************************************************************/
#include "defects_editor.h"

#include <QtGui>

DefectsEditor::DefectsEditor(const QList<Defect<GeneralComponentSystem>>& defects, QMap<int,
                                                                                       QString>& boundaryConditionsIdentifier,
    QMainWindow* parent) : QDialog(parent), boundaryConditionsIdentifier(boundaryConditionsIdentifier), defects(defects)
{

    setupUi(this);
    defectsModel = new DefectsModel(defects, boundaryConditionsIdentifier, this);
    TextSelectionDelegate* delegate = new TextSelectionDelegate(boundaryConditionsIdentifier, this);
    tableView->setItemDelegateForColumn(4, delegate);
    tableView->setModel(defectsModel);
    tableView->horizontalHeader()->setStretchLastSection(true);
    tableView->resizeColumnsToContents();
    tableView->resizeRowsToContents();
    connect(defectsModel, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(updateData(const QModelIndex&, const QModelIndex&)));
    connect(insertButton, SIGNAL(clicked()), this, SLOT(insertRow()));
    connect(deleteButton, SIGNAL(clicked()), this, SLOT(removeRow()));
    connect(upButton, SIGNAL(clicked()), this, SLOT(moveUp()));
    connect(downButton, SIGNAL(clicked()), this, SLOT(moveDown()));
}

DefectsEditor::~DefectsEditor()
{
}

void DefectsEditor::updateData(const QModelIndex&, const QModelIndex&)
{
    defects = defectsModel->defectsList;
}

void DefectsEditor::insertRow()
{
    QModelIndex index = tableView->selectionModel()->currentIndex();
    QAbstractItemModel* model = tableView->model();

    if (!model->insertRows(index.row() + 1, 1, index.parent()))
        return;

    //updateActions();

    updateData(index, index);
    /*for (int column = 0; column < model->columnCount( index.parent() ); ++column) {
   QModelIndex child = model->index( index.row() + 1, column, index.parent() );
   model->setData( child, QVariant( "[No data]" ), Qt::EditRole );
   }*/
}

void DefectsEditor::removeRow()
{
    QModelIndex index = tableView->selectionModel()->currentIndex();
    QAbstractItemModel* model = tableView->model();
    if (model->removeRows(index.row(), 1, index.parent())) {
        //updateActions();
        updateData(index, index);
    }
}

void DefectsEditor::moveDown()
{
    QModelIndex index = tableView->selectionModel()->currentIndex();
    DefectsModel* model = static_cast<DefectsModel*>(tableView->model());
    if (model->moveDownRow(index.row(), 1, index.parent())) {
        //updateActions();
        updateData(index, index);
    }
}

void DefectsEditor::moveUp()
{
    QModelIndex index = tableView->selectionModel()->currentIndex();
    DefectsModel* model = static_cast<DefectsModel*>(tableView->model());
    if (model->moveUpRow(index.row(), 1, index.parent())) {
        //updateActions();
        updateData(index, index);
    }
}

DefectsModel::DefectsModel(const QList<Defect<GeneralComponentSystem>>& defects, const QMap<int, QString>& boundaryConditionsIdentifier, QObject* parent) : QAbstractTableModel(parent), defectsList(defects), boundaryConditionsIdentifier(boundaryConditionsIdentifier)
{
}

int DefectsModel::rowCount(const QModelIndex& /*parent = QModelIndex()*/) const
{
    return defectsList.size();
}

int DefectsModel::columnCount(const QModelIndex& /*parent = QModelIndex()*/) const
{
    return 5;
}

QVariant DefectsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {

        switch (section) {
            case (0):
                return QString("x");
            case (1):
                return QString("y");
            case (2):
                return QString("Radius");
            case (3):
                return QString("Values");
            case (4):
                return QString("Boundary Condition");
            default:
                return QVariant();
        }
    } else {
        return QString("Defect %1").arg(section);
    }
}

QVariant DefectsModel::data(const QModelIndex& index, int role /*= Qt::DisplayRole*/) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= defectsList.size() && index.column() > 5)
        return QVariant();

    if (role == Qt::DisplayRole)
        switch (index.column()) {
            case (0):
                return defectsList[index.row()].centre.x;
            case (1):
                return defectsList[index.row()].centre.y;
            case (2):
                return defectsList[index.row()].radius;
            case (3): {
                QString ret = "[";
                for (uint i = 0; i < defectsList[index.row()].value.number_of_Variables; ++i) {
                    ret = ret.append("%1").arg(defectsList[index.row()].value[i]);
                    if (i < defectsList[index.row()].value.number_of_Variables - 1)
                        ret = ret.append(", ");
                }
                ret = ret.append("]");
                return ret;
            }
            /*case(3):
         QVector<double> vec = QVector<double>::fromStdVector( defectsList[ index.row() ].value.toVector() );
         QVector<QVariant> var;
         var = vec;
         return QVariant( var ).toList() );*/
            case (4):
                return boundaryConditionsIdentifier.value(defectsList[index.row()].boundaryCondition);
            default:
                return QVariant();
        }
    else
        return QVariant();
}

Qt::ItemFlags DefectsModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

bool DefectsModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (index.isValid() && role == Qt::EditRole) {

        switch (index.column()) {
            case (0):
                defectsList[index.row()].centre.x = value.toDouble();
                break;
            case (1):
                defectsList[index.row()].centre.y = value.toDouble();
                break;
            case (2):
                defectsList[index.row()].radius = value.toDouble();
                break;
            case (3): {
                QString s = value.toString().replace("[", "").replace("]", "").trimmed();
                QStringList sl = s.split(",");
                QVector<double> vec;
                for (int i = 0; i < sl.size(); ++i) {
                    vec << sl.at(i).toDouble();
                }
                GeneralComponentSystem g = GeneralComponentSystem(vec.toStdVector());
                defectsList[index.row()].value.resize(g.components.size());

                defectsList[index.row()].value = g;

                break;
            }
            case (4):
                defectsList[index.row()].boundaryCondition
                    = static_cast<BoundaryCondition>(value.toInt());
                break;
        }

        emit dataChanged(index, index);
        return true;
    }
    return false;
}

bool DefectsModel::insertRows(int position, int rows, const QModelIndex& /*parent*/)
{
    beginInsertRows(QModelIndex(), position, position + rows - 1);

    for (int row = 0; row < rows; ++row) {
        defectsList.insert(position, Defect<GeneralComponentSystem>());
    }

    endInsertRows();
    return true;
}

bool DefectsModel::removeRows(int position, int rows, const QModelIndex& /*parent*/)
{
    beginRemoveRows(QModelIndex(), position, position + rows - 1);

    for (int row = 0; row < rows; ++row) {
        defectsList.removeAt(position);
    }

    endRemoveRows();
    return true;
}

bool DefectsModel::moveUpRow(int position, int rowsUp, const QModelIndex& /*index  = QModelIndex() */)
{
    if (position - rowsUp < 0)
        return false;
    Defect<GeneralComponentSystem> defect = defectsList.at(position);

    beginRemoveRows(QModelIndex(), position, position - rowsUp + 1);
    beginInsertRows(QModelIndex(), position, position - rowsUp + 1);

    for (int row = 0; row < rowsUp; ++row) {
        defectsList[position] = defectsList[position - 1];
    }
    defectsList[position - rowsUp] = defect;

    endInsertRows();
    endRemoveRows();
    return true;
}

bool DefectsModel::moveDownRow(int position, int rowsDown, const QModelIndex& /* index = QModelIndex() */)
{
    if (position + rowsDown >= defectsList.size())
        return false;
    Defect<GeneralComponentSystem> defect = defectsList.at(position);

    beginRemoveRows(QModelIndex(), position, position + rowsDown - 1);
    beginInsertRows(QModelIndex(), position, position + rowsDown - 1);
    for (int row = 0; row < rowsDown; ++row) {
        defectsList[position] = defectsList[position + 1];
    }
    defectsList[position + rowsDown] = defect;

    endInsertRows();
    endRemoveRows();
    return true;
}

TextSelectionDelegate::TextSelectionDelegate(
    const QMap<int, QString>& boundaryConditionsIdentifier, QObject* parent) : QItemDelegate(parent), boundaryConditionsIdentifier(boundaryConditionsIdentifier)
{
}

QWidget* TextSelectionDelegate::createEditor(QWidget* parent,
    const QStyleOptionViewItem& /* option */, const QModelIndex& /* index */) const
{
    QComboBox* editor = new QComboBox(parent);

    QStringList strings;
    QMapIterator<int, QString> i(boundaryConditionsIdentifier);
    while (i.hasNext()) {
        i.next();
        editor->insertItem(i.key(), i.value());
    }

    return editor;
}

void TextSelectionDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    int value = index.model()->data(index, Qt::DisplayRole).toInt();

    QComboBox* comboBox = static_cast<QComboBox*>(editor);
    comboBox->setCurrentIndex(value);
}

void TextSelectionDelegate::setModelData(QWidget* editor, QAbstractItemModel* model,
    const QModelIndex& index) const
{
    QComboBox* comboBox = static_cast<QComboBox*>(editor);

    int value = comboBox->currentIndex();

    model->setData(index, value, Qt::EditRole);
}

void TextSelectionDelegate::updateEditorGeometry(QWidget* editor,
    const QStyleOptionViewItem& option, const QModelIndex& /* index */) const
{
    editor->setGeometry(option.rect);
}
