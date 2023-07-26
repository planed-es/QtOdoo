#include "QOdooInvoiceLine.h"

QOdooInvoiceLine::QOdooInvoiceLine(QObject* parent) :
  QOdooModel(parent),
  _name("name"),
  _accountId("account_id", 0), _productId("product_id", 0),
  _quantity("quantity", 0), _priceUnit("price_unit", 0),
  _taxIds("tax_ids")
{
  _properties << &_name << &_accountId << &_productId << &_quantity << &_priceUnit << &_taxIds;
}

void QOdooInvoiceLine::fromVariantMap(QVariantMap data)
{
  _name.first      = data[_name.key].toString();
  _accountId.first = data[_accountId.key].toInt(0);
  _productId.first = data[_productId.key].toInt(0);
  _quantity.first  = data[_quantity.key].toInt(0);
  _priceUnit.first = data[_priceUnit.key].toDouble(0);
  _taxIds.first    = data[_taxIds.key].toList();
}

QVariantMap QOdooInvoiceLine::xmlrpcTransaction() const
{
  XmlRpcTransaction transaction;

  transaction.addProperty(_name);
  transaction.addProperty(_accountId);
  transaction.addProperty(_productId);
  transaction.addProperty(_quantity);
  transaction.addProperty(_priceUnit);
  transaction.addProperty(_taxIds);
  return transaction;
}
