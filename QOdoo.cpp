#include "QOdoo.h"

OdooService::OdooService(QUrl baseUrl)
{
  url = baseUrl;
}

void OdooService::authenticate(const QString& database, const QString& username, const QString& password, std::function<void()> callback)
{
  authenticate(database, username, password, [callback, database, username](bool success)
  {
    if (success)
      callback();
    else
      qDebug() << "OdooService::authenticate failed (" << database << ", " << username << ")";
  });
}

void OdooService::authenticate(const QString& database, const QString& username, const QString& password, std::function<void(bool)> callback)
{
  this->database = database;
  this->password = password;
  url.setPath("/xmlrpc/2/common");
  xmlrpc.setEndpoint(url);
  xmlrpc.call("authenticate", {
    database, username, password, ""
  }, [this, callback, database, username, password](QVariant uid)
  {
    this->uid = uid.toInt(0);
    callback(this->uid != 0);
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
  objectsOperation("search_read", objectType, filters, callback);
}

void OdooService::countObjects(const QString& objectType, const QOdooSearchQuery& filters, std::function<void(QVariant)> callback)
{
  objectsOperation("search_count", objectType, filters, callback);
}

void OdooService::objectsOperation(const QString& operation, const QString& objectType, const QOdooSearchQuery& filters, std::function<void(QVariant)> callback)
{
  QVariantList params;
  QVariantList wrapper;
  QVariantMap keyParameters;

  wrapper.push_back(filters.params);
  params << objectType << operation;
  params.push_back(wrapper);
  if (operation != "search_count")
  {
    keyParameters.insert("fields", QVariant::fromValue(filters._fields));
    if (filters._offset > 0)
      keyParameters.insert("offset", QVariant::fromValue(filters._offset));
    if (filters._limit > 0)
      keyParameters.insert("limit", QVariant::fromValue(filters._limit));
    if (keyParameters.keys().size() > 0)
      params.push_back(keyParameters);
  }
  execute_kw(params, callback);
}

void OdooService::findObjects(const QString& objectType, const QVector<unsigned long>& ids, const QStringList& fields, std::function<void(QVariant)> callback)
{
  QVariantList params, idsParam;
  QVariantMap keyParameters;

  keyParameters.insert("fields", QVariant::fromValue(fields));
  for (unsigned long id : ids)
    idsParam << QVariant::fromValue(id);
  params << objectType << "read";
  params.push_back(idsParam);
  params << fields;
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
