#ifndef  QODOO_H
# define QODOO_H

#include <QXMLRpc.h>
#include <QStringList>
#include "QOdooSearchQuery.h"
#include "QOdooModel.h"

class OdooService : public QObject
{
  Q_OBJECT
public:
  OdooService(QUrl baseUrl);
  virtual ~OdooService();

  template<typename MODEL>
  void fetch(const QOdooSearchQuery& query, std::function<void(QVector<MODEL*>)> callback)
  {
    findObjects(MODEL().odooTypename(), query, [this, callback](QVariant results)
    {
      if (!QXMLRpcFault::isFault(results))
        recursivelyProcessReceivedModels<MODEL>({}, results.toList(), callback);
      else
        callback({});
    });
  }

  template<typename MODEL>
  void count(const QOdooSearchQuery& query, std::function<void(unsigned long)> callback)
  {
    countObjects(MODEL().odooTypename(), query, [this, callback](QVariant result)
    {
      unsigned long value = 0;

      if (!QXMLRpcFault::isFault(result))
        value = result.toULongLong();
      callback(value);
    });
  }

  template<typename MODEL>
  void fetch(QVector<unsigned long> ids, std::function<void(QVector<MODEL*>)> callback)
  {
    fetch(ids, MODEL().propertyNames(), callback);
  }

  template<typename MODEL>
  void fetch(QList<unsigned long> ids, QStringList properties, std::function<void(QVector<MODEL*>)> callback)
  {
    QVector<MODEL*> results;

    fetchNext(ids, results, properties, callback);
  }

  template<typename MODEL>
  void fetchNext(QList<unsigned long> ids, QVector<MODEL*> results, QStringList properties, std::function<void(QVector<MODEL*>)> callback)
  {
    if (ids.size() > 0)
    {
      unsigned long id = ids.takeFirst();
      auto afterFetch = std::bind(&OdooService::fetchNext<MODEL>, this, ids, results, properties, callback);

      fetch<MODEL>(id, properties, afterFetch);
    }
    else
      callback(results);
  }

  template<typename MODEL>
  void fetch(unsigned long id, std::function<void(MODEL*)> callback)
  {
    fetch(id, MODEL().propertyNames(), callback);
  }

  template<typename MODEL>
  void fetch(unsigned long id, QStringList properties, std::function<void(MODEL*)> callback)
  {
    if (id == 0)
    {
      callback(nullptr);
      return ;
    }
    findObject(MODEL().odooTypename(), id, properties, [this, id, callback](QVariant result)
    {
      if (!QXMLRpcFault::isFault(result))
      {
        MODEL* model = new MODEL(this);
        QVariantMap data = result.toMap();

        model->fromVariantMap(data);
        model->setId(id);
        model->fetchRelationships(*this, data, [callback, model]() { callback(model); });
      }
      else
        callback(nullptr);
    });
  }

  template<typename MODEL, typename LIST>
  void fetchRelationship(QOdooModel* self, QVariantList ids, LIST& out, std::function<void()> callback)
  {
    QOdooSearchQuery query;

    query.fields(MODEL().propertyNames());
    query.where("id").in(ids);
    fetch<MODEL>(query, [self, &out, callback](QVector<MODEL*> results)
    {
      out.clear();
      for (auto* result : results)
      {
        result->setParent(self);
        out << result;
      }
      callback();
    });
  }

  void save(QOdooModel& model, std::function<void()> callback)
  {
    QVariantList params;

    params.push_back(model.xmlrpcTransaction());
    if (model.id() == 0)
      createObject(model.odooTypename(), params, [this, &model, callback](int id) { model.setId(id); model.onSaved(); callback(); });
    else
      updateObject(model.odooTypename(), model.id(), params, [&model, callback]() { model.onSaved(); callback(); });
  }

  void destroy(const QOdooModel& model, std::function<void()> callback)
  {
    deleteObject(model.odooTypename(), model.id(), callback);
  }

  void authenticate(const QString& database, const QString& username, const QString& password, std::function<void()> callback);
  void authenticate(const QString& database, const QString& username, const QString& password, std::function<void(bool)> callback);
  void execute_kw(const QVariantList& params, std::function<void(QVariant)> callback);

  void findObject(const QString& objectType, int id, std::function<void(QVariant)> callback);
  void findObject(const QString& objectType, int id, const QStringList& fields, std::function<void(QVariant)> callback);
  void findObjects(const QString& objectType, const QOdooSearchQuery& query, std::function<void(QVariant)> callback);
  void findObjects(const QString& objectType, const QVector<unsigned long>& ids, const QStringList& fields, std::function<void(QVariant)> callback);
  void countObjects(const QString& objectType, const QOdooSearchQuery& query, std::function<void(QVariant)> callback);
  void createObject(const QString& objectType, const QVariantList& data, std::function<void(int)> callback);
  void updateObject(const QString& objectType, int id, const QVariantList& data, std::function<void()> callback);
  void deleteObject(const QString& objectType, int id, std::function<void()> callback);

  void enableFaults(bool value) { ignoreFaults = !value; }

signals:
  void faultReceived(QXMLRpcFault);

private:
  void objectsOperation(const QString& operation, const QString& objectType, const QOdooSearchQuery& query, std::function<void(QVariant)> callback);

  template<typename MODEL>
  void recursivelyProcessReceivedModels(QVector<MODEL*> models, QVariantList results, std::function<void(QVector<MODEL*>)> callback)
  {
    if (results.size())
    {
      MODEL* model = new MODEL(this);
      QVariantMap properties = results.takeFirst().toMap();

      qDebug() << "Recursively processing one model, relationships will be fetched, do not flip";
      models.push_back(model);
      model->setId(properties["id"].toULongLong());
      model->fromVariantMap(properties);
      model->fetchRelationships(*this, properties, [this, models, results, callback]()
      {
        recursivelyProcessReceivedModels(models, results, callback);
      });
      //model->fetchRelationships(*this, properties, std::bind(&OdooService::recursivelyProcessReceivedModels, this, models, results, callback));
    }
    else
      callback(models);
  }

  QUrl           url;
  QXMLRpcClient* xmlrpc;
  int            uid;
  QString        database, password;
  bool           ignoreFaults = true;
};

#endif
