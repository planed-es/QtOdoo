#include "QOdooModel.h"
#include "QOdooService.h"
#include <QtGlobal>

QOdooModel::QOdooModel(QObject* parent) : QObject(parent)
{
  connect(this, &QOdooModel::requestPropertyRefresh, this, &QOdooModel::onRefreshProperties);
}

QOdooModel::~QOdooModel()
{
}

void QOdooModel::onRefreshProperties()
{
  for (const PropertyInterface* property : _properties)
    property->emitChange();
}

void QOdooModel::setId(IdType value)
{
  _id = value;
  emit idChanged();
}

void QOdooModel::onSaved()
{
  for (PropertyInterface* property : _properties)
    property->resetState();
}

bool QOdooModel::hasChanged() const
{
  for (const PropertyInterface* property : _properties)
  {
    if (property->hasChanged())
      return true;
  }
  return false;
}

QStringList QOdooModel::propertyNames() const
{
  QStringList result;

  for (const PropertyInterface* property : _properties)
    result << property->key;
  return result;
}

void QOdooModel::StringProperty::loadFromVariant(QVariant value)
{
  if (!value.isNull())
  {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    switch (value.typeId())
#else
    switch (static_cast<QMetaType::Type>(value.type()))
#endif
    {
    case QMetaType::Bool:
      break ;
    default:
      first = value.toString();
      break ;
    }
  }
}

void QOdooModel::IdProperty::loadFromVariant(QVariant value)
{
  if (!value.isNull())
  {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    switch (value.typeId())
#else
    switch (static_cast<QMetaType::Type>(value.type()))
#endif
    {
    case QMetaType::QVariantList:
      first = value.toList().first().toULongLong();
      break ;
    case QMetaType::Int:
    case QMetaType::UInt:
    case QMetaType::Long:
    case QMetaType::LongLong:
    case QMetaType::ULong:
    case QMetaType::ULongLong:
      first = value.toULongLong();
      break ;
    default:
      qDebug() << "Could not load IdProperty" << key << "from value" << value;
      break ;
    }
  }
}
