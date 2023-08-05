#include "QOdooModel.h"
#include "QOdoo.h"

void QOdooModel::setId(IdType value)
{
  _id = value;
  emit idChanged();
}

void QOdooModel::save(OdooService& odoo, std::function<void()> callback)
{
  QVariantList params;
  std::function<void()> callbacks = [this, callback]()
  {
    onSaved();
    callback();
  };

  params.push_back(xmlrpcTransaction());
  if (_id == 0)
    odoo.createObject(odooTypename(), params, [this, callbacks](int id) { _id = id; callbacks(); });
  else
    odoo.updateObject(odooTypename(), _id, params, callbacks);
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
