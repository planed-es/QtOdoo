#include "QOdooCollection.h"

QOdooCollectionInterface::QOdooCollectionInterface(QObject* parent) : QObject(parent)
{
  connect(this, &QOdooCollectionInterface::queryChanged, this, &QOdooCollectionInterface::onQueryChanged);
  connect(this, &QOdooCollectionInterface::queryChanged, this, &QOdooCollectionInterface::pageCountChanged);
  connect(this, &QOdooCollectionInterface::countChanged, this, &QOdooCollectionInterface::pageCountChanged);
}
