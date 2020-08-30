# Client and Provider

The client is the central orchestrator of `reason-urql`, and is responsible for executing queries, mutations, and subscriptions passed to `useQuery`, `useMutation`, `useDynamicMutation` and `useSubscription`. The `Provider` wraps the root of your application and passes your `reason-urql` client instance, via React context, to hooks in your React tree.

## `Provider`

The `Provider`'s responsibility is to pass the `reason-urql` client instance down to `useQuery`, `useMutation`, `useDynamicMutation`, and `useSubcription` hooks through context. Wrap the root of your application with `Provider`.

You can access the `Provider` component by referencing `Context.Provider` after `open`ing `ReasonUrql`.

### Props

| Prop    | Type       | Description                 |
| ------- | ---------- | --------------------------- |
| `value` | `Client.t` | The `urql` client instance. |

### Example

```reason
open ReasonUrql;

let client = Client.make(~url="https://localhost:3000/graphql", ());

ReactDOMRe.renderToElementWithId(<Context.Provider value=client><App /></Context.Provider>, "root");
```

## Client

The client executes all requests in `reason-urql` and delegates all incoming responses to subscribed hooks. Its full API is below; you can also look at its associated [interface file](../src/client/Client.rei).

### Client.make

Create an instance of an `urql` client.

```reason
open ReasonUrql;

let client = Client.make(~url="https://localhost:3000/graphql", ());
```

`Client.make` accepts the following arguments:

| Argument        | Type                          | Description                                                                                                                                                                                                                                                                   |
| --------------- | ----------------------------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `url`           | `string`                      | The url of your GraphQL API.                                                                                                                                                                                                                                                  |
| `fetchOptions`  | `Client.fetchOptions('a)=?`   | Optional. A variant type representing fetch options to be used by your client. You can pass your `fetchOptions` as a plain `Fetch.requestInit` by wrapping it in the `Client.FetchOpts` tag, or instantiate it dynamically in a function wrapped by the `Client.FetchFn` tag. |
| `exchanges`     | `array(Exchanges.exchange)=?` | Optional. The array of exchanges to be used by your client.                                                                                                                                                                                                                   |
| `suspense`      | `bool=?`                      | Optional. A flag activating the experimental React suspense mode, which can be used during server-side rendering to prefetch data. Defaults to `false`.                                                                                                                       |
| `fetch`         | `fetchImpl('a)=?`             | Optional. A custom `fetch` implementation to use in lieu of `window.fetch`. For now, see [`__tests__/UrqlClient_test.re`](../__tests__/UrqlClient_test.re) for examples of how to use this option.                                                                            |
| `requestPolicy` | `Types.requestPolicy=?`       | Optional. A polymorphic variant defining the default `requestPolicy` to use for all outgoing requests. Defaults to CacheFirst. Additional options include CacheOnly, NetworkOnly, and CacheAndNetwork. See [`Types.rei`](../src/Types.rei) for the full definition.           |

`Client.make` will return an instance of an `urql` client, represented by the abstract type `Client.t`.

#### Examples

**Create a client with just a `url`.**

```reason
open ReasonUrql;

let client = Client.make(~url="https://localhost:3000/graphql", ());
```

**Create a client with `fetchOptions`.**

```reason
open ReasonUrql;

let fetchOptions =
  Client.FetchOpts(Fetch.RequestInit.make(
    ~method_=Post,
    ~headers=Fetch.HeadersInit.make({"Content-Type": "application/json"}),
    (),
  ));

let client = Client.make(~url="https://localhost:3000/graphql", ~fetchOptions, ());
```

**Create a client with `exchanges`.**

```reason
open ReasonUrql;

let client = Client.(
  make(
    ~url="https://localhost:3000/graphql",
    ~exchanges=[|
      Exchanges.debugExchange,
      Exchanges.dedupExchange,
      Exchanges.cacheExchange,
      Exchanges.fetchExchange
    |],
    ()
  )
);
```

**Create a client with a non-default requestPolicy.**

