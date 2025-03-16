#include "QOdooBankStatementLine.h"
#include "QOdoo.h"
#define prop(name, odooName)            construct_qodoo_prop(QOdooBankStatementLine, name, odooName)
#define prop_def(name, odooName, value) construct_qodoo_prop_def(QOdooBankStatementLine, name, odooName, value)

QOdooBankStatementLine::QOdooBankStatementLine(QObject* parent) :
  QOdooModel(parent),
  prop(paymentRef, payment_ref),
  prop(amount, amount),
  prop(date, date),
  prop_def(journalId,    journal_id, 0),
  prop_def(partnerId,    partner_id, 0),
  prop_def(moveId,       move_id,    0),
  prop_def(state,        state,      NoState),
  prop_def(isReconciled, is_reconciled, false)
{
  _properties << &_paymentRef << &_amount << &_date << &_journalId << &_partnerId << &_moveId << &_state << &_isReconciled;
}

void QOdooBankStatementLine::fromVariantMap(QVariantMap data)
{
  _paymentRef.loadFromVariant(data[_paymentRef.key]);
  _amount.first    = data[_amount.key].toDouble(0);
  _date.first      = QDate::fromString(data[_date.key].toString(), "yyyy-MM-dd");
  _journalId.loadFromVariant(data[_journalId.key]);
  _partnerId.loadFromVariant(data[_partnerId.key]);
  _moveId.loadFromVariant(data[_moveId.key]);
  _state.first = states.fromValue(data[_state.key].toString());
  _isReconciled.first = data[_isReconciled.key].toBool();
}

QVariantMap QOdooBankStatementLine::xmlrpcTransaction() const
{
  XmlRpcTransaction transaction;

  transaction.addProperty(_paymentRef);
  transaction.addProperty(_amount);
  transaction.addProperty(_date);
  transaction.addProperty(_journalId);
  transaction.addProperty(_partnerId);
  transaction.addProperty<State, QString>(_state, states.propertyUpdater());
  return transaction;
}
