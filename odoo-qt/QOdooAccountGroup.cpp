#include "QOdooAccountGroup.h"
#define prop(name, odooName)            construct_qodoo_prop(QOdooAccountGroup, name, odooName)
#define prop_def(name, odooName, value) construct_qodoo_prop_def(QOdooAccountGroup, name, odooName, value)

QOdooAccountGroup::QOdooAccountGroup(QObject* parent) : QOdooModel(parent),
  prop(name, name),
  prop(codePrefixStart, code_prefix_start),
  prop(codePrefixEnd, code_prefix_end)
{
  _properties << &_name << &_codePrefixStart << &_codePrefixEnd;
}

void QOdooAccountGroup::fromVariantMap(QVariantMap data)
{
  if (!data["id"].isNull())
    setId(data["id"].toInt());
  _name.loadFromVariant(data[_name.key]);
  _codePrefixStart.loadFromVariant(data[_codePrefixStart.key]);
  _codePrefixEnd.loadFromVariant(data[_codePrefixEnd.key]);
}

QVariantMap QOdooAccountGroup::xmlrpcTransaction() const
{
  XmlRpcTransaction transaction;

  transaction.addProperty(_name);
  transaction.addProperty(_codePrefixStart);
  transaction.addProperty(_codePrefixEnd);
  return transaction;
}
