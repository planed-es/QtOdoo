#include "QOdooAccount.h"
#define prop(name, odooName)            construct_qodoo_prop(QOdooAccount, name, odooName)
#define prop_def(name, odooName, value) construct_qodoo_prop_def(QOdooAccount, name, odooName, value)

QOdooAccount::QOdooAccount(QObject* parent) : QOdooModel(parent),
  prop(code,           code),
  prop(name,           name),
  prop(groupId,        group_id),
  prop_def(reconcile,  reconcile,  false),
  prop_def(deprecated, deprecated, false),
  prop_def(nonTrade,   non_trade,  false)
{
  _properties << &_code << &_name << &_reconcile << &_deprecated << &_nonTrade;
}

void QOdooAccount::fromVariantMap(QVariantMap data)
{
  if (!data["id"].isNull())
    setId(data["id"].toInt());
  _code.loadFromVariant(data[_code.key]);
  _name.loadFromVariant(data[_name.key]);
  _groupId.loadFromVariant(data[_groupId.key]);
  if (!data[_reconcile.key].isNull())
    _reconcile.first = data[_reconcile.key].toBool();
  if (!data[_deprecated.key].isNull())
    _deprecated.first = data[_deprecated.key].toBool();
  if (!data[_nonTrade.key].isNull())
    _nonTrade.first = data[_nonTrade.key].toBool();
}

QVariantMap QOdooAccount::xmlrpcTransaction() const
{
  XmlRpcTransaction transaction;

  transaction.addProperty(_code);
  transaction.addProperty(_name);
  //transaction.addProperty(_groupId); // read-only ?
  transaction.addProperty(_reconcile);
  transaction.addProperty(_deprecated);
  transaction.addProperty(_nonTrade);
  return transaction;
}
