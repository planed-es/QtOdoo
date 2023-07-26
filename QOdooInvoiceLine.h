#ifndef  QODOO_INVOICE_LINE_H
# define QODOO_INVOICE_LINE_H

# include "QOdooModel.h"

class QOdooInvoiceLine : public QOdooModel
{
  Q_OBJECT
  Q_PROPERTY(QString      name      READ name WRITE setName)
  Q_PROPERTY(IdType       accountId READ accountId WRITE setAccountId)
  Q_PROPERTY(IdType       productId READ productId WRITE setProductId)
  Q_PROPERTY(unsigned int quantity  READ quantity WRITE setQuantity)
  Q_PROPERTY(double       priceUnit READ priceUnit WRITE setPriceUnit)
  Q_PROPERTY(QVariantList taxIds    READ taxIds WRITE setTaxIds)
public:
  const char* odooTypename() const override { return "account.move.line"; }

  QOdooInvoiceLine(QObject* parent = nullptr);

  void fromVariantMap(QVariantMap data);

  const QString& name() const { return _name.first; }
  IdType accountId() const { return _accountId.first; }
  IdType productId() const { return _productId.first; }
  unsigned int quantity() const { return _quantity.first; }
  double priceUnit() const { return _priceUnit.first; }
  QVariantList taxIds() const { return _taxIds.first; }

  void setName(const QString& value) { _name.set(value); }
  void setAccountId(IdType value) { _accountId.set(value); }
  void setProductId(IdType value) { _productId.set(value); }
  void setQuantity(unsigned int value) { _quantity.set(value); }
  void setPriceUnit(double value) { _priceUnit.set(value); }
  void setTaxIds(QVariantList value) { _taxIds.set(value); }

protected:
  QVariantMap xmlrpcTransaction() const override;

private:
  Property<QString>      _name;
  Property<IdType>       _accountId, _productId;
  Property<unsigned int> _quantity;
  Property<double>       _priceUnit;
  Property<QVariantList> _taxIds;
};

#endif
