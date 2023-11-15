#ifndef  QODOO_ACCOUNTGROUP_H
# define QODOO_ACCOUNTGROUP_H

# include "QOdooModel.h"

class QTODOO_LIBRARY_EXPORT QOdooAccountGroup : public QOdooModel
{
  Q_OBJECT
  Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
  Q_PROPERTY(QString codePrefixStart READ codePrefixStart WRITE setCodePrefixStart NOTIFY codePrefixStartChanged)
  Q_PROPERTY(QString codePrefixEnd READ codePrefixEnd WRITE setCodePrefixEnd NOTIFY codePrefixEndChanged)
public:
  const char* odooTypename() const override { return "account.group"; }

  QOdooAccountGroup(QObject* parent = nullptr);

  void fromVariantMap(QVariantMap);

  QString name() const { return *_name; }
  QString codePrefixStart() const { return *_codePrefixStart; }
  QString codePrefixEnd() const { return *_codePrefixEnd; }

  void setName(const QString& value) { _name.set(value); }
  void setCodePrefixStart(const QString& value) { _codePrefixStart.set(value); }
  void setCodePrefixEnd(const QString& value) { _codePrefixEnd.set(value); }

signals:
  void nameChanged();
  void codePrefixStartChanged();
  void codePrefixEndChanged();

protected:
  QVariantMap xmlrpcTransaction() const override;

private:
  StringProperty _name, _codePrefixStart, _codePrefixEnd;
};

#endif
