#include "QOdoo.h"

OdooService::OdooService(QUrl baseUrl)
{
  url = baseUrl;
}

void OdooService::authenticate(const QString& database, const QString& username, const QString& password, std::function<void()> callback)
{
  this->database = database;
  this->password = password;
  url.setPath("/xmlrpc/2/common");
  xmlrpc.setEndpoint(url);
  xmlrpc.call("authenticate", {
    database, username, password, ""
  }, [this, callback](QVariant uid)
  {
    this->uid = uid.toInt();
    callback();
  });
}

void OdooService::execute_kw(const QVariantList& params, std::function<void(QVariant)> callback)
{
  QVariantList finalParams;

  finalParams << database << uid << password;
  for (const QVariant& param : params)
    finalParams << param;
  url.setPath("/xmlrpc/2/object");
  xmlrpc.setEndpoint(url);
  xmlrpc.call("execute_kw", finalParams, callback);
}

void OdooService::findObjects(const QString& objectType, const QVariantList& filters, std::function<void(QVariant)> callback)
{
  QVariantList params;
  QVariantList wrapper, wrapperWrapper;

  wrapper.push_back(filters);
  wrapperWrapper.push_back(wrapper);
  params << objectType << "search_read";
  params.push_back(wrapperWrapper);
  execute_kw(params, callback);
}

void OdooService::createObject(const QString& objectType, const QVariantList& data, std::function<void(int)> callback)
{
  QVariantList params;

  params << objectType << "create";
  params.push_back(data);
  execute_kw(params, [callback](QVariant id) { callback(id.toInt()); });
}

void OdooService::updateObject(const QString& objectType, int id, const QVariantList& data, std::function<void()> callback)
{
  QVariantList params, query;

  query.push_back(QVariantList() << id);
  query << data;
  params << objectType << "write";
  params.push_back(query);
  execute_kw(params, [callback](QVariant) { callback(); });
}

void OdooService::deleteObject(const QString& objectType, int id, std::function<void()> callback)
{
  QVariantList params;

  params << objectType << "unlink";
  params.push_back(QVariantList() << id);
  execute_kw(params, [callback](QVariant) { callback(); });
}
