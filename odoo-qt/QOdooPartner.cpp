#include "QOdooPartner.h"
#define prop(name, odooName)            construct_qodoo_prop(QOdooPartner, name, odooName)
#define prop_def(name, odooName, value) construct_qodoo_prop_def(QOdooPartner, name, odooName, value)

static const ODOO_ENUM_BEGIN(companyTypes, QOdooPartner::CompanyType, QString, QOdooPartner::NoCompanyType)
  {QOdooPartner::Company, "company"},
  {QOdooPartner::Individual, ""} // TODO find the string value for this
ODOO_ENUM_END()

QOdooPartner::QOdooPartner(QObject* parent) :
  QOdooModel(parent),
  prop(name,                name),
  prop(vat,                 vat),
  prop(phone,               phone),
  prop(city,                city),
  prop(street,              street),
  prop(zip,                 zip),
  prop(countryId,           country_id),
  prop(stateId,             state_id),
  prop_def(companyType,     company_type, NoCompanyType),
  prop(comment,             comment),
  prop(receivableAccountId, property_account_receivable_id),
  prop(payableAccountId,    property_account_payable_id)
{
  _properties << &_name << &_vat << &_phone << &_city << &_street << &_zip
              << &_countryId << &_stateId
              << &_companyType << &_comment
              << &_receivableAccountId << &_payableAccountId;
}

void QOdooPartner::fromVariantMap(QVariantMap data)
{
  if (!data["id"].isNull())
    setId(data["id"].toInt());
  _name.loadFromVariant(data[_name.key]);
  _vat.loadFromVariant(data[_vat.key]);
  _phone.loadFromVariant(data[_phone.key]);
  _city.loadFromVariant(data[_city.key]);
  _street.loadFromVariant(data[_street.key]);
  _zip.loadFromVariant(data[_zip.key]);
  _countryId.loadFromVariant(data[_countryId.key]);
  _stateId.loadFromVariant(data[_stateId.key]);
  _companyType.first = companyTypes.fromValue(data[_companyType.key].toString());
  _comment.loadFromVariant(data[_comment.key]);
  _receivableAccountId.loadFromVariant(data[_receivableAccountId.key]);
  _payableAccountId.loadFromVariant(data[_payableAccountId.key]);
}

QVariantMap QOdooPartner::xmlrpcTransaction() const
{
  XmlRpcTransaction transaction;

  transaction.addProperty(_name);
  transaction.addProperty(_vat);
  transaction.addProperty(_phone);
  transaction.addProperty(_city);
  transaction.addProperty(_street);
  transaction.addProperty(_zip);
  transaction.addProperty(_countryId);
  transaction.addProperty(_stateId);
  transaction.addProperty<CompanyType, QString>(_companyType, companyTypes.propertyUpdater());
  transaction.addProperty(_comment);
  transaction.addProperty(_receivableAccountId);
  transaction.addProperty(_payableAccountId);
  return transaction;
}
