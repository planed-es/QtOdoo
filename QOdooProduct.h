#ifndef  QODOO_PRODUCT_H
# define QODOO_PRODUCT_H

# include "QOdooModel.h"

class QOdooProduct : public QOdooModel
{
  Q_OBJECT
  Q_PROPERTY(QString      name READ name WRITE setName NOTIFY nameChanged)
  Q_PROPERTY(QString      defaultCode READ defaultCode WRITE setDefaultCode NOTIFY defaultCodeChanged)
  Q_PROPERTY(QString      barcode READ barcode WRITE setBarcode NOTIFY barCodeChanged)
  Q_PROPERTY(ProductType  productType READ productType WRITE setProductType NOTIFY productTypeChanged)
  Q_PROPERTY(float        standardPrice READ standardPrice WRITE setStandardPrice NOTIFY standardPriceChanged)
  Q_PROPERTY(float        lstPrice READ lstPrice WRITE setLstPrice NOTIFY lstPriceChanged)
  Q_PROPERTY(QVariantList taxesIds READ taxesIds WRITE setTaxesIds NOTIFY taxIdsChanged)
public:
  const char* odooTypename() const override { return "product.product"; }

  enum ProductType
  {
    NoProductType,
    Consumible,
    Service
  };
  Q_ENUM(ProductType)

  QOdooProduct(QObject* parent = nullptr);

  void fetch(OdooService&, IdType id, std::function<void()> callback);
  void fromVariantMap(QVariantMap data);

  QString name() const { return *_name; }
  QString defaultCode() const { return *_defaultCode; }
  QString barcode() const { return *_barCode; }
  ProductType productType() const { return *_productType; }
  float standardPrice() const { return *_standardPrice; }
  float lstPrice() const { return *_lstPrice; }
  QVariantList taxesIds() const { return *_taxIds; }

  void setName(const QString& value) { _name.set(value); }
  void setDefaultCode(const QString& value) { _defaultCode.set(value); }
  void setBarcode(const QString& value) { _barCode.set(value); }
  void setProductType(ProductType value) { _productType.set(value); }
  void setStandardPrice(float value) { _standardPrice.set(value); }
  void setLstPrice(float value) { _lstPrice.set(value); }
  void setTaxesIds(QVariantList value) { _taxIds.set(value); }

signals:
  void nameChanged();
  void defaultCodeChanged();
  void barCodeChanged();
  void productTypeChanged();
  void standardPriceChanged();
  void lstPriceChanged();
  void taxIdsChanged();

protected:
  QVariantMap xmlrpcTransaction() const override;

private:
  StringProperty         _name, _defaultCode, _barCode;
  Property<ProductType>  _productType;
  Property<float>        _standardPrice, _lstPrice;
  Property<QVariantList> _taxIds;
};

#endif
