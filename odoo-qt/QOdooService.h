#ifndef  QODOO_SERVICE_H
# define QODOO_SERVICE_H

# include <QObject>
# include <QVector>
# include <QUrl>
# include <QJsonArray>
# include <QJsonObject>
# include "QOdoo_global.h"
# include "QJson2Query.h"
# include "QOdooSearchQuery.h"
# include "QOdooModel.h"
# include "QOdooFuture.h"

// Replaces the old XML-RPC OdooService. Talks to Odoo's JSON-2 API through
// a QJson2Client and exposes everything as QFuture - there is no callback
// parameter anywhere in this class anymore. Errors surface as a failed
// QFuture carrying a QOdooException (see QOdooException.h); use
// future.then(...).onFailed(...) or a try/catch around future.result().
class QTODOO_LIBRARY_EXPORT QOdooService : public QObject
{
  Q_OBJECT
public:
  explicit QOdooService(const QUrl& endpoint, const QByteArray& apiKey, const QByteArray& database = QByteArray(), QObject* parent = nullptr);

  QJson2Client& client() { return _client; }

  // --- read side --------------------------------------------------------

  template<typename MODEL>
  QFuture<QVector<MODEL*>> fetch(const QOdooSearchQuery& query, QObject* parent = nullptr)
  {
    QFuture<QFuture<QVector<MODEL*>>> nested = _client.call(MODEL().odooTypename(), "search_read", searchArgs<MODEL>(query))
      .then([this, parent](QJsonValue value) { return buildModels<MODEL>(value.toArray(), parent); });

    return QOdooFuture::unwrap(nested);
  }

  template<typename MODEL>
  QFuture<unsigned long> count(const QOdooSearchQuery& query)
  {
    QJsonObject args;

    args["domain"] = QJsonArray::fromVariantList(query.getParams());
    return _client.call(MODEL().odooTypename(), "search_count", args).then([](QJsonValue value)
    {
      return (unsigned long)value.toDouble();
    });
  }

  template<typename MODEL>
  QFuture<MODEL*> fetch(QOdooModel::IdType id, QObject* parent = nullptr)
  {
    return fetch<MODEL>(id, MODEL().propertyNames(), parent);
  }

  template<typename MODEL>
  QFuture<MODEL*> fetch(QOdooModel::IdType id, const QStringList& fields, QObject* parent = nullptr)
  {
    if (id == 0)
      return QOdooFuture::ready<MODEL*>(nullptr);

    QJsonObject args;

    args["fields"] = QJsonArray::fromStringList(fields);

    QFuture<QFuture<MODEL*>> nested = _client.call(MODEL().odooTypename(), "read", QVariantList{QVariant::fromValue(id)}, args)
      .then([this, parent](QJsonValue value) -> QFuture<MODEL*>
      {
        QJsonArray records = value.toArray();

        if (records.isEmpty())
          return QOdooFuture::ready<MODEL*>(nullptr);
        return buildModel<MODEL>(records.first().toObject(), parent);
      });
    return QOdooFuture::unwrap(nested);
  }

  // Fetches an explicit set of ids as a single search_read - this is what
  // relationship loading (e.g. invoice lines) and the old ONE-BY-ONE
  // OdooService::fetchNext() used to do with a network round trip per id.
  template<typename MODEL>
  QFuture<QVector<MODEL*>> fetch(const QVector<QOdooModel::IdType>& ids, QObject* parent = nullptr)
  {
    QOdooSearchQuery query;
    QVariantList idList;

    if (ids.isEmpty())
      return QOdooFuture::ready(QVector<MODEL*>());
    for (QOdooModel::IdType id : ids)
      idList << QVariant::fromValue(id);
    query.fields(MODEL().propertyNames());
    query.where("id").in(idList);
    return fetch<MODEL>(query, parent);
  }

  // --- write side ---------------------------------------------------------

  QFuture<void> save(QOdooModel& model)
  {
    QJsonObject vals = QJsonObject::fromVariantMap(model.jsonTransaction());

    if (model.id() == 0)
    {
      QJsonObject args;

      args["vals_list"] = QJsonArray{vals};
      return _client.call(model.odooTypename(), "create", args).then([&model](QJsonValue result)
      {
        QJsonArray ids = result.toArray();

        model.setId(ids.isEmpty() ? 0 : (QOdooModel::IdType)ids.first().toDouble());
        model.onSaved();
      });
    }

    QJsonObject args;

    args["vals"] = vals;
    return _client.call(model.odooTypename(), "write", QVariantList{QVariant::fromValue(model.id())}, args)
      .then([&model](QJsonValue) { model.onSaved(); });
  }

  QFuture<void> destroy(const QOdooModel& model)
  {
    return _client.call(model.odooTypename(), "unlink", QVariantList{QVariant::fromValue(model.id())})
      .then([](QJsonValue) {});
  }

private:
  template<typename MODEL>
  static QJsonObject searchArgs(const QOdooSearchQuery& query)
  {
    QStringList fields = query.getFields().isEmpty() ? MODEL().propertyNames() : query.getFields();
    QJsonObject args;

    args["domain"] = QJsonArray::fromVariantList(query.getParams());
    args["fields"] = QJsonArray::fromStringList(fields);
    if (query.getOffset() > 0)
      args["offset"] = (double)query.getOffset();
    if (query.getLimit() > 0)
      args["limit"] = (double)query.getLimit();
    return args;
  }

  template<typename MODEL>
  QFuture<MODEL*> buildModel(const QJsonObject& record, QObject* parent)
  {
    MODEL* model = new MODEL(parent);

    model->fromVariantMap(record.toVariantMap());
    model->setId((QOdooModel::IdType)record.value("id").toDouble());
    return model->fetchRelationships(*this, record).then([model]() { return model; });
  }

  template<typename MODEL>
  QFuture<QVector<MODEL*>> buildModels(const QJsonArray& records, QObject* parent)
  {
    QVector<QFuture<MODEL*>> futures;

    for (const QJsonValue& record : records)
      futures << buildModel<MODEL>(record.toObject(), parent);
    return QOdooFuture::combine(futures);
  }

  QJson2Client _client;
};

#endif
