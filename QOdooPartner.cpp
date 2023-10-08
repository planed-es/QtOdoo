#include "QOdooPartner.h"

static const ODOO_ENUM_BEGIN(companyTypes, QOdooPartner::CompanyType, QString, QOdooPartner::NoCompanyType)
  {QOdooPartner::Company, "company"},
  {QOdooPartner::Individual, ""} // TODO find the string value for this
ODOO_ENUM_END()

QOdooPartner::QOdooPartner(QObject* parent) :
  QOdooModel(parent),
  _name("name"),
  _vat("vat"), _phone("phone"),
  _city("city"), _street("street"), _zip("zip"),
  _companyType("company_type", NoCompanyType),
  _comment("comment"),
  _receivableAccountId("property_account_receivable_id"),
  _payableAccountId("property_account_payable_id")
{
  _properties << &_name << &_vat << &_phone << &_city << &_street << &_zip << &_companyType << &_comment
              << &_receivableAccountId << &_payableAccountId;
}

void QOdooPartner::fromVariantMap(QVariantMap data)
{
  if (!data["id"].isNull())
    setId(data["id"].toInt());
  _name.first                = data[_name.key].toString();
  _vat.first                 = data[_vat.key].toString();
  _phone.first               = data[_phone.key].toString();
  _city.first                = data[_city.key].toString();
  _street.first              = data[_street.key].toString();
  _zip.first                 = data[_zip.key].toString();
  _companyType.first         = companyTypes.fromValue(data[_companyType.key].toString());
  _comment.first             = data[_comment.key].toString();
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
  transaction.addProperty<CompanyType, QString>(_companyType, companyTypes.propertyUpdater());
  transaction.addProperty(_comment);
  return transaction;
}
