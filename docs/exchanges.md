# Exchanges

Exchanges are the mechanism by which `reason-urql` modifies requests before they are sent to your GraphQL API and alters responses as they are received. If you want to add some additional functionality to your GraphQL operations, this is a great place to do that. The following exchanges are provided out of the box with `reason-urql`.

#### `cacheExchange`

The `cacheExchange` provides basic caching support for your GraphQL operations. It is of type `Exchanges.t`.

#### `dedupExchange`

The `dedupExchange` will deduplicate pending operations waiting for a response. For example, if a user attempts to execute the same query by clicking a button in rapid succession, the `dedupExchange` will filter these events to a single request. It is of type `Exchanges.t`.

#### `fetchExchange`

The `fetchExchange` is responsible for actually sending your request to your GraphQL API and handling the response. It is of type `Exchanges.t`.

#### `defaultExchanges`

The above three exchanges make up `urql`'s `defaultExchanges`. When you create a client in `reason-urql` these exchanges are already applied by default. If you specify an `exchanges` array, be sure to include the specific exchanges you need. You almost always want the `defaultExchanges`, so make sure to include them using `Array.concat` or `Array.append`.

```reason
open ReasonUrql;

let client = Client.make(
  ~url="https://mygraphql.com/api",
  ~exchanges=[|
    myCustomExchange,
    Exchanges.dedupExchange,
    Exchanges.cacheExchange,
    Exchanges.fetchExchange
  |],
  ()
);
```

#### `debugExchange`

The `debugExchange` is useful for tracking how operations are passing through the exchanges pipeline. It simply logs all incoming and outgoing operations to the console. Be sure to remove this in production! It is of type `Exchanges.t`.

#### `subscriptionExchange`

The `subscriptionExchange` should be used in the event that you intend to support GraphQL subscriptions in your application through use of `useSubscription` or the client's `executeSubscription` method.

In order to use the `subscriptionExchange`, you'll need to do a little bit of setup. Specifically, you'll need to configure a subscription client of some kind that will handle the websocket connection to your GraphQL API. In `examples/5-subscription` we have some simple bindings to `subscriptions-transport-ws` that allow us to use this protocol (which is supported by Apollo). Here's an example of how to set everything up:

```reason
/* Create the subscriptionClient using APIs from your subscription client of choice.
In this case we use custom bindings to SubscriptionsTransportWS. */
let subscriptionClient =
  SubscriptionsTransportWS.make(
    ~url="ws://localhost:4001/graphql",
    ~subscriptionClientConfig=SubscriptionsTransportWS.makeClientOptions(),
    (),
  );

/* Implement the forwardSubscription function. This tells reason-urql how to handle
incoming operations of operationType 'subscription'. */
let forwardSubscription = operation => subscriptionClient##request(operation);

/* Confirgure options for reason-urql's subscriptionExchange. */
let subscriptionExchangeOpts =
  Exchanges.{forwardSubscription: forwardSubscription};

/* Create the subcription exchange. */
let subscriptionExchange =
  Exchanges.subscriptionExchange(subscriptionExchangeOpts);

/* Include the subscriptionExchange in your client's exchanges array. */
let client = Client.make(
  ~url="https://localhost:3000/graphql",
  ~exchanges=Array.append(Exchanges.defaultExchanges, [|subscriptionExchange|]),
  ()
);
```

#### `ssrExchange`

The `ssrExchange` accepts a single optional argument, `~ssrExchangeParams`, a record with two fields:

- `~initialState` - which populates the server-side rendered data with a rehydrated cache.
- `~isClient` – tell the exchange whether or not it is executing on the client or the server. By default, `reason-urql` will look at the `suspense` parameter passed to the client to determine this if `isClient` is not provided.

If using the `ssrExchange`, it should be placed after any caching exchanges, like `cacheExchange`, but before any asynchronous exchanges, like `fetchExchange`.

```reason
open ReasonUrql;

let ssrCache = Exchanges.ssrExchange();

let client = Client.make(
  ~url="http://localhost:3000",
  ~exchanges=[|
    Exchanges.dedupExchange,
    Exchanges.cacheExchange,
    ssrCache,
    Exchanges.fetchExchange
  |],
  ()
);
```

