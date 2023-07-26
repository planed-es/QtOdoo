#ifndef  QODOO_INVOICE_H
# define QODOO_INVOICE_H

# include "QOdooModel.h"
# include "QOdooInvoiceLine.h"

class QOdooInvoice : public QOdooModel
{
  friend class OdooService;
  Q_OBJECT
  Q_PROPERTY(QString  name READ name WRITE setName)
  Q_PROPERTY(QString  ref READ ref WRITE setRef)
  Q_PROPERTY(MoveType moveType READ moveType WRITE setMoveType)
  Q_PROPERTY(State    state READ state WRITE setState)
  Q_PROPERTY(QDate    date READ date WRITE setDate)
  Q_PROPERTY(QDate    invoiceDate READ invoiceDate WRITE setInvoiceDate)
  Q_PROPERTY(QDate    invoiceDateDue READ invoiceDateDue WRITE setInvoiceDateDue)
  Q_PROPERTY(IdType   partnerId READ partnerId WRITE setPartnerId)
  Q_PROPERTY(IdType   partnerBankId READ partnerBankId WRITE setPartnerBankId)
  Q_PROPERTY(IdType   journalId READ journalId WRITE setJournalId)
public:
  const char* odooTypename() const override { return "account.move"; }

  enum MoveType
  {
    NoMoveType = 0,
    IncomingInvoice,
    OutgoingInvoice
  };
  Q_ENUM(MoveType)

  enum State
  {
    NoState = 0,
    DraftState,
    PostedState,
    CanceledState
  };
  Q_ENUM(State)

  enum PaymentState
  {
    NotPaid = 0,
    PartiallyPaid,
    Paid
  };
  Q_ENUM(PaymentState)

  QOdooInvoice(QObject* parent = nullptr);

  //void fetch(OdooService& odoo, IdType id, std::function<void()> callback);
  void fromVariantMap(QVariantMap data);

  void setName(const QString& value) { _name.set(value); }
  void setRef(const QString& value) { _ref.set(value); }
  void setPaymentReference(const QString& value) { _paymentReference.set(value); }
  void setPaymentState(PaymentState value) { _paymentState.set(value); }
  void setMoveType(MoveType value) { _moveType.set(value); }
  void setState(State value) { _state.set(value); }
  void setDate(QDate value) { _date.set(value); }
  void setInvoiceDate(QDate value) { _invoiceDate.set(value); }
  void setInvoiceDateDue(QDate value) { _invoiceDateDue.set(value); }
  void setPartnerId(IdType value) { _partnerId.set(value); }
  void setPartnerBankId(IdType value) { _partnerBankId.set(value); }
  void setJournalId(IdType value) { _journalId.set(value); }

  const QString& name() const { return *_name; }
  const QString& ref() const { return *_ref; }
  const QString& paymentReference() const { return *_paymentReference; }
  PaymentState   paymentState() const { return *_paymentState; }
  MoveType       moveType() const { return *_moveType; }
  State          state() const { return *_state; }
  QDate          date() const { return *_date; }
  QDate          invoiceDate() const { return *_invoiceDate; }
  QDate          invoiceDateDue() const { return *_invoiceDateDue; }
  IdType         partnerId() const { return *_partnerId; }
  IdType         partnerBankId() const { return *_partnerBankId; }
  IdType         journalId() const { return *_journalId; }

  QOdooInvoiceLine* lineAt(unsigned short index);

  QStringList propertyNames() const override
  {
    return QOdooModel::propertyNames() << "invoice_line_ids";
  }

protected:
  QVariantMap xmlrpcTransaction() const override;
  void fetchRelationships(OdooService&, QVariantMap , std::function<void ()> callback) override;

private:
  Property<QString>          _name, _ref, _paymentReference;
  Property<PaymentState>     _paymentState;
  Property<MoveType>         _moveType;
  Property<State>            _state;
  Property<QDate>            _date, _invoiceDate, _invoiceDateDue;
  Property<IdType>           _partnerId, _partnerBankId;
  Property<IdType>           _journalId;
  QVector<QOdooInvoiceLine*> _lines;
};

#endif
