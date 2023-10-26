#include "QOdooCountryState.h"
#define prop(name, odooName)            construct_qodoo_prop(QOdooCountryState, name, odooName)
#define prop_def(name, odooName, value) construct_qodoo_prop_def(QOdooCountryState, name, odooName, value)

QOdooCountryState::QOdooCountryState(QObject* parent) : QOdooModel(parent),
  prop(name, name),
  prop(code, code),
  prop(countryId, country_id)
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