The resulting data structure returned from creating the `ssrExchange` can be accessed using two getters:

- `extractData` – this is typically used on the server-side to extract data returned from your GraphQL requests after they've been executed on the server.

```reason
let ssrCache = Exchanges.ssrExchange(~ssrExchangeParams, ());

/* Extract data from the ssrCache. (Server-side) */
let extractedData = Exchanges.extractData(~exchange=ssrCache);
```

- `restoreData` is typically used to rehydrate the client with data from the server. The `restore` argument is what allows you to reference the data returned from the server to the client.

```reason
let ssrCache = Exchanges.ssrExchange(~ssrExchangeParams, ());

/* Extract data from the ssrCache. */
let extractedData = Exchanges.restoreData(~exchange=ssrCache, ~restore=urqlData);
```

This part of the API is still quite experimental, as server-side rendering in Reason with Next.js is still in its infancy. Use with caution. For more information, read `urql`'s server-side rendering guide [here](https://github.com/FormidableLabs/urql/blob/master/docs/basics.md#server-side-rendering). To see an example of server-side rendering with `reason-urql`, check out our [`reason-urql-ssr` example](https://github.com/parkerziegler/reason-urql-ssr).

#### `composeExchanges`

`composeExchanges` is a helper function that will compose a single exchange function from an array of exchanges. Operations will be run through the provided exchanges in the order that they were provided to `composeExchanges`.

### Custom Exchanges

`reason-urql` also allows you to write your own exchanges to modify outgoing GraphQL requests and incoming responses. To read up on the basics of exchanges, check out the excellent [`urql` documentation](https://formidable.com/open-source/urql/docs/concepts/exchanges/).

The signature of an exchange in `reason-urql` is:

```reason
type t =
  exchangeInput =>
  (. Wonka.Types.sourceT(Types.operation)) =>
  Wonka.Types.sourceT(Types.operationResult);
```

`exchangeInput` here is a record containing two fields:

- `forward` – a function for forwarding the current operation onto the next exchange.
- `client` – your `reason-urql` client instance.

Let's look at an example to see how to implement our own exchanges.

#### Rewriting the `debugExchange` in Reason

To see how we can write our own custom exchange, we'll reimplement the `urql`'s native `debugExchange` in Reason:

```reason
open ReasonUrql;

/* This is the native debugExchange that ships with `urql`, re-implemented in Reason.
     Typically, you'd just add Exchanges.debugExchange to the Client's exchange array.
   */
let debugExchange = (Exchanges.{forward}) =>
  /* Notice this function is uncurried using BuckleScript's uncurrying syntax.
  We want to ensure that our exchange returns a function that accepts operations. */
  (. ops) =>
    ops
    /* Execute the supplied callback as an operation is sent out. */
    |> Wonka.tap((. op) =>
         Js.log2("[debugExchange]: Outgoing operation: ", op)
       )
    /* Forward the operation on to the next exchange. */
    |> forward
    /* Execute the supplied callback as an operation comes back in (i.e. an API request resolves). */
    |> Wonka.tap((. res) =>
         Js.log2("[debugExchange]: Completed operation: ", res)
       );
```

That's it! We've successfully re-implemented `urql`'s `debugExchange` in Reason. Because the `Wonka` library used for much of `urql`'s exchange architecture is written in Reason itself, writing our own exchanges is often more ergonomic than the JS experience. When you've written your exchange, just supply it to your client like so:

```reason
open ReasonUrql;

/* This is the native debugExchange that ships with `urql`, re-implemented in Reason.
     Typically, you'd just add Exchanges.debugExchange to the Client's exchange array.
   */
let debugExchange = (Exchanges.{forward}) =>
  (. ops) =>
    ops
    |> Wonka.tap((. op) =>
         Js.log2("[debugExchange]: Outgoing operation: ", op)
       )
    |> forward
    |> Wonka.tap((. res) =>
         Js.log2("[debugExchange]: Completed operation: ", res)
       );

let client =
  Client.make(
    ~url="https://my-graphql-endpoint.com/graphql",
    ~exchanges=[|
      debugExchange,
      Exchanges.dedupExchange,
      Exchanges.cacheExchange,
      Exchanges.fetchExchange
    |],
    (),
  );
```
