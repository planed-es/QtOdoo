#include "QOdooProduct.h"
#include "QOdoo.h"
#define prop(name, odooName)            construct_qodoo_prop(QOdooProduct, name, odooName)
#define prop_def(name, odooName, value) construct_qodoo_prop_def(QOdooProduct, name, odooName, value)

static const ODOO_ENUM_BEGIN(productTypes, QOdooProduct::ProductType, QString, QOdooProduct::NoProductType)
  {QOdooProduct::Consumible, "consu"},
  {QOdooProduct::Service,    "service"}
ODOO_ENUM_END()

QOdooProduct::QOdooProduct(QObject* parent) :
  QOdooModel(parent),
  prop(name,              name),
  prop(defaultCode,       default_code),
  prop(barCode,           barcode),
  prop_def(productType,   detailed_type,  NoProductType),
  prop_def(standardPrice, standard_price, 0.f),
  prop_def(lstPrice,      lst_price,      0.f),
  prop(taxIds,            taxesIds)
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
  if (!data["id"].isNull())
    setId(data["id"].toInt());
  _name.first          = data[_name.key].toString();
  _defaultCode.first   = data[_defaultCode.key].toString();
  _barCode.first       = data[_barCode.key].toString();
  _productType.first   = productTypes.fromValue(data[_productType.key].toString());
  _standardPrice.first = data[_standardPrice.key].toFloat();
  _lstPrice.first      = data[_lstPrice.key].toFloat();
  _taxIds.first        = data[_taxIds.key].toList();
}

QVariantMap QOdooProduct::xmlrpcTransaction() const
{
  XmlRpcTransaction transaction;

  transaction.addProperty(_name);
  transaction.addProperty(_defaultCode);
  transaction.addProperty(_barCode);
  transaction.addProperty<ProductType, QString>(_productType, productTypes.propertyUpdater());
  transaction.addProperty(_standardPrice);
  transaction.addProperty(_lstPrice);
  transaction.addProperty(_taxIds);
  return transaction;
}
