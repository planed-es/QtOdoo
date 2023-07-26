#include "QOdooProduct.h"

static const QMap<QOdooProduct::ProductType, QString> productTypes{
  {QOdooProduct::Consumible, "consu"},
  {QOdooProduct::Service,    "service"}
};

static QString productTypeToString(const QOdooModel::Property<QOdooProduct::ProductType>& property)
{
  auto it = productTypes.find(*property);

  if (it != productTypes.end())
    return *it;
  return QString();
}

static QOdooProduct::ProductType readProductType(const QVariant& value)
{
  QString symbol = value.toString();

  for (auto it = productTypes.begin() ; it != productTypes.end() ; ++it)
  {
    if (it.value() == symbol)
      return it.key();
  }
  return QOdooProduct::NoProductType;
}

QOdooProduct::QOdooProduct(QObject* parent) :
  QOdooModel(parent),
  _name("name"), _defaultCode("default_code"), _barCode("barcode"),
  _productType("product_type", NoProductType),
  _standardPrice("standard_price", 0.f),
  _lstPrice("lst_price", 0.f),
  _taxIds("taxes_ids")
{
  _properties << &_name << &_defaultCode << &_barCode << &_productType << &_standardPrice << &_lstPrice << &_taxIds;
}

void QOdooProduct::fetch(OdooService& odoo, IdType id, std::function<void()> callback)
{
  if (id == 0)
  {
    callback();
    return ;
  }
  odoo.findObject(odooTypename(), id, propertyNames(), [this, id, callback](QVariant result)
  {
    if (!QXMLRpcFault::isFault(result))
    {
      _id = id;
      fromVariantMap(result.toMap());
      callback();
    }
    else
      callback();
  });
}

void QOdooProduct::fromVariantMap(QVariantMap data)
{
  _name.first    = data[_name.key].toString();
  _defaultCode   = data[_defaultCode.key].toString();
  _barCode       = data[_barCode.key].toString();
  _productType   = readProductType(data[_productType.key]);
  _standardPrice = data[_standardPrice.key].toFloat();
  _lstPrice      = data[_lstPrice.key].toFloat();
  _taxIds        = data[_taxIds.key].toList();
}

QVariantMap QOdooProduct::xmlrpcTransaction() const
{
  XmlRpcTransaction transaction;

  transaction.addProperty(_name);
  transaction.addProperty(_defaultCode);
  transaction.addProperty(_barCode);
  transaction.addProperty<ProductType, QString>(_productType, std::bind(&productTypeToString, std::placeholders::_1));
  transaction.addProperty(_standardPrice);
  transaction.addProperty(_lstPrice);
  transaction.addProperty(_taxIds);
  return transaction;
}
