#include "QOdooModel.h"

void QOdooModel::setId(IdType value)
{
  _id = value;
  emit idChanged();
}

void QOdooModel::save(OdooService& odoo, std::function<void()> callback)
{
  QVariantList params;

  params.push_back(xmlrpcTransaction());
  if (_id == 0)
    odoo.createObject(odooTypename(), params, [this, callback](int id) { _id = id; callback(); });
  else
    odoo.updateObject(odooTypename(), _id, params, callback);
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
