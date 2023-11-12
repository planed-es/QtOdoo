#ifndef  QODOO_COLLECTION_H
# define QODOO_COLLECTION_H

# include "QOdooSearchQuery.h"
# include "QOdooModel.h"
# include "QOdoo.h"
# ifdef QT_QUICK_LIB
#  include <QtQml/QQmlListProperty>
# endif

Q_DECLARE_METATYPE(QQmlListProperty<QOdooModel>)

class QOdooCollectionInterface : public QObject
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
  int page() const { return _query.getOffset() / limit(); }
  int count() const { return _count; }
  int pageCount() const { return 1 + _count / limit(); }
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
  QOdooCollection(OdooService& service, QObject* parent = nullptr)
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
    service.count<MODEL>(QOdooCollectionInterface::_query, [this](unsigned long value)
    {
      QOdooCollectionInterface::_count = value;
      emit QOdooCollectionInterface::countChanged();
    });
  }

  void fetchModels()
  {
    service.fetch<MODEL>(QOdooCollectionInterface::_query, [this](QVector<MODEL*> results)
    {
      for (MODEL* model : results)
        model->setParent(this);
      setModels(results);
    });
  }

  virtual void setModels(const QVector<MODEL*>& list)
  {
    QOdooIterableCollection<MODEL>::reset(list);
  }

  OdooService& service;
};

template<typename MODEL> // no pagination suppport
class QOdooCollection<MODEL, false> : public QOdooIterableCollection<MODEL>
{
public:
  QOdooCollection(OdooService& service, QObject* parent = nullptr)
    : QOdooIterableCollection<MODEL>(parent ? parent : &service), service(service)
  {
  }

protected:
  void onQueryChanged() override
  {
    QOdooSearchQuery query = QOdooCollectionInterface::_query;

    query.limit(0);
    query.offset(0);
    service.fetch<MODEL>(query, [this](QVector<MODEL*> results)
    {
      QOdooCollectionInterface::_count = results.size();
      int from = QOdooCollectionInterface::page() * QOdooCollectionInterface::limit();
      int to = from + QOdooCollectionInterface::limit();
      QVector<MODEL*> selection;

      for (int i = from ; i < to && i < results.size() ; ++i)
      {
        MODEL* model = results.at(i);

        model->setParent(this);
        selection.push_back(model);
      }
      setModels(selection);
      emit QOdooCollectionInterface::countChanged();
    });
  }

  virtual void setModels(const QVector<MODEL*>& list)
  {
    QOdooIterableCollection<MODEL>::reset(list);
  }

  OdooService& service;
};

#endif
