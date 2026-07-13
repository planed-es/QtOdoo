#include "QOdooInvoice.h"
#include "QOdooService.h"
#define prop(name, odooName)            construct_qodoo_prop(QOdooInvoice, name, odooName)
#define prop_def(name, odooName, value) construct_qodoo_prop_def(QOdooInvoice, name, odooName, value)

static const ODOO_ENUM_BEGIN(moveTypes, QOdooInvoice::MoveType, QString, QOdooInvoice::NoMoveType)
  {QOdooInvoice::IncomingInvoice, "in_invoice"},
  {QOdooInvoice::OutgoingInvoice, "out_invoice"}
ODOO_ENUM_END()

static const ODOO_ENUM_BEGIN(paymentStates, QOdooInvoice::PaymentState, QString, QOdooInvoice::NotPaid)
  {QOdooInvoice::Paid,          "paid"},
  {QOdooInvoice::PartiallyPaid, "partial"},
  {QOdooInvoice::NotPaid,       "not_paid"}
ODOO_ENUM_END()

QString QOdooInvoice::valueFor(QOdooInvoice::MoveType value) { return moveTypes.fromEnum(value); }
QString QOdooInvoice::valueFor(QOdooInvoice::State value) { return states.fromEnum(value); }
QString QOdooInvoice::valueFor(QOdooInvoice::PaymentState value) { return paymentStates.fromEnum(value); }

QOdooInvoice::QOdooInvoice(QObject* parent) :
  QOdooModel(parent),
  prop(name,              name),
  prop(ref,               ref),
  prop(paymentReference,  payment_reference),
  prop(narration,         narration),
  prop_def(paymentState,  payment_state, NotPaid),
  prop_def(moveType,      move_type,     NoMoveType),
  prop_def(state,         state,         NoState),
  prop(date,              date),
  prop(invoiceDate,       invoice_date),
  prop(invoiceDateDue,    invoice_date_due),
  prop_def(partnerId,     partner_id, 0),
  prop_def(journalId,     journal_id, 0),
  prop_def(partnerBankId, partner_bank_id, 0)
{
  _properties << &_name << &_ref
    << &_paymentReference << &_paymentState
    << &_moveType << &_state << &_date << &_invoiceDate << &_invoiceDateDue
    << &_partnerId << &_partnerBankId << &_journalId
    << &_narration;
}

void QOdooInvoice::fromVariantMap(QVariantMap data)
{
  _name.loadFromVariant(data[_name.key]);
  _ref.loadFromVariant(data[_ref.key]);
  _paymentReference.loadFromVariant(data[_paymentReference.key]);
  _paymentState.first     = paymentStates.fromValue(data[_paymentState.key].toString());
  _moveType.first         = moveTypes.fromValue(data[_moveType.key].toString());
  _state.first            = states.fromValue(data[_state.key].toString());
  _date.first             = QDate::fromString(data[_date.key].toString(), "yyyy-MM-dd");
  _invoiceDate.first      = QDate::fromString(data[_invoiceDate.key].toString(), "yyyy-MM-dd");
  _invoiceDateDue.first   = QDate::fromString(data[_invoiceDateDue.key].toString(), "yyyy-MM-dd");
  _partnerId.loadFromVariant(data[_partnerId.key]);
  _partnerBankId.first    = data[_partnerBankId.key].toInt();
  _journalId.first        = data[_journalId.key].toInt();
}

void QOdooInvoice::onSaved()
{
  QOdooModel::onSaved();
  for (QOdooInvoiceLine* line : _lines)
    line->onSaved();
}

QOdooInvoiceLine* QOdooInvoice::lineAt(unsigned short index)
{
  while (index >= _lines.size())
    _lines.push_back(new QOdooInvoiceLine(this));
  return _lines[index];
}

void QOdooInvoice::deleteLineAt(unsigned short index)
{
  lineAt(index)->markForDeletion();
}

QFuture<void> QOdooInvoice::fetchRelationships(QOdooService& odoo, const QJsonObject& data)
{
  QVector<IdType> ids;

  for (const QJsonValue& value : data.value("invoice_line_ids").toArray())
    ids << value.toULongLong();
  return odoo.fetch<QOdooInvoiceLine>(ids, this).then([this](QVector<QOdooInvoiceLine*> lines)
  {
    for (QOdooInvoiceLine* line : _lines)
    {
      if (line->parent() == this)
        line->deleteLater();
    }
    _lines = lines;
  });
}

QVariantMap QOdooInvoice::jsonTransaction() const
{
  JsonTransaction transaction;

  transaction.addProperty(_name);
  transaction.addProperty(_ref);
  transaction.addProperty(_paymentReference);
  transaction.addProperty<PaymentState, QString>(_paymentState, paymentStates.propertyUpdater());
  transaction.addProperty<MoveType, QString>(_moveType, moveTypes.propertyUpdater());
  transaction.addProperty<State, QString>(_state, states.propertyUpdater());
  transaction.addProperty(_date);
  transaction.addProperty(_invoiceDate);
  transaction.addProperty(_invoiceDateDue);
  transaction.addProperty(_partnerId);
  transaction.addProperty(_partnerBankId);
  transaction.addProperty(_journalId);
  transaction.addProperty(_narration);
  transaction.addRelationship("invoice_line_ids", _lines);
  return transaction;
}
