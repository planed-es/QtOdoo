#ifndef  QODOO_PARTNER_H
# define QODOO_PARTNER_H

# include "QOdooModel.h"

class QOdooPartner : public QOdooModel
{
  Q_OBJECT
  Q_PROPERTY(QString name READ name WRITE setName)
  Q_PROPERTY(QString vat READ vat WRITE setVat)
  Q_PROPERTY(QString phone READ phone WRITE setPhone)
  Q_PROPERTY(QString city READ city WRITE setCity)
  Q_PROPERTY(QString street READ street WRITE setStreet)
  Q_PROPERTY(QString zip READ zip WRITE setZip)
  Q_PROPERTY(QString comment READ comment WRITE setComment)
  Q_PROPERTY(IdType  receivableAccountId READ receivableAccountId WRITE setReceivableAccountId)
  Q_PROPERTY(IdType  payableAccountId READ payableAccountId WRITE setPayableAccountId)
public:
  const char* odooTypename() const override { return "res.partner"; }

  enum CompanyType
  {
    NoCompanyType,
    Company,
    Individual
  };
  Q_ENUM(CompanyType)

  QOdooPartner(QObject* parent = nullptr);

  void fromVariantMap(QVariantMap);

  void setName(const QString& value) { _name.set(value); }
  void setVat(const QString& value) { _vat.set(value); }
  void setPhone(const QString& value) { _phone.set(value); }
  void setCity(const QString& value) { _city.set(value); }
  void setStreet(const QString& value) { _street.set(value); }
  void setZip(const QString& value) { _zip.set(value); }
  void setCountryId(IdType value) { _countryId.set(value); }
  void setStateId(IdType value) { _stateId.set(value); }
  void setCompanyType(CompanyType value) { _companyType.set(value); }
  void setComment(const QString& value) { _comment.set(value); }
  void setReceivableAccountId(IdType value) { _receivableAccountId.set(value); }
  void setPayableAccountId(IdType value) { _payableAccountId.set(value); }

  QString     name() const { return *_name; }
  QString     vat() const { return *_vat; }
  QString     phone() const { return *_phone; }
  QString     city() const { return *_city; }
  QString     street() const { return *_street; }
  QString     zip() const { return *_zip; }
  IdType      countryId() const { return *_countryId; }
  IdType      stateId() const { return *_stateId; }
  CompanyType companyType() const { return *_companyType; }
  QString     comment() const { return *_comment; }
  IdType      receivableAccountId() const { return *_receivableAccountId; }
  IdType      payableAccountId() const { return *_payableAccountId; }

protected:
  QVariantMap xmlrpcTransaction() const override;

private:
  StringProperty _name, _vat, _phone, _city, _street, _zip, _comment;
  IdProperty _countryId, _stateId;
  Property<CompanyType> _companyType;
  IdProperty _receivableAccountId, _payableAccountId;
};

#endif
