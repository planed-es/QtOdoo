#ifndef  QODOO_COUNTRY_STATE_H
# define QODOO_COUNTRY_STATE_H

# include "QOdooModel.h"

class QOdooCountryState : public QOdooModel
{
  Q_OBJECT
  Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
  Q_PROPERTY(QString code READ code WRITE setCode NOTIFY codeChanged)
  Q_PROPERTY(IdType countryId READ countryId WRITE setCountryId NOTIFY countryIdChanged)
public:
  const char* odooTypename() const override { return "res.country.state"; }

  QOdooCountryState(QObject* parent = nullptr);

  void fromVariantMap(QVariantMap);

  void setName(const QString& value) { _name.set(value); }
  void setCode(const QString& value) { _code.set(value); }
  void setCountryId(IdType value) { _countryId.set(value); }

  QString name() const { return *_name; }
  QString code() const { return *_code; }
  IdType countryId() const { return *_countryId; }

signals:
  void nameChanged();
  void codeChanged();
  void countryIdChanged();

protected:
  QVariantMap xmlrpcTransaction() const override;

private:
  StringProperty _name, _code;
  IdProperty _countryId;
};

#endif
