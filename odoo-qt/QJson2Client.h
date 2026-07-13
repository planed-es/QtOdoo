#ifndef  QJSON2_CLIENT_H
# define QJSON2_CLIENT_H

# include <QtNetwork/QNetworkAccessManager>
# include <QString>
# include <QUrl>
# include <QFuture>
# include <QJsonObject>
# include <QJsonValue>
# include <QVariantList>
# include "QOdoo_global.h"
# include "QOdooException.h"

class QNetworkRequest;

// Thin wrapper around Odoo's JSON-2 external API:
//   POST /json/2/<model>/<method>
//   Authorization: bearer <api key>
//   body: JSON object of the method's named arguments (+ "ids" when the
//   method targets a specific recordset).
// This class knows nothing about QOdooModel - it only deals in model/method
// name strings and QJsonObject/QJsonValue, so it can be tested or reused on
// its own.
class QTODOO_LIBRARY_EXPORT QJson2Client : public QObject
{
  Q_OBJECT
public:
  explicit QJson2Client(QObject* parent = nullptr);

  void setEndpoint(const QUrl& value) { endpoint = value; }
  void setDatabase(const QByteArray& name) { database = name; }
  void setApiKey(const QByteArray& value) { apiKey = value; }
  const QUrl& getEndpoint() const { return endpoint; }

  // Class-level call (search_read, search_count, create, ...): no "ids".
  QFuture<QJsonValue> call(const QString& model, const QString& method, QJsonObject args = QJsonObject());

  // Recordset call (read, write, unlink, or any custom method meant to run
  // "on" a set of records): adds args["ids"] = ids.
  QFuture<QJsonValue> call(const QString& model, const QString& method, const QVariantList& ids, QJsonObject args = QJsonObject());

  QFuture<QByteArray> version();

private:
  QUrl urlFor(const QString& model, const QString& method) const;
  void decorateRequest(QNetworkRequest& request) const;
  QFuture<QJsonValue> post(const QUrl& url, const QJsonObject& body);

  QUrl                  endpoint;
  QByteArray            database;
  QByteArray            apiKey;
  QNetworkAccessManager network;
};

#endif
