#ifndef  QODOO_TAX_H
# define QODOO_TAX_H

# include "QOdooModel.h"

// Missing properties: tax_group_id, country_id
// Missing relationships: distribucion de facturas/distribucion para reembolsos

class QTODOO_LIBRARY_EXPORT QOdooTax : public QOdooModel
{
  friend class OdooService;
  Q_OBJECT
  Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
  Q_PROPERTY(QString description READ description WRITE setDescription NOTIFY descriptionChanged)
  Q_PROPERTY(TaxType typeTaxUse READ typeTaxUse WRITE setTypeTaxUse NOTIFY typeTaxUseChanged)
  Q_PROPERTY(AmountType amountType READ amountType WRITE setAmountType NOTIFY amountTypeChanged)
  Q_PROPERTY(TaxScope taxScope READ taxScope WRITE setTaxScope NOTIFY taxScopeChanged)
  Q_PROPERTY(bool active READ active WRITE setActive NOTIFY activeChanged)
  Q_PROPERTY(bool priceInclude READ priceInclude WRITE setPriceInclude NOTIFY priceIncludeChanged)
  Q_PROPERTY(bool includeBaseAmount READ includeBaseAmount WRITE setIncludeBaseAmount NOTIFY includeBaseAmountChanged)
  Q_PROPERTY(bool isBaseAffected READ isBaseAffected WRITE setIsBaseAffected NOTIFY isBaseAffectedChanged)
  Q_PROPERTY(float amount READ amount WRITE setAmount NOTIFY amountChanged)
public:
  const char * odooTypename() const override { return "account.tax"; }
  constexpr static const bool supportsPagination = false;

  enum TaxType
  {
    NoTaxType,
    SaleTax,
    PurchaseTax
  };
  Q_ENUM(TaxType)
  static QString valueFor(TaxType);

  enum AmountType
  {
    NoAmountType,
    GroupedAmount,
    FixedAmount,
    PercentAmount,
    DivisionAmount
  };
  Q_ENUM(AmountType)
  static QString valueFor(AmountType);

  enum TaxScope
  {
    NoTaxScope,
    Consumible,
    Service
  };
  Q_ENUM(TaxScope)
  static QString valueFor(TaxScope);

  QOdooTax(QObject* parent = nullptr);

  void fromVariantMap(QVariantMap data);

  QString name() const { return *_name; }
  QString description() const { return *_description; }
  TaxType typeTaxUse() const { return *_typeTaxUse; }
  AmountType amountType() const { return *_amountType; }
  TaxScope taxScope() const { return *_taxScope; }
  bool active() const { return *_active; }
  bool priceInclude() const { return *_priceInclude; }
  bool includeBaseAmount() const { return *_includeBaseAmount; }
  bool isBaseAffected() const { return *_isBaseAffected; }
  float amount() const { return *_amount; }

  void setName(const QString& value) { _name.set(value); }
  void setDescription(const QString& value) { _description.set(value); }
  void setTypeTaxUse(TaxType value) { _typeTaxUse.set(value); }
  void setAmountType(AmountType value) { _amountType.set(value); }
  void setTaxScope(TaxScope value) { _taxScope.set(value); }
  void setActive(bool value) { _active.set(value); }
  void setPriceInclude(bool value) { _priceInclude.set(value); }
  void setIncludeBaseAmount(bool value) { _includeBaseAmount.set(value); }
  void setIsBaseAffected(bool value) { _isBaseAffected.set(value); }
  void setAmount(float value) { _amount.set(value); }

signals:
  void nameChanged();
  void descriptionChanged();
  void typeTaxUseChanged();
  void amountTypeChanged();
  void taxScopeChanged();
  void activeChanged();
  void priceIncludeChanged();
  void includeBaseAmountChanged();
  void isBaseAffectedChanged();
  void amountChanged();

protected:
  QVariantMap xmlrpcTransaction() const override;

private:
  StringProperty       _name, _description;
  Property<TaxType>    _typeTaxUse;
  Property<AmountType> _amountType;
  Property<TaxScope>   _taxScope;
  Property<bool>       _active, _priceInclude, _includeBaseAmount, _isBaseAffected;
  Property<float>      _amount;
};

#endif
