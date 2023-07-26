#include "QOdooCollection.h"

QOdooCollectionInterface::QOdooCollectionInterface(QObject* parent) : QObject(parent)
{
  connect(this, &QOdooCollectionInterface::queryChanged, this, &QOdooCollectionInterface::onQueryChanged);
}
