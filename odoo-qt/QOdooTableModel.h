#ifndef QODOOTABLEMODEL_H
# define QODOOTABLEMODEL_H

# include <QAbstractTableModel>
# include "QOdooCollection.h"
# ifdef QT_QUICK_LIB
#  include <QtQml/QQmlListProperty>
# endif

class QTODOO_LIBRARY_EXPORT QOdooTableColumn : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QString property MEMBER _property NOTIFY propertyChanged)
  Q_PROPERTY(QString label    MEMBER _label    NOTIFY labelChanged)
  Q_PROPERTY(bool    isMain   MEMBER _main     NOTIFY mainChanged)
public:
  typedef std::function<QVariant(const QOdooModel*)> Handler;

  QOdooTableColumn(QObject* parent = nullptr);
  QOdooTableColumn(const QString& property, const QString& label, QObject* parent = nullptr);
  QOdooTableColumn(const QString& property, const QString& label, Handler handler, QObject* parent = nullptr);

  const QString&       property() const { return _property; }
  const QString&       label() const { return _label; }
  Q_INVOKABLE QVariant valueFor(QObject* model) const;

  void setProperty(const QString& value) { _property = value; emit propertyChanged(); }
  void setLabel(const QString& value) { _label = value; emit labelChanged(); }
  void setMain(bool value) { _main = value; emit mainChanged(); }

signals:
  void propertyChanged();
  void labelChanged();
  void mainChanged();

private:
  QString  _property, _label;
  Handler  _handler;
  bool     _main = false;
};

class QTODOO_LIBRARY_EXPORT QOdooTableModel : public QAbstractTableModel
{
  Q_OBJECT
# ifdef QT_QUICK_LIB
  Q_PROPERTY(QQmlListReference models MEMBER _modelsProperty WRITE setQModels NOTIFY modelsChanged)
  Q_PROPERTY(QQmlListReference columns MEMBER _columnsProperty WRITE setQColumns NOTIFY columnsChanged)
  QQmlListReference _modelsProperty, _columnsProperty;
  void setQModels(QQmlListReference value);
  void setQColumns(QQmlListReference value);
# endif
public:
  QOdooTableModel(QObject* parent = nullptr);

  int rowCount(const QModelIndex& = QModelIndex()) const override;
  int columnCount(const QModelIndex& = QModelIndex()) const override;
  QVariant data(const QModelIndex&, int role = Qt::DisplayRole) const override;
  QHash<int, QByteArray> roleNames() const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
  bool setData(const QModelIndex&, const QVariant&, int role) override;

  Q_INVOKABLE QVariant columnName(int index) const;

  QList<QOdooModel*>& models() { return _models; }
  QList<QOdooTableColumn*>& columns() { return _columns; }

  QOdooTableColumn* columnAt(std::size_t) const;
  QOdooModel* modelAt(std::size_t) const;

signals:
  void modelsChanged();
  void columnsChanged();

private:
  QList<QOdooModel*> _models;
  QList<QOdooTableColumn*> _columns;
};

#endif
