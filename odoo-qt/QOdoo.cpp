#include "QOdoo.h"

OdooService::OdooService(QUrl baseUrl)
{
  url = baseUrl;
  xmlrpc = new QXMLRpcClient();
}

OdooService::~OdooService()
{
  xmlrpc->deleteLater();
}

QFuture<bool> OdooService::authenticate(const QString& database, const QString& username, const QString& password, std::function<void()> callback)
{
  return authenticate(database, username, password)
    .then([callback, database, username](bool success)
  {
    if (success)
      callback();
    else
      qDebug() << "OdooService::authenticate failed (" << database << ", " << username << ")";
    return success;
  });
}

QFuture<bool> OdooService::authenticate(const QString& database, const QString& username, const QString& password, std::function<void(bool)> callback)
{
  return authenticate(database, username, password)
    .then([callback](bool value)
    {
      callback(value);
      return value;
    });
}

QFuture<bool> OdooService::authenticate(const QString& database, const QString& username, const QString& password)
{
  this->database = database;
  this->password = password;
  url.setPath("/xmlrpc/2/common");
  xmlrpc->setEndpoint(url);
  return xmlrpc->call("authenticate", {
    database, username, password, ""
  }).then([this](QVariant uid) -> bool
  {
    this->uid = uid.toInt(0);
    return this->uid != 0;
  });
}

QFuture<QVariant> OdooService::execute_kw(const QVariantList& params)
{
  QVariantList finalParams;

  finalParams << database << uid << password;
  for (const QVariant& param : params)
    finalParams << param;
  url.setPath("/xmlrpc/2/object");
  xmlrpc->setEndpoint(url);
  return xmlrpc->call("execute_kw", finalParams);
}

QFuture<QVariant> OdooService::findObject(const QString& objectType, int id, const QStringList& fields, std::function<void(QVariant)> callback)
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
  return execute_kw(params)
    .then([](QVariant result)
    {
      if (!QXMLRpcFault::isFault(result) && result.toList().size() > 0)
        return (result.toList().first());
      return result;
    })
    .then([callback](QVariant result)
    {
      if (callback)
        callback(result);
      return result;
    });
}

QFuture<QVariant> OdooService::findObject(const QString& objectType, int id, std::function<void(QVariant)> callback)
{
  return findObject(objectType, id, QStringList(), callback);
}

QFuture<QVariant> OdooService::findObjects(const QString& objectType, const QOdooSearchQuery& filters, std::function<void(QVariant)> callback)
{
  return objectsOperation("search_read", objectType, filters).then([callback](QVariant result)
  {
    callback(result);
    return result;
  });
}

void OdooService::countObjects(const QString& objectType, const QOdooSearchQuery& filters, std::function<void(QVariant)> callback)
{
  objectsOperation("search_count", objectType, filters).then([callback](QVariant result)
  {
    callback(result);
    return result;
  });
}

QFuture<QVariant> OdooService::objectsOperation(const QString& operation, const QString& objectType, const QOdooSearchQuery& filters)
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
  return execute_kw(params);
}

QFuture<QVariant> OdooService::findObjects(const QString& objectType, const QVector<unsigned long>& ids, const QStringList& fields, std::function<void(QVariant)> callback)
{
  QVariantList params, idsParam;
  QVariantMap keyParameters;

  keyParameters.insert("fields", QVariant::fromValue(fields));
  for (unsigned long id : ids)
    idsParam << QVariant::fromValue(id);
  params << objectType << "read";
  params.push_back(idsParam);
  params << fields;
  return execute_kw(params).then([callback](QVariant result)
  {
    callback(result);
    return result;
  });
}

void OdooService::createObject(const QString& objectType, const QVariantList& data, std::function<void(int)> callback)
{
  QVariantList params;

  params << objectType << "create";
  params.push_back(data);
  execute_kw(params).then([this, callback](QVariant id)
  {
    if (ignoreFaults || !QXMLRpcFault::isFault(id))
      callback(id.toInt());
    else
      emit faultReceived(id);
  });
}

void OdooService::updateObject(const QString& objectType, int id, const QVariantList& data, std::function<void()> callback)
{
  QVariantList params, query;

  query.push_back(QVariantList() << id);
  query << data;
  params << objectType << "write";
  params.push_back(query);
  execute_kw(params).then([this, callback](QVariant result)
  {
    if (ignoreFaults || !QXMLRpcFault::isFault(result))
      callback();
    else
      emit faultReceived(result);
  });
}

void OdooService::deleteObject(const QString& objectType, int id, std::function<void()> callback)
{
  QVariantList params;

  params << objectType << "unlink";
  params.push_back(QVariantList() << id);
  execute_kw(params).then([this, callback](QVariant result)
  {
    if (ignoreFaults || !QXMLRpcFault::isFault(result))
      callback();
    else
      emit faultReceived(result);
  });
}
