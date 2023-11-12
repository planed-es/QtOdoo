#include "QOdooCollection.h"

QOdooCollectionInterface::QOdooCollectionInterface(QObject* parent) : QObject(parent)
{
  connect(this, &QOdooCollectionInterface::queryChanged, this, &QOdooCollectionInterface::onQueryChanged);
  connect(this, &QOdooCollectionInterface::queryChanged, this, &QOdooCollectionInterface::pageCountChanged);
  connect(this, &QOdooCollectionInterface::countChanged, this, &QOdooCollectionInterface::pageCountChanged);
}

int QOdooCollectionInterface::indexOf(QOdooModel::IdType id) const
{
  for (int i = 0 ; i < _models.size() ; ++i)
  {
    const QOdooModel* model = _models.at(i);

    if (model->id() == id)
      return i;
  }
  return -1;
}

int QOdooCollectionInterface::indexOf(const QOdooModel* source) const
{
  for (int i = 0 ; i < _models.size() ; ++i)
  {
    const QOdooModel* model = _models.at(i);

    if (source == model)
      return i;
  }
  return -1;
}
