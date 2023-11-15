#include "QOdooTableModel.h"
#include <QDebug>

QOdooTableModel::QOdooTableModel(QObject* parent) : QAbstractTableModel(parent)
{
}

QOdooTableColumn::QOdooTableColumn(QObject* parent) : QObject(parent)
{
}

QOdooTableColumn::QOdooTableColumn(const QString& a, const QString& b, QObject* parent) : QObject(parent), _property(a), _label(b)
{
}

QOdooTableColumn::QOdooTableColumn(const QString& a, const QString& b, Handler c, QObject* parent) : QObject(parent), _property(a), _label(b), _handler(c)
{
}

QVariant QOdooTableColumn::valueFor(QObject* model2) const
{
  QOdooModel* model = qobject_cast<QOdooModel*>(model2);
  qDebug() << "QOdooTableColumn: Getting value for" << _property << model;
  if (model)
  {
    if (_handler)
      return _handler(model);
    return model->property(_property.toStdString().c_str());
  }
  return QVariant();
}

int QOdooTableModel::rowCount(const QModelIndex&) const
{
  qDebug() << "QOdooTableModel: rowCount = " << _models.size() << this;
  return _models.size();
}

int QOdooTableModel::columnCount(const QModelIndex&) const
{
  qDebug() << "QOdooTableModel: columnCount = " << _columns.size() << this;
  return _columns.size();
}

QOdooTableColumn* QOdooTableModel::columnAt(std::size_t index) const
{
  return _columns.size() > index ? _columns.at(index) : nullptr;
}

QOdooModel* QOdooTableModel::modelAt(std::size_t index) const
{
  return _models.size() > index ? _models.at(index) : nullptr;
}

QVariant QOdooTableModel::data(const QModelIndex& index, int role) const
{
  QOdooTableColumn* column = columnAt(index.column());
  QVariant          value;

  if (column)
  {
    if (role == Qt::DisplayRole)
      value = column->valueFor(modelAt(index.row()));
    else
      value = column->label();
  }
  return value;
}

QHash<int, QByteArray> QOdooTableModel::roleNames() const
{
  return {
    {Qt::DisplayRole, "display"},
    {Qt::DisplayRole + 1, "header"}
  };
}

QVariant QOdooTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  return QVariant();
}

QVariant QOdooTableModel::columnName(int index) const
{
  const auto* column = columnAt(index);

  if (column)
    return column->label();
  return QVariant();
}

bool QOdooTableModel::setData(const QModelIndex& index, const QVariant& data, int role)
{
  QOdooTableColumn* column = _columns.size() > index.column() ? _columns.at(index.column()) : nullptr;
  QVariant          value;

  if (column)
  {
    if (role == Qt::DisplayRole)
    {
      QOdooModel* model = _models.size() > index.row() ? _models.at(index.row()) : nullptr;

      if (model && column)
      {
        model->setProperty(column->property().toStdString().c_str(), data);
        return true;
      }
    }
  }
  return false;
}

#ifdef QT_QUICK_LIB
void QOdooTableModel::setQModels(QQmlListReference value)
{
  _models.clear();
  for (int i = 0 ; i < value.count() ; ++i)
    _models << qobject_cast<QOdooModel*>(value.at(i));
  _modelsProperty = value;
  qDebug() << "QOdooTableModel::setQModels" << _models.size() << this;
  emit modelsChanged();
}
  
void QOdooTableModel::setQColumns(QQmlListReference value)
{
  _columns.clear();
  for (int i = 0 ; i < value.count() ; ++i)
    _columns << qobject_cast<QOdooTableColumn*>(value.at(i));
  _columnsProperty = value;
  emit columnsChanged();
}
#endif
