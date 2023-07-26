#include "QOdooPartner.h"

static const QMap<QOdooPartner::CompanyType, QString> companyTypeMap{
  {QOdooPartner::Company, "company"},
  {QOdooPartner::Individual, ""} // TODO find the string value for this
};

static QString companyTypeToString(const QOdooModel::Property<QOdooPartner::CompanyType>& property)
{
  auto it = companyTypeMap.find(property.first);

  if (it != companyTypeMap.end())
    return *it;
  return "";
}

static QOdooPartner::CompanyType readCompanyType(QVariant value)
{
  QString symbol = value.toString();
  for (auto it = companyTypeMap.begin() ; it != companyTypeMap.end() ; ++it)
  {
    if (it.value() == symbol)
      return it.key();
  }
  return QOdooPartner::NoCompanyType;
}

QOdooPartner::QOdooPartner(QObject* parent) :
  QOdooModel(parent),
  _name("name"),
  _vat("vat"), _phone("phone"),
  _city("city"), _street("street"), _zip("zip"),
  _companyType("company_type", NoCompanyType),
  _comment("comment")
{
  _properties << &_name << &_vat << &_phone << &_city << &_street << &_zip << &_companyType << &_comment;
}

void QOdooPartner::fromVariantMap(QVariantMap data)
{
  _name.first        = data[_name.key].toString();
  _vat.first         = data[_vat.key].toString();
  _phone.first       = data[_phone.key].toString();
  _city.first        = data[_city.key].toString();
  _street.first      = data[_street.key].toString();
  _zip.first         = data[_zip.key].toString();
  _companyType.first = readCompanyType(data[_companyType.key]);
  _comment.first     = data[_comment.key].toString();
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
  transaction.addProperty<CompanyType, QString>(_companyType, &companyTypeToString);
  transaction.addProperty(_comment);
  return transaction;
}
