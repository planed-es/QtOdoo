#include "QOdooTax.h"
#include <QDebug>

static const ODOO_ENUM_BEGIN(taxTypes, QOdooTax::TaxType, QString, QOdooTax::NoTaxType)
  {QOdooTax::SaleTax,     "sale"},
  {QOdooTax::PurchaseTax, "purchase"}
ODOO_ENUM_END()

static const ODOO_ENUM_BEGIN(taxScopes, QOdooTax::TaxScope, QString, QOdooTax::NoTaxScope)
  {QOdooTax::Consumible, "consu"},
  {QOdooTax::Service,    "service"}
ODOO_ENUM_END()

static const ODOO_ENUM_BEGIN(amountTypes, QOdooTax::AmountType, QString, QOdooTax::NoAmountType)
  {QOdooTax::GroupedAmount,  "group"},
  {QOdooTax::FixedAmount,    "fixed"},
  {QOdooTax::PercentAmount,  "percent"},
  {QOdooTax::DivisionAmount, "division"}
ODOO_ENUM_END()

QString QOdooTax::valueFor(TaxType value) { return taxTypes.fromEnum(value); }
QString QOdooTax::valueFor(TaxScope value) { return taxScopes.fromEnum(value); }
QString QOdooTax::valueFor(AmountType value) { return amountTypes.fromEnum(value); }

QOdooTax::QOdooTax(QObject* parent) :
  QOdooModel(parent),
  _name("name"), _description("description"),
  _typeTaxUse("type_tax_use", NoTaxType),
  _amountType("amount_type", NoAmountType),
  _taxScope("tax_scope", NoTaxScope),
  _active("active", true),
  _priceInclude("price_include", false),
  _includeBaseAmount("include_base_amount", false),
  _isBaseAffected("is_base_affected", false),
  _amount("amount", 0)
{
  _properties
    << &_name << &_description
    << &_typeTaxUse << &_amountType << &_taxScope
    << &_active << &_priceInclude << &_includeBaseAmount << &_isBaseAffected
    << &_amount;
}

void QOdooTax::fromVariantMap(QVariantMap data)
{
  qDebug() << "QOdooTax created from variant map:" << data;
  if (!data["id"].isNull())
    setId(data["id"].toULongLong());
  _name.first = data[_name.key].toString();
  _description.first = data[_description.key].toString();
  _typeTaxUse.first = taxTypes.fromValue(data[_typeTaxUse.key].toString());
  _amountType.first = amountTypes.fromValue(data[_amountType.key].toString());
  _taxScope.first = taxScopes.fromValue(data[_taxScope.key].toString());
  _active.first = data[_active.key].toBool();
  _amount.first = data[_amount.key].toFloat();
  _priceInclude.first = data[_priceInclude.key].toBool();
  _includeBaseAmount.first = data[_includeBaseAmount.key].toBool();
  _isBaseAffected.first = data[_isBaseAffected.key].toBool();
}

QVariantMap QOdooTax::xmlrpcTransaction() const
{
  XmlRpcTransaction transaction;

  transaction.addProperty(_name);
  transaction.addProperty(_description);
  transaction.addProperty<TaxType, QString>(_typeTaxUse, taxTypes.propertyUpdater());
  transaction.addProperty<AmountType, QString>(_amountType, amountTypes.propertyUpdater());
  transaction.addProperty<TaxScope, QString>(_taxScope, taxScopes.propertyUpdater());
  transaction.addProperty(_active);
  transaction.addProperty(_amount);
  transaction.addProperty(_priceInclude);
  transaction.addProperty(_includeBaseAmount);
  transaction.addProperty(_isBaseAffected);
  return transaction;
}
