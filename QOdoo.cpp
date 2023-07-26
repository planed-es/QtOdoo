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
    this->uid = uid.toInt(0);
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

void OdooService::findObject(const QString& objectType, int id, const QStringList& fields, std::function<void(QVariant)> callback)
{
  QVariantList params;

  params << objectType << "read";
  params.push_back(QVariantList() << id);
  if (fields.size() > 0)
  {
    params.push_back(QVariantMap{
      {"fields", QVariant::fromValue(fields)}
    });
  }
  qDebug() << "findObjects read params: " << params;
  execute_kw(params, [callback](QVariant result)
  {
    if (!QXMLRpcFault::isFault(result) && result.toList().size() > 0)
      callback(result.toList().first());
    else
      callback(result);
  });
}

void OdooService::findObject(const QString& objectType, int id, std::function<void(QVariant)> callback)
{
  findObject(objectType, id, QStringList(), callback);
}

void OdooService::findObjects(const QString& objectType, const QOdooSearchQuery& filters, std::function<void(QVariant)> callback)
{
  QVariantList params;
  QVariantList wrapper, wrapperWrapper;
  QVariantMap keyParameters;

  wrapper.push_back(filters.params);
  wrapperWrapper.push_back(wrapper);
  params << objectType << "search_read";
  params.push_back(wrapperWrapper);
  if (filters._offset > 0)
    keyParameters.insert("offset", QVariant::fromValue(filters._offset));
  if (filters._limit > 0)
    keyParameters.insert("limit", QVariant::fromValue(filters._limit));
  if (keyParameters.keys().size() > 0)
    params.push_back(keyParameters);
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
