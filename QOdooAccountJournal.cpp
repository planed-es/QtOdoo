#include "QOdooAccountJournal.h"
#include "QOdoo.h"

static const ODOO_ENUM_BEGIN(journalTypes, QOdooAccountJournal::Type, QString, QOdooAccountJournal::General)
  {QOdooAccountJournal::Sale,     "sale"},
  {QOdooAccountJournal::Purchase, "purchase"},
  {QOdooAccountJournal::Cash,     "cash"},
  {QOdooAccountJournal::Bank,     "bank"},
  {QOdooAccountJournal::General,  "general"},
ODOO_ENUM_END()

QString QOdooAccountJournal::valueFor(QOdooAccountJournal::Type value) { return journalTypes.fromEnum(value); }

QOdooAccountJournal::QOdooAccountJournal(QObject* parent) : QOdooModel(parent),
  _name("name"), _code("code"), _type("type"),
  _restrictModeHashTable("restrict_mode_hash_table")
{
  _properties << &_name << &_code << &_type << &_restrictModeHashTable;
}

void QOdooAccountJournal::fromVariantMap(QVariantMap data)
{
  _name.first = data[_name.key].toString();
  _code.first = data[_code.key].toString();
  _type.first = journalTypes.fromValue(data[_type.key].toString());
  _restrictModeHashTable = data[_restrictModeHashTable.key].toBool();
}

void QOdooAccountJournal::onSaved()
{
  QOdooModel::onSaved();
  for (QOdooAccount* account : _accountControlIds)
    account->onSaved();
}

void QOdooAccountJournal::fetchRelationships(OdooService& odoo, QVariantMap data, std::function<void()> callback)
{
  odoo.fetchRelationship<QOdooAccount>(
    this, data["account_control_ids"].toList(), _accountControlIds, callback
  );
}

QVariantMap QOdooAccountJournal::xmlrpcTransaction() const
{
  XmlRpcTransaction transaction;

  transaction.addProperty(_name);
  transaction.addProperty(_code);
  transaction.addProperty<Type, QString>(_type, journalTypes.propertyUpdater());
  transaction.addProperty(_restrictModeHashTable);
  transaction.addRelationship("account_control_ids", _accountControlIds);
  return transaction;
}
