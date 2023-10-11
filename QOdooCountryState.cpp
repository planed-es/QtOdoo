#include "QOdooCountryState.h"

QOdooCountryState::QOdooCountryState(QObject* parent) : QOdooModel(parent),
  _name("name"), _code("code"), _countryId("country_id")
{
  _properties << &_name << &_code << &_countryId;
}

void QOdooCountryState::fromVariantMap(QVariantMap data)
{
  if (!data["id"].isNull())
    setId(data["id"].toInt());
  _name.first = data[_name.key].toString();
  _code.first = data[_code.key].toString();
  _countryId.loadFromVariant(data[_countryId.key]);
}

QVariantMap QOdooCountryState::xmlrpcTransaction() const
{
  XmlRpcTransaction transaction;

  transaction.addProperty(_name);
  transaction.addProperty(_code);
  transaction.addProperty(_countryId);
  return transaction;
}
