#include "QOdooCountry.h"

QOdooCountry::QOdooCountry(QObject* parent) : QOdooModel(parent),
  _name("name"), _code("code"), _currencyId("currency_id"),
  _phoneCode("phone_code"), _vatLabel("vat_label"),
  _stateRequired("state_required"), _zipRequired("zip_required")
{
  _properties << &_name << &_code << &_currencyId
              << &_phoneCode << &_vatLabel
              << &_stateRequired << &_zipRequired;
}

void QOdooCountry::fromVariantMap(QVariantMap data)
{
  if (!data["id"].isNull())
    setId(data["id"].toInt());
  _name.first = data[_name.key].toString();
  _code.first = data[_code.key].toString();
  _currencyId.loadFromVariant(data[_currencyId.key]);
  _phoneCode = data[_phoneCode.key].toInt();
  _vatLabel = data[_vatLabel.key].toString();
  if (!data[_stateRequired.key].isNull())
    _stateRequired.first = data[_stateRequired.key].toBool();
  if (!data[_zipRequired.key].isNull())
    _zipRequired.first = data[_zipRequired.key].toBool();
}

QVariantMap QOdooCountry::xmlrpcTransaction() const
{
  XmlRpcTransaction transaction;

  transaction.addProperty(_name);
  transaction.addProperty(_code);
  transaction.addProperty(_currencyId);
  transaction.addProperty(_phoneCode);
  transaction.addProperty(_vatLabel);
  transaction.addProperty(_stateRequired);
  transaction.addProperty(_zipRequired);
  return transaction;
}
