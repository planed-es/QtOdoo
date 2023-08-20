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

N.B.: Asynchronous procedures always take a callback as their last parameter. Note how we captured the `service` shared pointer within our callback: it is very important to make sure that the objects used to call such asycnhronous method are not wiped from memory during the operation, which is why using smart pointers and capturing them in the callbacks can be useful, even when the captured object isn't going to be used within the callback.

## Interacting with models

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
  });
}
```

### Ownership of fetched models

Fetched models implement `QObject` and are parented to the `OdooService` object.
To better handle their lifetime, you may use `setParent` to parent them to another
`QObject`. You may also safely delete them at anytime.

### Creating models

You may also perform change and update models, or create them from scratch using `QOdoo::save`. The following examples create a new incoming invoice from scratch using `QOdooInvoice` and `QOdooInvoiceLine`:

```c++
void odoo_test()
{
  QSharedPointer<OdooService> service(new OdooService);

  service->authenticate("database", "username", "password", [service]()
  {
    QSharedPointer<QOdooInvoice> invoice(new QOdooInvoice);
    QOdooInvoiceLine* line1 = invoice->lineAt(0);
    QOdooInvoiceLine* line2 = invoice->lineAt(1);

    invoice->setMoveType(QOdooInvoice::IncomingInvoice);
    invoice->setState(QOdooInvoice::PostedState);
    invoice->setRef("Invoice Reference");
    invoice->setName("INC_INV_001");
    invoice->setInvoiceDate(QDate::currentDate());
    invoice->setNarraiton("RGPD of sorts");
    line1->setName("product A");
    line1->setProductId(1);
    line1->setPriceUnit(50);
    line2->setName("product B");
    line2->setProductId(2);
    line2->setPriceUnit(12);
    service->save(*invoice, [service, invoice]()
    {
      qDebug() << "Invoice saved successfully.";
      qDebug() << "The invoice id in the odoo database is" << invoice->id();
    });
  });
}
```

Note that calling for `QOdooInvoice::lineAt` will either return an existing line at the given index, or create as many lines as necessary before returning a newly created line at the given index.

The `QOdoo::save` method can also be called on models that were fetched using `QOdoo::fetch`. In which case, the models will be updated instead of created.

### Destroying models

You may also remove models from the odoo database using `QOdoo::destroy`:

```c++
  QSharedPointer<OdooService> service(new OdooService);

  service->authenticate("database", "username", "password", [service]()
  {
    QOdooSearchQuery query;

    query.fields(QOdooPartner().propertyNames());
    query.where("name") == QString("Partner to remove");
    service->fetch<QOdooPartner>(query, [service](QVector<QOdooPartner*> results)
    {
      for (QOdooPartner* partner : results)
      {
        service->destroy(*partner, [service, partner]()
        {
          qDebug() << "Successfully removed partner" << partner->id() << partner->name();
        });
      }
    });
  });
```

## Collections

Collections are helper classes designed to help you navigate through pages of models.
They can be handy when working with a QML user interface:

```c++
class MyController : public QObject
{
  QOdooCollection<QOdooPartner> collection;
public:
  MyController(OdooService* service) :
    QObject(service),
    collection(*service, this)
  {
    connect(&collection, &QOdooCollectionInterface::modelsChanged, this, &MyController::onModelsChanged);
  }

  void findByType(QOdooPartner::CompanyType type)
  {
    QOdooSearchQuery query;

    query.fields(QOdooPartner().propertyNames());
    switch (type)
    {
    case QOdooPartner::Company:
      query.where("company_type") == "company";
      break ;
    case QOdooPartner::Individual:
      query.where("company_type") != "company";
      break ;
    }
    collection.setQuery(query);
  }

  void onModelsChanged()
  {
    qDebug() << "Received results, page" << collection.page();
    for (const QOdooPartner& model : collection)
      qDebug() << "- found partner:" << model.id() << model.name();
  }
};

void odoo_test()
{
  QSharedPointer<OdooService> service(new OdooService);

  service->authenticate("database", "username", "password", [service]()
  {
    MyController* controller = new MyController(service.get());

    controller.findByType(QOdooPartner::Company);
  });
}
```

TODO
