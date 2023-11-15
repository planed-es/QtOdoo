#ifndef  QODOO_INVOICE_H
# define QODOO_INVOICE_H

# include "QOdooModel.h"
# include "QOdooInvoiceLine.h"

class QTODOO_LIBRARY_EXPORT QOdooInvoice : public QOdooModel
{
  friend class OdooService;
  Q_OBJECT
  Q_PROPERTY(QString  name READ name WRITE setName NOTIFY nameChanged)
  Q_PROPERTY(QString  ref READ ref WRITE setRef NOTIFY refChanged)
  Q_PROPERTY(MoveType moveType READ moveType WRITE setMoveType NOTIFY moveTypeChanged)
  Q_PROPERTY(State    state READ state WRITE setState NOTIFY stateChanged)
  Q_PROPERTY(QDate    date READ date WRITE setDate NOTIFY dateChanged)
  Q_PROPERTY(QDate    invoiceDate READ invoiceDate WRITE setInvoiceDate NOTIFY invoiceDateChanged)
  Q_PROPERTY(QDate    invoiceDateDue READ invoiceDateDue WRITE setInvoiceDateDue NOTIFY invoiceDateDueChanged)
  Q_PROPERTY(QString  narration READ narration WRITE setNarration NOTIFY narrationChanged)
  Q_PROPERTY(QString  paymentReference READ paymentReference WRITE setPaymentReference NOTIFY paymentReferenceChanged)
  Q_PROPERTY(QOdooModel::IdType partnerId READ partnerId WRITE setPartnerId NOTIFY partnerIdChanged)
  Q_PROPERTY(QOdooModel::IdType partnerBankId READ partnerBankId WRITE setPartnerBankId NOTIFY partnerBankIdChanged)
  Q_PROPERTY(QOdooModel::IdType journalId READ journalId WRITE setJournalId NOTIFY journalIdChanged)
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
  void onSaved() override;

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
  void setNarration(const QString& value) { _narration.set(value); }

  QString        name() const { return *_name; }
  QString        ref() const { return *_ref; }
  QString        paymentReference() const { return *_paymentReference; }
  PaymentState   paymentState() const { return *_paymentState; }
  MoveType       moveType() const { return *_moveType; }
  State          state() const { return *_state; }
  QDate          date() const { return _date.first.value_or(QDate()); }
  QDate          invoiceDate() const { return _invoiceDate.first.value_or(QDate()); }
  QDate          invoiceDateDue() const { return _invoiceDateDue.first.value_or(QDate()); }
  IdType         partnerId() const { return *_partnerId; }
  IdType         partnerBankId() const { return *_partnerBankId; }
  IdType         journalId() const { return *_journalId; }
  QString        narration() const { return *_narration; }

  std::size_t       lineCount() const { return _lines.size(); }
  QOdooInvoiceLine* lineAt(unsigned short index);
  void              deleteLineAt(unsigned short index);

  QStringList propertyNames() const override
  {
    return QOdooModel::propertyNames() << "invoice_line_ids";
  }

  static QString valueFor(MoveType);
  static QString valueFor(State);
  static QString valueFor(PaymentState);

signals:
  void nameChanged();
  void refChanged();
  void paymentReferenceChanged();
  void narrationChanged();
  void paymentStateChanged();
  void moveTypeChanged();
  void stateChanged();
  void dateChanged();
  void invoiceDateChanged();
  void invoiceDateDueChanged();
  void partnerIdChanged();
  void partnerBankIdChanged();
  void journalIdChanged();

protected:
  QVariantMap xmlrpcTransaction() const override;
  void fetchRelationships(OdooService&, QVariantMap , std::function<void ()> callback) override;

private:
  StringProperty             _name, _ref, _paymentReference, _narration;
  Property<PaymentState>     _paymentState;
  Property<MoveType>         _moveType;
  Property<State>            _state;
  Property<QDate>            _date, _invoiceDate, _invoiceDateDue;
  IdProperty                 _partnerId, _partnerBankId;
  IdProperty                 _journalId;
  QVector<QOdooInvoiceLine*> _lines;
};

#endif
