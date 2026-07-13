#include "QOdooService.h"

QOdooService::QOdooService(const QUrl& endpoint, const QByteArray& apiKey, const QByteArray& database, QObject* parent) : QObject(parent)
{
  _client.setEndpoint(endpoint);
  _client.setApiKey(apiKey);
  _client.setDatabase(database);
}
