#ifndef  QODOO_H
# define QODOO_H

#include <QXMLRpc.h>
#include <QStringList>
#include "QOdooSearchQuery.h"

class OdooService : public QObject
{
  Q_OBJECT
public:
  OdooService(QUrl baseUrl);

  template<typename MODEL>
  void fetch(const QOdooSearchQuery& query, std::function<void(QVector<MODEL*>)> callback)
  {
    findObjects(MODEL().odooTypename(), query.params, [this, callback](QVariant results)
    {
      QVector<MODEL*> models;

      if (!QXMLRpcFault::isFault(results))
      {
        for (QVariant data : results.toList())
        {
          MODEL* model = new MODEL(this);

          model->fromVariantMap(data.toMap());
          models.push_back(model);
        }
      }
      callback(models);
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

        model->_id = id;
        model->fromVariantMap(data);
        model->fetchRelationships(*this, data, [callback, model]() { callback(model); });
      }
      else
        callback(nullptr);
    });
  }

  template<typename MODEL>
  void save(MODEL& model, std::function<void()> callback)
  {
    QVariantList params;

    params.push_back(model.xmlrpcTransaction());
    if (model.id() == 0)
      createObject(model.odooTypename(), params, [this, &model, callback](int id) { model._id = id; callback(); });
    else
      updateObject(model.odooTypename(), model.id(), params, callback);
  }

  template<typename MODEL>
  void destroy(MODEL& model, std::function<void()> callback)
  {
    deleteObject(model.odooTypename(), model.id(), callback);
  }

  void authenticate(const QString& database, const QString& username, const QString& password, std::function<void()> callback);
  void execute_kw(const QVariantList& params, std::function<void(QVariant)> callback);

  void findObject(const QString& objectType, int id, std::function<void(QVariant)> callback);
  void findObject(const QString& objectType, int id, const QStringList& fields, std::function<void(QVariant)> callback);
  void findObjects(const QString& objectType, const QOdooSearchQuery& query, std::function<void(QVariant)> callback);
  void createObject(const QString& objectType, const QVariantList& data, std::function<void(int)> callback);
  void updateObject(const QString& objectType, int id, const QVariantList& data, std::function<void()> callback);
  void deleteObject(const QString& objectType, int id, std::function<void()> callback);

private:
  QUrl          url;
  QXMLRpcClient xmlrpc;
  int           uid;
  QString       database, password;
};

#endif
