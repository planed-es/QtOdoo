#include "QOdooBankStatementLine.h"
#include "QOdoo.h"
#define prop(name, odooName)            construct_qodoo_prop(QOdooBankStatementLine, name, odooName)
#define prop_def(name, odooName, value) construct_qodoo_prop_def(QOdooBankStatementLine, name, odooName, value)

QOdooBankStatementLine::QOdooBankStatementLine(QObject* parent) :
  QOdooModel(parent),
  prop(paymentRef, payment_ref),
  prop(amount, amount),
  prop(date, date),
  prop_def(journalId, journal_id, 0),
  prop_def(partnerId, partner_id, 0)
{
  _properties << &_paymentRef << &_amount << &_date << &_journalId << &_partnerId;
}

void QOdooBankStatementLine::fromVariantMap(QVariantMap data)
{
  _paymentRef.loadFromVariant(data[_paymentRef.key]);
  _amount.first    = data[_amount.key].toDouble(0);
  _date.first      = QDate::fromString(data[_date.key].toString(), "yyyy-MM-dd");
  _journalId.first = data[_journalId.key].toInt();
  _partnerId.first = data[_partnerId.key].toInt();
}

QVariantMap QOdooBankStatementLine::xmlrpcTransaction() const
{
  XmlRpcTransaction transaction;

  transaction.addProperty(_paymentRef);
  transaction.addProperty(_amount);
  transaction.addProperty(_date);
  transaction.addProperty(_journalId);
  transaction.addProperty(_partnerId);
  return transaction;
}
