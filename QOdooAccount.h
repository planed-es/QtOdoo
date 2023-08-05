#ifndef  QODOO_ACCOUNT_H
# define QODOO_ACCOUNT_H

# include "QOdooModel.h"

class QOdooAccount : public QOdooModel
{
  Q_OBJECT
  Q_PROPERTY(QString code READ code WRITE setCode)
  Q_PROPERTY(QString name READ name WRITE setName)
  Q_PROPERTY(bool reconcile READ reconcile WRITE setReconcile)
  Q_PROPERTY(bool deprecated READ deprecated WRITE setDeprecated)
public:
  const char* odooTypename() const override { return "account.account"; }

  QOdooAccount(QObject* parent = nullptr);

  QString code() const { return *_code; }
  QString name() const { return *_name; }
  bool    reconcile() const { return _reconcile.valueOr(false); }
  bool    deprecated() const { return _deprecated.valueOr(false); }

  void setCode(const QString& value) { _code.set(value); }
  void setName(const QString& value) { _name.set(value); }
  void setReconcile(bool value) { _reconcile.set(value); }
  void setDeprecated(bool value) { _deprecated.set(value); }

protected:
  QVariantMap xmlrpcTransaction() const override;

private:
  StringProperty _code, _name;
  Property<bool> _reconcile, _deprecated;
};

#endif
