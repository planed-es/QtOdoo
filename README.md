# QtOdoo

Qt library to communicate with an Odoo server using the External API.
Primarily designed for backend work, it also provides tools useful for
integrating Odoo with QML frontends.

## Connecting to your server

```c++
void odoo_test()
{
  QUrl odooUrl("https://odoo.domain.com");
  QSharedPointer<OdooService> service(new OdooService(odooUrl));

  service->authenticate("database", "username", "password", [service]()
  {
    qDebug() << "Connected to Odoo";
  });
}
```

## Fetching models

To fetch models from Odoo, we use `OdooService`'s `fetch` method, alongside
a `QOdooSearchQuery` object. The latter lets you handle pagination, filters,
and pick the fields you wish to fetch from a model:

```c++
void odoo_test()
{
  QSharedPointer<OdooService> service(new OdooService);

  service->authenticate("database", "username", "password", [service]()
  {
    QOdooSearchQuery query;

    query.fields(QOdooPartner().propertyNames());
    query.limit(100);
    query.offset(0);
    query.where("name") != QString("Excluded name");
    service->fetch<QOdooPartner>(query, [service](QVector<QOdooPartner*> results)
    {
      for (QOdooPartner* partner : results)
        qDebug() << "Found partner" << partner->id() << partner->name();
    });
    qDebug() << "Connected to Odoo";
  });
}
```

### Ownership of fetched models

Fetched models implement `QObject` and are parented to the `OdooService` object.
To better handle their lifetime, you may use `setParent` to parent them to another
`QObject`. You may also safely delete them at anytime.

### Collections

TODO
