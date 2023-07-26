#ifndef  QODOO_COLLECTION_H
# define QODOO_COLLECTION_H

# include "QOdooModel.h"

class QOdooCollectionInterface : public QObject
{
  Q_OBJECT
  Q_PROPERTY(unsigned long limit READ limit WRITE setLimit NOTIFY queryChanged)
  Q_PROPERTY(unsigned long page  READ page  WRITE page NOTIFY queryChanged)
public:
  QOdooCollectionInterface(QObject* parent = nullptr);

  void setQuery(const QOdooSearchQuery& value) { _query = value; emit queryChanged(); }
  unsigned long limit() const { return _query.getLimit(); }
  unsigned long page() const { return _query.getOffset() / limit(); }
  void setLimit(unsigned long value) { _query.limit(value); emit queryChanged(); }
  void setPage(unsigned long page) { _query.offset(limit() * page); emit queryChanged(); }
  std::size_t size() const { return _models.size(); }

signals:
  void modelsChanged();
  void queryChanged();

protected slots:
  virtual void onQueryChanged() = 0;

protected:
  QList<QOdooModel*> _models;
  QOdooSearchQuery   _query;
};

template<typename MODEL>
class QOdooCollection : public QOdooCollectionInterface
{
public:
  typedef MODEL Model;

  class iterator
  {
  public:
    iterator(const QOdooCollection& self, std::size_t index) : self(self), index(index) {}

    MODEL& operator*() const { return *(self.at(index)); }
    MODEL* operator->() const { return self.at(index); }

    iterator& operator++() { index = std::min<std::size_t>(index + 1, self.size()); return *this; }
    bool operator==(const iterator& other) const { return &self == &(other.self) && index == other.index; }
    bool operator!=(const iterator& other) const { return !(*this == other); }
  private:
    std::size_t index;
    const QOdooCollection& self;
  };

  typedef iterator const_iterator;

  QOdooCollection(OdooService& service, QObject* parent = nullptr) : QOdooCollectionInterface(parent), service(service)
  {
  }

  ~QOdooCollection()
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
    return iterator(*this, QOdooCollectionInterface::size()); }
  }

  MODEL* at(std::size_t index) const
  {
    QOdooModel* result = QOdooCollectionInterface::_models.at(index);

    return reinterpret_cast<MODEL*>(result);
  }

protected:
  void onQueryChanged() override
  {
    service.fetch(QOdooCollectionInterface::_query, [this](QVector<MODEL*> results)
    {
      for (MODEL* model : results)
        model->setParent(this);
      reset(results);
    });
  }

  OdooService& service;
};

#endif