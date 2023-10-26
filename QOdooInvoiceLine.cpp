#include "QOdooInvoiceLine.h"
#define prop(name, odooName)            construct_qodoo_prop(QOdooInvoiceLine, name, odooName)
#define prop_def(name, odooName, value) construct_qodoo_prop_def(QOdooInvoiceLine, name, odooName, value)

QOdooInvoiceLine::QOdooInvoiceLine(QObject* parent) :
  QOdooModel(parent),
  prop(name,          name),
  prop_def(accountId, account_id, 0),
  prop_def(productId, product_id, 0),
  prop_def(quantity,  quantity,   0),
  prop_def(priceUnit, price_unit, 0),
  prop_def(credit,    credit,     0),
  prop_def(debit,     debit,      0),
  prop(taxIds,        tax_ids)
{
  _properties << &_name << &_accountId << &_productId << &_quantity << &_priceUnit << &_taxIds;
}

void QOdooInvoiceLine::fromVariantMap(QVariantMap data)
{
  _name.first      = data[_name.key].toString();
  _accountId.loadFromVariant(data[_accountId.key]);
  _productId.loadFromVariant(data[_productId.key]);
  _quantity.first  = data[_quantity.key].toDouble(0);
  _priceUnit.first = data[_priceUnit.key].toDouble(0);
  _credit.first    = data[_credit.key].toInt(0);
  _debit.first     = data[_debit.key].toInt(0);
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
