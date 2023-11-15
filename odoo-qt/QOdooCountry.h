#ifndef  QODOO_COUNTRY_H
# define QODOO_COUNTRY_H

# include "QOdooModel.h"

class QTODOO_LIBRARY_EXPORT QOdooCountry : public QOdooModel
{
  Q_OBJECT
  Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
  Q_PROPERTY(QString code READ code WRITE setCode NOTIFY codeChanged)
  Q_PROPERTY(QOdooModel::IdType currencyId READ currencyId WRITE setCurrencyId NOTIFY currencyIdChanged)
  Q_PROPERTY(int phoneCode READ phoneCode WRITE setPhoneCode NOTIFY phoneCodeChanged)
  Q_PROPERTY(QString vatLabel READ vatLabel WRITE setVatLabel NOTIFY vatLabelChanged)
  Q_PROPERTY(bool stateRequired READ stateRequired WRITE setStateRequired NOTIFY stateRequiredChanged)
  Q_PROPERTY(bool zipRequired READ zipRequired WRITE setZipRequired NOTIFY zipRequiredChanged)
public:
  const char* odooTypename() const override { return "res.country"; }

  QOdooCountry(QObject* parent = nullptr);

  void fromVariantMap(QVariantMap);

  void setName(const QString& value) { _name.set(value); }
  void setCode(const QString& value) { _code.set(value); }
  void setCurrencyId(IdType value) { _currencyId.set(value); }
  void setPhoneCode(int value) { _phoneCode.set(value); }
  void setVatLabel(const QString& value) { _vatLabel.set(value); }
  void setStateRequired(bool value) { _stateRequired.set(value); }
  void setZipRequired(bool value) { _zipRequired.set(value); }

  QString name() const { return *_name; }
  QString code() const { return *_code; }
  IdType currencyId() const { return *_currencyId; }
  int phoneCode() const { return *_phoneCode; }
  QString vatLabel() const { return *_vatLabel; }
  bool stateRequired() const { return _stateRequired.valueOr(false); }
  bool zipRequired() const { return _zipRequired.valueOr(false); }

signals:
  void nameChanged();
  void codeChanged();
  void currencyIdChanged();
  void phoneCodeChanged();
  void vatLabelChanged();
  void stateRequiredChanged();
  void zipRequiredChanged();

protected:
  QVariantMap xmlrpcTransaction() const override;

private:
  StringProperty _name, _code;
  IdProperty _currencyId;
  Property<int> _phoneCode;
  StringProperty _vatLabel;
  Property<bool> _stateRequired, _zipRequired;
};

#endif
