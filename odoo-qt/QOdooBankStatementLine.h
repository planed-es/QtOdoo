#ifndef  QODOO_BANK_STATEMENT_LINE
# define QODOO_BANK_STATEMENT_LINE

# include "QOdooModel.h"

class QTODOO_LIBRARY_EXPORT QOdooBankStatementLine : public QOdooModel
{
  friend class OdooService;
  Q_OBJECT
  Q_PROPERTY(double amount READ amount WRITE setAmount NOTIFY amountChanged)
  Q_PROPERTY(QDate date READ date WRITE setDate NOTIFY dateChanged)
  Q_PROPERTY(QString paymentRef READ paymentRef WRITE setPaymentRef NOTIFY paymentRefChanged)
  Q_PROPERTY(QOdooModel::IdType journalId READ journalId WRITE setJournalId NOTIFY journalIdChanged)
  Q_PROPERTY(QOdooModel::IdType partnerId READ partnerId WRITE setPartnerId NOTIFY partnerIdChanged)
public:
  const char* odooTypename() const override { return "account.bank.statement.line"; }

  QOdooBankStatementLine(QObject* parent = nullptr);

  void fromVariantMap(QVariantMap data);

  QString paymentRef() const { return *_paymentRef; }
  double  amount() const { return *_amount; }
  QDate   date() const { return _date.first.value_or(QDate()); }
  IdType  journalId() const { return *_journalId; }
  IdType  partnerId() const { return *_partnerId; }

  void setPaymentRef(const QString& value) { _paymentRef.set(value); }
  void setAmount(double value) { _amount.set(value); }
  void setDate(QDate value) { _date.set(value); }
  void setJournalId(IdType value) { _journalId.set(value); }
  void setPartnerId(IdType value) { _partnerId.set(value); }

signals:
  void paymentRefChanged();
  void amountChanged();
  void dateChanged();
  void journalIdChanged();
  void partnerIdChanged();

protected:
  QVariantMap xmlrpcTransaction() const override;

private:
  Property<double> _amount;
  Property<QDate>  _date;
  StringProperty   _paymentRef;
  IdProperty       _journalId;
  IdProperty       _partnerId;
};

#endif
