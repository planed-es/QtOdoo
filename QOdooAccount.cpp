#include "QOdooAccount.h"

QOdooAccount::QOdooAccount(QObject* parent) : QOdooModel(parent),
  _code("code"), _name("name"),
  _groupId("group_id"),
  _reconcile("reconcile", false), _deprecated("deprecated", false), _nonTrade("non_trade", false)
{
  _properties << &_code << &_name << &_reconcile << &_deprecated << &_nonTrade;
}

void QOdooAccount::fromVariantMap(QVariantMap data)
{
  if (!data["id"].isNull())
    setId(data["id"].toInt());
  _code.first = data[_code.key].toString();
  _name.first = data[_name.key].toString();
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
