#include "QOdooInvoice.h"

static const ODOO_ENUM_BEGIN(moveTypes, QOdooInvoice::MoveType, QString, QOdooInvoice::NoMoveType)
  {QOdooInvoice::IncomingInvoice, "in_invoice"},
  {QOdooInvoice::OutgoingInvoice, "out_invoice"}
ODOO_ENUM_END()

static const ODOO_ENUM_BEGIN(states, QOdooInvoice::State, QString, QOdooInvoice::DraftState)
  {QOdooInvoice::PostedState,   "posted"},
  {QOdooInvoice::DraftState,    "draft"},
  {QOdooInvoice::CanceledState, "cancel"}
ODOO_ENUM_END()

static const ODOO_ENUM_BEGIN(paymentStates, QOdooInvoice::PaymentState, QString, QOdooInvoice::NotPaid)
  {QOdooInvoice::Paid,          "paid"},
  {QOdooInvoice::PartiallyPaid, "partial"},
  {QOdooInvoice::NotPaid,       "not_paid"}
ODOO_ENUM_END()

static QList<unsigned long> readInvoiceLineIds(QVariant value)
{
  QList<unsigned long> result;

  for (QVariant item : value.toList())
    result.push_back(item.toULongLong());
  return result;
}

QOdooInvoice::QOdooInvoice(QObject* parent) :
  QOdooModel(parent),
  _name("name"), _ref("ref"),
  _paymentReference("payment_reference"), _paymentState("payment_state", NotPaid),
  _moveType("move_type", NoMoveType), _state("state", NoState),
  _date("date"), _invoiceDate("invoice_date"), _invoiceDateDue("invoice_date_due"),
  _partnerId("partner_id", 0), _journalId("journal_id", 0), _partnerBankId("partner_bank_id", 0)
{
  _properties << &_name << &_ref
    << &_paymentReference << &_paymentState
    << &_moveType << &_state << &_date << &_invoiceDate << &_invoiceDateDue
    << &_partnerId << &_partnerBankId << &_journalId;
}

void QOdooInvoice::fromVariantMap(QVariantMap data)
{
  _name.first             = data[_name.key].toString();
  _ref.first              = data[_ref.key].toString();
  _paymentReference.first = data[_paymentReference.key].toString();
  _paymentState.first     = paymentStates.fromValue(data[_paymentState.key].toString());
  _moveType.first         = moveTypes.fromValue(data[_moveType.key].toString());
  _state.first            = states.fromValue(data[_state.key].toString());
  _date.first             = QDate::fromString(data[_date.key].toString(), "yyyy-MM-dd");
  _invoiceDate.first      = QDate::fromString(data[_invoiceDate.key].toString(), "yyyy-MM-dd");
  _invoiceDateDue.first   = QDate::fromString(data[_invoiceDateDue.key].toString(), "yyyy-MM-dd");
  _partnerId.first        = data[_partnerId.key].toInt();
  _partnerBankId.first    = data[_partnerBankId.key].toInt();
  _journalId.first        = data[_journalId.key].toInt();
}

QOdooInvoiceLine* QOdooInvoice::lineAt(unsigned short index)
{
  while (index >= _lines.size())
    _lines.push_back(new QOdooInvoiceLine(this));
  return _lines[index];
}
/*
static void fetchNextLine(QOdooInvoice* invoice, QVector<QOdooInvoiceLine*>& lines, OdooService& odoo, QVector<int> ids, std::function<void()> callback)
{
  auto lineId = ids.takeFirst();

  odoo.findObject(QOdooInvoiceLine().odooTypename(), lineId, QOdooInvoiceLine().propertyNames(), [invoice, &lines, &odoo, ids, callback](QVariant result)
  {
    if (!(QXMLRpcFault::isFault(result)))
    {
      auto* line = new QOdooInvoiceLine(invoice);

      lines << line;
      line->fromVariantMap(result.toMap());
      if (ids.size() == 0)
        callback();
      else
        fetchNextLine(invoice, lines, odoo, ids, callback);
    }
    else
      callback();
  });
}

void QOdooInvoice::fetch(OdooService& odoo, IdType id, std::function<void()> callback)
{
  if (id == 0)
  {
    callback();
    return ;
  }
  odoo.findObject(odooTypename(), id, propertyNames(), [this, &odoo, id, callback](QVariant result)
  {
    if (!QXMLRpcFault::isFault(result))
    {
      QVariantMap data = result.toMap();
      _id = id;
      qDebug() << "Result being: " << data;
      fromVariantMap(data);
      fetchNextLine(this, _lines, odoo, readInvoiceLineIds(data["invoice_line_ids"]), [this, callback]()
      {
        qDebug() << "FETCHED LINES" << _lines;
        callback();
      });
    }
    else
      callback();
  });
}
*/
void QOdooInvoice::fetchRelationships(OdooService& odoo, QVariantMap data, std::function<void ()> callback)
{
  QList<unsigned long> lineIds = readInvoiceLineIds(data["invoice_line_ids"]);

  odoo.fetch<QOdooInvoiceLine>(lineIds, [this, callback](QVector<QOdooInvoiceLine*> fetchedLines)
  {
    for (auto* line : fetchedLines)
    {
      line->setParent(this);
      _lines << line;
    }
    callback();
  });
}

QVariantMap QOdooInvoice::xmlrpcTransaction() const
{
  XmlRpcTransaction transaction;

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
  transaction.addRelationship("invoice_line_ids", _lines);
  return transaction;
}
