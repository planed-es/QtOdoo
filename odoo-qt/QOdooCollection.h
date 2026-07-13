#ifndef  QODOO_COLLECTION_H
# define QODOO_COLLECTION_H

# include <QPointer>
# include "QOdooSearchQuery.h"
# include "QOdooModel.h"
# include "QOdooService.h"
# include "QOdooException.h"
# ifdef QT_QUICK_LIB
#  include <QtQml/QQmlListProperty>
Q_DECLARE_METATYPE(QQmlListProperty<QOdooModel>)
# endif

class QTODOO_LIBRARY_EXPORT QOdooCollectionInterface : public QObject
{
  Q_OBJECT
  Q_PROPERTY(int limit READ limit WRITE setLimit NOTIFY queryChanged)
  Q_PROPERTY(int page  READ page  WRITE setPage NOTIFY queryChanged)
  Q_PROPERTY(int count READ count NOTIFY countChanged)
  Q_PROPERTY(int pageCount READ pageCount NOTIFY pageCountChanged)
# ifdef QT_QUICK_LIB
  Q_PROPERTY(QQmlListProperty<QObject> models READ models NOTIFY modelsChanged)
  virtual QQmlListProperty<QObject> models() { return QQmlListProperty<QObject>(this, reinterpret_cast<QList<QObject*>*>(&_models)); }
# endif
public:
  QOdooCollectionInterface(QObject* parent = nullptr);

  void setQuery(const QOdooSearchQuery& value) { _query = value; emit queryChanged(); }
  int limit() const { return _query.getLimit(); }
  int page() const { return limit() > 0 ? _query.getOffset() / limit() : 0; }
  int count() const { return _count; }
  int pageCount() const { return limit() > 0 ? 1 + _count / limit() : 1; }
  void setLimit(unsigned long value) { _query.limit(value); emit queryChanged(); }
  void setPage(unsigned long page) { _query.offset(limit() * page); emit queryChanged(); }
  std::size_t size() const { return _models.size(); }

  Q_INVOKABLE int indexOf(QOdooModel::IdType) const;
  Q_INVOKABLE int indexOf(const QOdooModel*) const;

  virtual QOdooModel* modelAt(std::size_t index) const { return nullptr; }

signals:
  void modelsChanged();
  void queryChanged();
  void countChanged();
  void pageCountChanged();
  void requestFailed(QOdooException error);

protected slots:
  virtual void onQueryChanged() {};

protected:
  QList<QOdooModel*> _models;
  QOdooSearchQuery   _query;
  int                _count = 0;
};

template<typename MODEL>
class QOdooIterableCollection : public QOdooCollectionInterface
{
public:
  typedef MODEL Model;

  class iterator
  {
  public:
    iterator(const QOdooIterableCollection& self, std::size_t index) : self(self), index(index) {}

    MODEL& operator*() const { return *(self.at(index)); }
    MODEL* operator->() const { return self.at(index); }

    iterator& operator++() { index = std::min<std::size_t>(index + 1, self.size()); return *this; }
    bool operator==(const iterator& other) const { return &self == &(other.self) && index == other.index; }
    bool operator!=(const iterator& other) const { return !(*this == other); }
  private:
    std::size_t index;
    const QOdooIterableCollection& self;
  };

  typedef iterator const_iterator;

  QOdooIterableCollection(QObject* parent) : QOdooCollectionInterface(parent)
  {
  }

  ~QOdooIterableCollection()
  {
    reset();
  }

  void reset()
  {
    for (QOdooModel* model : _models)
    {
      if (model->parent() == this)
        model->deleteLater();
    }
    _models.clear();
  }

  void reset(const QVector<MODEL*>& list)
  {
    reset();
    for (MODEL* model : list)
      _models.push_back(model);
    emit modelsChanged();
  }

  iterator begin() const
  {
    return iterator(*this, 0);
  }

  iterator end() const
  {
    return iterator(*this, QOdooCollectionInterface::size());
  }

  MODEL* at(std::size_t index) const
  {
    QOdooModel* result = QOdooCollectionInterface::_models.at(index);

    return reinterpret_cast<MODEL*>(result);
  }

  QOdooModel* modelAt(std::size_t index) const override
  {
    return at(index);
  }
};

template<typename MODEL, bool paginated = MODEL::supportsPagination>
class QOdooCollection : public QOdooIterableCollection<MODEL>
{
public:
  QOdooCollection(QOdooService& service, QObject* parent = nullptr)
    : QOdooIterableCollection<MODEL>(parent ? parent : &service), service(service)
  {
  }

protected:
  void onQueryChanged() override
  {
    fetchCount();
    fetchModels();
  }

  void fetchCount()
  {
    service.count<MODEL>(QOdooCollectionInterface::_query).then(this, [this](unsigned long value)
    {
      QOdooCollectionInterface::_count = value;
      emit QOdooCollectionInterface::countChanged();
    }).onFailed(this, [this](const QOdooException& error)
    {
      emit QOdooCollectionInterface::requestFailed(error);
    });
  }

  void fetchModels()
  {
    // Passing `this` as the parent means every fetched MODEL is already
    // owned by the collection - no need for a follow-up re-parenting loop.
    service.fetch<MODEL>(QOdooCollectionInterface::_query, this).then(this, [this](QVector<MODEL*> results)
    {
      int limit = QOdooIterableCollection<MODEL>::limit();

      if (limit != 0 && results.size() > limit)
        results.resize(limit);
      setModels(results);
    }).onFailed(this, [this](const QOdooException& error)
    {
      emit QOdooCollectionInterface::requestFailed(error);
    });
  }

  virtual void setModels(const QVector<MODEL*>& list)
  {
    QOdooIterableCollection<MODEL>::reset(list);
  }

  QOdooService& service;
};

template<typename MODEL> // no pagination support
class QOdooCollection<MODEL, false> : public QOdooIterableCollection<MODEL>
{
public:
  QOdooCollection(QOdooService& service, QObject* parent = nullptr)
    : QOdooIterableCollection<MODEL>(parent ? parent : &service), service(service)
  {
  }

protected:
  void onQueryChanged() override
  {
    QOdooSearchQuery query = QOdooCollectionInterface::_query;

    query.limit(0);
    query.offset(0);
    service.fetch<MODEL>(query, this).then(this, [this](QVector<MODEL*> results)
    {
      int from = QOdooCollectionInterface::page() * QOdooCollectionInterface::limit();
      int to = from + QOdooCollectionInterface::limit();
      QVector<MODEL*> selection;

      QOdooCollectionInterface::_count = results.size();
      for (int i = 0 ; i < results.size() ; ++i)
      {
        if (i >= from && i < to)
          selection.push_back(results.at(i));
        else
          results.at(i)->deleteLater();
      }
      setModels(selection);
      emit QOdooCollectionInterface::countChanged();
    }).onFailed(this, [this](const QOdooException& error)
    {
      emit QOdooCollectionInterface::requestFailed(error);
    });
  }

  virtual void setModels(const QVector<MODEL*>& list)
  {
    QOdooIterableCollection<MODEL>::reset(list);
  }

  QOdooService& service;
};

#endif
