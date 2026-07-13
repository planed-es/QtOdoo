#include "QOdooAccountJournal.h"
#include "QOdooService.h"
#define prop(name, odooName)            construct_qodoo_prop(QOdooAccountJournal, name, odooName)
#define prop_def(name, odooName, value) construct_qodoo_prop_def(QOdooAccountJournal, name, odooName, value)

static const ODOO_ENUM_BEGIN(journalTypes, QOdooAccountJournal::Type, QString, QOdooAccountJournal::General)
  {QOdooAccountJournal::Sale,     "sale"},
  {QOdooAccountJournal::Purchase, "purchase"},
  {QOdooAccountJournal::Cash,     "cash"},
  {QOdooAccountJournal::Bank,     "bank"},
  {QOdooAccountJournal::General,  "general"},
ODOO_ENUM_END()

QString QOdooAccountJournal::valueFor(QOdooAccountJournal::Type value) { return journalTypes.fromEnum(value); }

QOdooAccountJournal::QOdooAccountJournal(QObject* parent) : QOdooModel(parent),
  prop(name, name),
  prop(code, code),
  prop(type, type),
  prop(restrictModeHashTable, restrict_mode_hash_table)
{
  _properties << &_name << &_code << &_type << &_restrictModeHashTable;
}

void QOdooAccountJournal::fromVariantMap(QVariantMap data)
{
  _name.loadFromVariant(data[_name.key]);
  _code.loadFromVariant(data[_code.key]);
  _type.first = journalTypes.fromValue(data[_type.key].toString());
  _restrictModeHashTable = data[_restrictModeHashTable.key].toBool();
}

void QOdooAccountJournal::onSaved()
{
  QOdooModel::onSaved();
  for (QOdooAccount* account : _accountControlIds)
    account->onSaved();
}

QFuture<void> QOdooAccountJournal::fetchRelationships(QOdooService& odoo, const QJsonObject& data)
{
  QVector<IdType> ids;

  for (const QJsonValue& value : data.value("account_control_ids").toArray())
    ids << (IdType)value.toDouble();
  return odoo.fetch<QOdooAccount>(ids, this).then([this](QVector<QOdooAccount*> accounts)
  {
    for (QOdooAccount* account : _accountControlIds)
    {
      if (account->parent() == this)
        account->deleteLater();
    }
    _accountControlIds = accounts;
  });
}

QVariantMap QOdooAccountJournal::jsonTransaction() const
{
  JsonTransaction transaction;

  transaction.addProperty(_name);
  transaction.addProperty(_code);
  transaction.addProperty<Type, QString>(_type, journalTypes.propertyUpdater());
  transaction.addProperty(_restrictModeHashTable);
  transaction.addRelationship("account_control_ids", _accountControlIds);
  return transaction;
}
