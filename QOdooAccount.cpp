#include "QOdooAccount.h"

QOdooAccount::QOdooAccount(QObject* parent) : QOdooModel(parent),
  _code("code"), _name("name"),
  _reconcile("reconcile", false), _deprecated("deprecated", false)
{
  _properties << &_code << &_name << &_reconcile << &_deprecated;
}

QVariantMap QOdooAccount::xmlrpcTransaction() const
{
  XmlRpcTransaction transaction;

  transaction.addProperty(_code);
  transaction.addProperty(_name);
  transaction.addProperty(_reconcile);
  transaction.addProperty(_deprecated);
  return transaction;
}
