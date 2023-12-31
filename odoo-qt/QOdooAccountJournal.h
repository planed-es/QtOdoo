#ifndef  QODOO_ACCOUNT_JOURNAL_H
# define QODOO_ACCOUNT_JOURNAL_H

# include "QOdooModel.h"
# include "QOdooAccount.h"

class QTODOO_LIBRARY_EXPORT QOdooAccountJournal : public QOdooModel
{
  Q_OBJECT
  Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
  Q_PROPERTY(QString code READ code WRITE setCode NOTIFY codeChanged)
  Q_PROPERTY(Type    type READ type WRITE setType NOTIFY typeChanged)
  Q_PROPERTY(bool    restrictModeHashTable READ restrictModeHashTable WRITE setRestrictModeHashTable NOTIFY restrictModeHashTableChanged)
public:
  const char* odooTypename() const override { return "account.journal"; }

  enum Type
  {
    Sale,
    Purchase,
    Cash,
    Bank,
    General
  };
  Q_ENUM(Type)
  static QString valueFor(Type value);

  QOdooAccountJournal(QObject* parent = nullptr);

  void fromVariantMap(QVariantMap);
  void onSaved() override;

  void setName(const QString& value) { _name.set(value); }
  void setCode(const QString& value) { _code.set(value); }
  void setType(Type value) { _type.set(value); }
  void setRestrictModeHashTable(bool value) { _restrictModeHashTable.set(value); }

  QString name() const { return *_name; }
  QString code() const { return *_code; }
  Type    type() const { return *_type; }
  bool    restrictModeHashTable() const { return *_restrictModeHashTable; }

  QStringList propertyNames() const override
  {
    return QOdooModel::propertyNames() << "account_control_ids";
  }

signals:
  void nameChanged();
  void codeChanged();
  void typeChanged();
  void restrictModeHashTableChanged();

protected:
  QVariantMap xmlrpcTransaction() const override;
  void fetchRelationships(OdooService&, QVariantMap , std::function<void ()> callback) override;

private:
  StringProperty _name, _code;
  Property<Type> _type;
  Property<bool> _restrictModeHashTable;
  QVector<QOdooAccount*> _accountControlIds;
};

#endif
