#ifndef  QODOO_H
# define QODOO_H

#include <QXMLRpc.h>

class OdooService
{
public:
  OdooService(QUrl baseUrl);

  void authenticate(const QString& database, const QString& username, const QString& password, std::function<void()> callback);
  void execute_kw(const QVariantList& params, std::function<void(QVariant)> callback);

  void findObjects(const QString& objectType, const QVariantList& filters, std::function<void(QVariant)> callback);
  void createObject(const QString& objectType, const QVariantList& data, std::function<void(int)> callback);
  void updateObject(const QString& objectType, int id, const QVariantList& data, std::function<void()> callback);
  void deleteObject(const QString& objectType, int id, std::function<void()> callback);

  void findPartners(const QVariantList& filters, std::function<void(QVariant)> callback) { findObjects("res.partner", filters, callback); }
  void createPartner(const QVariantList& data, std::function<void(int)> callback) { createObject("res.partner", data, callback); }
  void updatePartner(int id, const QVariantList& data, std::function<void()> callback) { updateObject("res.partner", id, data, callback); }
  void deletePartner(int id, std::function<void()> callback) { deleteObject("res.partner", id, callback); }

  void createAccountMovement(const QVariantList& data, std::function<void()> callback) { createObject("account.move", data, callback); }

private:
  QUrl          url;
  QXMLRpcClient xmlrpc;
  int           uid;
  QString       database, password;
};

#endif
