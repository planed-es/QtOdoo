#ifndef  QODOO_ACCOUNT_H
# define QODOO_ACCOUNT_H

# include "QOdooModel.h"

class QTODOO_LIBRARY_EXPORT QOdooAccount : public QOdooModel
{
  Q_OBJECT
  Q_PROPERTY(QString code READ code WRITE setCode NOTIFY codeChanged)
  Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
  Q_PROPERTY(bool reconcile READ reconcile WRITE setReconcile NOTIFY reconcileChanged)
  Q_PROPERTY(bool deprecated READ deprecated WRITE setDeprecated NOTIFY deprecatedChanged)
  Q_PROPERTY(bool nonTrade READ nonTrade WRITE setNonTrade NOTIFY nonTradeChanged)
  Q_PROPERTY(IdType groupId READ groupId NOTIFY groupIdChanged)
public:
  const char* odooTypename() const override { return "account.account"; }

  QOdooAccount(QObject* parent = nullptr);

  void fromVariantMap(QVariantMap);

  QString code() const { return *_code; }
  QString name() const { return *_name; }
  bool    reconcile() const { return _reconcile.valueOr(false); }
  bool    deprecated() const { return _deprecated.valueOr(false); }
  bool    nonTrade() const { return _nonTrade.valueOr(false); }
  IdType  groupId() const { return *_groupId; }

  void setCode(const QString& value) { _code.set(value); }
  void setName(const QString& value) { _name.set(value); }
  void setReconcile(bool value) { _reconcile.set(value); }
  void setDeprecated(bool value) { _deprecated.set(value); }
  void setNonTrade(bool value) { _nonTrade.set(value); }
  //void setGroupId(IdType value) { _groupId.set(value); }

signals:
  void codeChanged();
  void nameChanged();
  void reconcileChanged();
  void deprecatedChanged();
  void nonTradeChanged();
  void groupIdChanged();

protected:
  QVariantMap xmlrpcTransaction() const override;

private:
  StringProperty _code, _name;
  Property<bool> _reconcile, _deprecated, _nonTrade;
  IdProperty _groupId;
};

#endif