```reason
let client = Client.make(
  ~url="https://localhost:3000/graphql",
  ~requestPolicy=`CacheAndNetwork,
  ()
);
```

### `Client.executeQuery`

Imperatively execute a GraphQL query operation.

| Argument        | Type                         | Description                                                                                                                                                                                                                                                             |
| --------------- | ---------------------------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `client`        | `Client.t`                   | The `urql` client instance.                                                                                                                                                                                                                                             |
| `request`       | `Types.request`              | The `graphql_ppx` request representing your query. Generated by calling `.make()` on the `graphql_ppx` module. If you're not using `graphql_ppx`, pass a `Js.t` of the following shape: `{. "query": string, "variables": Js.Json.t, "parse": Js.Json.t => 'response }` |
| `fetchOptions`  | `Fetch.requestInit=?`        | Optional. The fetch options to apply on the outgoing request.                                                                                                                                                                                                           |
| `requestPolicy` | `Types.requestPolicy=?`      | Optional. The request policy to use to execute the query. Defaults to `"cache-first"`.                                                                                                                                                                                  |
| `url`           | `string=?`                   | Optional. The GraphQL endpoint to use for the executing operation (if different from the one specified by `Client.make`).                                                                                                                                               |
| `pollInterval`  | `int=?`                      | Optional. Instructs the client to reexecute the query every `pollInterval` milliseconds.                                                                                                                                                                                |
| `meta`          | `Types.operationDebugMeta=?` | Optional. Add metadata that is only available in development with devtools.                                                                                                                                                                                             |

`client.executeQuery` returns a [`wonka` `source`](https://wonka.kitten.sh/api/sources) containing the results of executing the query. The result record has a variant type called `response` with constructors for `Data(d)`, `Error(e)`, and `Empty`, in addition to `data` and `error` fields for accessing the raw response values if desired.

| Return                                                  | Description                                                                                                                             |
| ------------------------------------------------------- | --------------------------------------------------------------------------------------------------------------------------------------- |
| `Wonka.Types.sourceT(Client.clientResponse('response))` | A `wonka` `source` containing a record of the results of query execution. Use the `response` field on this record for pattern matching. |

#### Example

```reason
open ReasonUrql;

let client = Client.make(~url="https://localhost:3000/graphql", ());

module GetAllDogs = [%graphql
  {|
  query dogs {
    dogs {
      name
      breed
      likes
    }
  }
|}
];

Client.executeQuery(~client, ~request=GetAllDogs.make(), ())
  |> Wonka.subscribe((. data) => {
    switch(Client.(data.response)) {
      | Data(d) => /* Access data returned from executing the request. */
      | Error(e) => /* Access any errors returned from executing the request. */
      | Empty => /* Fallback if neither Data nor Error return information. */
    }
  });
```

### `Client.query`

The same as `Client.executeQuery`, but returns a `Js.Promise.t` rather than a `wonka` `source`.

| Argument        | Type                         | Description                                                                                                                                                                                                                                                             |
| --------------- | ---------------------------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `client`        | `Client.t`                   | The `urql` client instance.                                                                                                                                                                                                                                             |
| `request`       | `Types.request`              | The `graphql_ppx` request representing your query. Generated by calling `.make()` on the `graphql_ppx` module. If you're not using `graphql_ppx`, pass a `Js.t` of the following shape: `{. "query": string, "variables": Js.Json.t, "parse": Js.Json.t => 'response }` |
| `fetchOptions`  | `Fetch.requestInit=?`        | Optional. The fetch options to apply on the outgoing request.                                                                                                                                                                                                           |
| `requestPolicy` | `Types.requestPolicy=?`      | Optional. The request policy to use to execute the query. Defaults to `"cache-first"`.                                                                                                                                                                                  |
| `url`           | `string=?`                   | Optional. The GraphQL endpoint to use for the executing operation (if different from the one specified by `Client.make`).                                                                                                                                               |
| `pollInterval`  | `int=?`                      | Optional. Instructs the client to reexecute the query every `pollInterval` milliseconds.                                                                                                                                                                                |
| `meta`          | `Types.operationDebugMeta=?` | Optional. Add metadata that is only available in development with devtools.                                                                                                                                                                                             |

`Client.query` returns a `Js.Promise.t` containing the results of executing the query. The result record has a variant type called `response` with constructors for `Data(d)`, `Error(e)`, and `Empty`, in addition to `data` and `error` fields for accessing the raw response values if desired.

| Return                                           | Description                                                                                                                           |
| ------------------------------------------------ | ------------------------------------------------------------------------------------------------------------------------------------- |
| `Js.Promise.t(Client.clientResponse('response))` | A `Js.Promise.t` containing a record of the results of query execution. Use the `response` field on this record for pattern matching. |

#### Example

```reason
open ReasonUrql;

let client = Client.make(~url="https://localhost:3000/graphql", ());

module GetAllDogs = [%graphql
  {|
  query dogs {
    dogs {
      name
      breed
      likes
    }
  }
|}
];

Client.query(~client, ~request=GetAllDogs.make(), ())
  |> Js.Promise.then_(data => {
    switch(Client.(data.response)) {
      | Data(d) => /* Access data returned from executing the request. */
      | Error(e) => /* Access any errors returned from executing the request. */
      | Empty => /* Fallback if neither Data nor Error return information. */
    }
  });
```

### `Client.executeMutation`

Execute a GraphQL mutation operation.

| Argument        | Type                         | Description                                                                                                                                                                                                                                                                |
| --------------- | ---------------------------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `client`        | `Client.t`                   | The `urql` client instance.                                                                                                                                                                                                                                                |
| `request`       | `Types.request`              | The `graphql_ppx` request representing your mutation. Generated by calling `.make()` on the `graphql_ppx` module. If you're not using `graphql_ppx`, pass a `Js.t` of the following shape: `{. "query": string, "variables": Js.Json.t, "parse": Js.Json.t => 'response }` |
| `fetchOptions`  | `Fetch.requestInit=?`        | Optional. The fetch options to apply on the outgoing request.                                                                                                                                                                                                              |
| `requestPolicy` | `Types.requestPolicy=?`      | Optional. The request policy to use to execute the query. Defaults to `"cache-first"`.                                                                                                                                                                                     |
| `url`           | `string=?`                   | Optional. The GraphQL endpoint to use for the executing operation (if different from the one specified by `Client.make`).                                                                                                                                                  |
| `pollInterval`  | `int=?`                      | Optional. Instructs the client to reexecute the mutation every `pollInterval` milliseconds.                                                                                                                                                                                |
| `meta`          | `Types.operationDebugMeta=?` | Optional. Add metadata that is only available in development with devtools.                                                                                                                                                                                                |

`Client.executeMutation` returns a [`wonka` `source`](https://wonka.kitten.sh/api/sources) containing the results of executing the mutation. The result record has a variant type called `response` with constructors for `Data(d)`, `Error(e)`, and `Empty`, in addition to `data` and `error` fields for accessing the raw response values if desired.

| Return                                                  | Description                                                                                                                                |
| ------------------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------ |
| `Wonka.Types.sourceT(Client.clientResponse('response))` | A `wonka` `source` containing a record of the results of mutation execution. Use the `response` field on this record for pattern matching. |

#### Example

```reason
open ReasonUrql;

let client = Client.make(~url="https://localhost:3000/graphql", ());

module LikeDog = [%graphql
  {|
  mutation likeDog($key: ID!) {
    likeDog(key: $key) {
      name
      breed
      likes
    }
  }
|}
];

Client.executeMutation(~client, ~request=LikeDog.make(~key="VmeRTX7j-", ()), ())
  |> Wonka.subscribe((. data) => {
    switch(Client.(data.response)) {
      | Data(d) => /* Access data returned from executing the request. */
      | Error(e) => /* Access any errors returned from executing the request. */
      | Empty => /* Fallback if neither Data nor Error return information. */
    }
  });
```

### `Client.mutation`

The same as `Client.executeMutation`, but returns a `Js.Promise.t` rather than a `wonka` `source`.

| Argument        | Type                         | Description                                                                                                                                                                                                                                                             |
| --------------- | ---------------------------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `client`        | `Client.t`                   | The `urql` client instance.                                                                                                                                                                                                                                             |
| `request`       | `Types.request`              | The `graphql_ppx` request representing your query. Generated by calling `.make()` on the `graphql_ppx` module. If you're not using `graphql_ppx`, pass a `Js.t` of the following shape: `{. "query": string, "variables": Js.Json.t, "parse": Js.Json.t => 'response }` |
| `fetchOptions`  | `Fetch.requestInit=?`        | Optional. The fetch options to apply on the outgoing request.                                                                                                                                                                                                           |
| `requestPolicy` | `Types.requestPolicy=?`      | Optional. The request policy to use to execute the query. Defaults to `"cache-first"`.                                                                                                                                                                                  |
| `url`           | `string=?`                   | Optional. The GraphQL endpoint to use for the executing operation (if different from the one specified by `Client.make`).                                                                                                                                               |
| `pollInterval`  | `int=?`                      | Optional. Instructs the client to reexecute the mutation every `pollInterval` milliseconds.                                                                                                                                                                             |
| `meta`          | `Types.operationDebugMeta=?` | Optional. Add metadata that is only available in development with devtools.                                                                                                                                                                                             |

`Client.mutation` returns a `Js.Promise.t` containing the results of executing the mutation. The result record has a variant type called `response` with constructors for `Data(d)`, `Error(e)`, and `Empty`, in addition to `data` and `error` fields for accessing the raw response values if desired.

| Return                                           | Description                                                                                                                              |
| ------------------------------------------------ | ---------------------------------------------------------------------------------------------------------------------------------------- |
| `Js.Promise.t(Client.clientResponse('response))` | A `Js.Promise.t` containing a record of the results of mutation execution. Use the `response` field on this record for pattern matching. |

#### Example

```reason
open ReasonUrql;

let client = Client.make(~url="https://localhost:3000/graphql", ());

module LikeDog = [%graphql
  {|
  mutation likeDog($key: ID!) {
    likeDog(key: $key) {
      name
      breed
      likes
    }
  }
|}
];

Client.mutation(~client, ~request=LikeDog.make(~key="VmeRTX7j-", ()), ())
  |> Js.Promise.then_(data => {
    switch(Client.(data.response)) {
      | Data(d) => /* Access data returned from executing the request. */
      | Error(e) => /* Access any errors returned from executing the request. */
      | Empty => /* Fallback if neither Data nor Error return information. */
    }
  });
```

### `Client.executeSubscription`

Execute a GraphQL subscription operation. If using the `executeSubscription` method, be sure your client is configured with the `subscriptionExchange`.

| Argument        | Type                         | Description                                                                                                                                                                                                                                                                    |
| --------------- | ---------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| `client`        | `Client.t`                   | The `urql` client instance.                                                                                                                                                                                                                                                    |
| `request`       | `Types.graphqlRequest`       | The `graphql_ppx` request representing your subscription. Generated by calling `.make()` on the `graphql_ppx` module. If you're not using `graphql_ppx`, pass a `Js.t` of the following shape: `{. "query": string, "variables": Js.Json.t, "parse": Js.Json.t => 'response }` |
| `fetchOptions`  | `Fetch.requestInit=?`        | Optional. The fetch options to apply on the outgoing request.                                                                                                                                                                                                                  |
| `requestPolicy` | `Types.requestPolicy=?`      | Optional. The request policy to use to execute the query. Defaults to `"cache-first"`.                                                                                                                                                                                         |
| `url`           | `string=?`                   | Optional. The GraphQL endpoint to use for the executing operation (if different from the one specified by `Client.make`).                                                                                                                                                      |
| `pollInterval`  | `int=?`                      | Optional. Instructs the client to reexecute the mutation every `pollInterval` milliseconds.                                                                                                                                                                                    |
| `meta`          | `Types.operationDebugMeta=?` | Optional. Add metadata that is only available in development with devtools.                                                                                                                                                                                                    |

#### Example

```reason
open ReasonUrql;

module SubscribeMessages = [%graphql
  {|
  subscription subscribeMessages {
    newMessage {
      id
      message
    }
  }
|}
];

Client.executeSubscription(~client, ~request=SubscribeMessages.make(), ())
  |> Wonka.subscribe((. data) => {
    switch(Client.(data.response)) {
      | Data(d) => /* Access data returned from executing the request. */
      | Error(e) => /* Access any errors returned from executing the request. */
      | Empty => /* Fallback if neither Data nor Error return information. */
    }
  });
```
