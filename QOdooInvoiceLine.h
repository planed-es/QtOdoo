#ifndef  QODOO_INVOICE_LINE_H
# define QODOO_INVOICE_LINE_H

# include "QOdooModel.h"

class QOdooInvoiceLine : public QOdooModel
{
  Q_OBJECT
  Q_PROPERTY(QString      name      READ name WRITE setName)
  Q_PROPERTY(IdType       accountId READ accountId WRITE setAccountId)
  Q_PROPERTY(IdType       productId READ productId WRITE setProductId)
  Q_PROPERTY(double       quantity  READ quantity WRITE setQuantity)
  Q_PROPERTY(double       priceUnit READ priceUnit WRITE setPriceUnit)
  Q_PROPERTY(double       credit    READ credit WRITE setCredit)
  Q_PROPERTY(double       debit     READ debit WRITE setDebit)
  Q_PROPERTY(QVariantList taxIds    READ taxIds WRITE setTaxIds)
public:
  const char* odooTypename() const override { return "account.move.line"; }

  QOdooInvoiceLine(QObject* parent = nullptr);

  void fromVariantMap(QVariantMap data);

  QString name() const { return *_name; }
  IdType accountId() const { return *_accountId; }
  IdType productId() const { return *_productId; }
  double quantity() const { return *_quantity; }
  double priceUnit() const { return *_priceUnit; }
  double credit() const { return *_credit; }
  double debit() const { return *_debit; }
  QVariantList taxIds() const { return _taxIds.first.value_or(QVariantList()); }

  void setName(const QString& value) { _name.set(value); }
  void setAccountId(IdType value) { _accountId.set(value); }
  void setProductId(IdType value) { _productId.set(value); }
  void setQuantity(double value) { _quantity.set(value); }
  void setPriceUnit(double value) { _priceUnit.set(value); }
  void setCredit(double value) { _credit.set(value); }
  void setDebit(double value) { _debit.set(value); }
  void setTaxIds(QVariantList value) { _taxIds.set(value); }

protected:
  QVariantMap xmlrpcTransaction() const override;

private:
  StringProperty         _name;
  IdProperty             _accountId, _productId;
  Property<double>       _quantity;
  Property<double>       _priceUnit;
  Property<double>       _credit, _debit;
  Property<QVariantList> _taxIds;
};

#endif
