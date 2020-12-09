/* RequestPolicy to be used for queries. */
@bs.deriving(jsConverter)
type requestPolicy = [
  | @bs.as("cache-first")
  #CacheFirst
  | @bs.as("cache-only")
  #CacheOnly
  | @bs.as("network-only")
  #NetworkOnly
  | @bs.as("cache-and-network")
  #CacheAndNetwork
]

/* OperationType for the active operation.
 Use with operationTypeToJs for proper conversion to strings. */
@bs.deriving(jsConverter)
type operationType = [
  | @bs.as("query")
  #Query
  | @bs.as("mutation")
  #Mutation
  | @bs.as("subscription")
  #Subscription
  | @bs.as("teardown")
  #Teardown
]

/* Cache outcomes for operations. */
@bs.deriving(jsConverter)
type cacheOutcome = [@bs.as("miss") #Miss | @bs.as("partial") #Partial | @bs.as("hit") #Hit]

/* Debug information on operations. */
type operationDebugMeta = {
  source: option<string>,
  cacheOutcome: option<cacheOutcome>,
  networkLatency: option<int>,
  startTime: option<int>,
}

/* The operation context object for a request. */
type operationContext = {
  additionalTypenames: option<array<string>>,
  fetch: option<(string, Fetch.requestInit) => Js.Promise.t<Fetch.response>>,
  fetchOptions: option<Fetch.requestInit>,
  requestPolicy: requestPolicy,
  url: string,
  pollInterval: option<int>,
  meta: option<operationDebugMeta>,
  suspense: option<bool>,
  preferGetMethod: option<bool>,
}

@bs.deriving({abstract: light})
type partialOperationContext = {
  @bs.optional
  additionalTypenames: array<string>,
  @bs.optional
  fetch: (string, Fetch.requestInit) => Js.Promise.t<Fetch.response>,
  @bs.optional
  fetchOptions: Fetch.requestInit,
  @bs.optional
  requestPolicy: string,
  @bs.optional
  url: string,
  @bs.optional
  pollInterval: int,
  @bs.optional
  meta: operationDebugMeta,
  @bs.optional
  suspense: bool,
  @bs.optional
  preferGetMethod: bool,
}

/* The active GraphQL operation. */
type operation = {
  key: int,
  query: string,
  variables: option<Js.Json.t>,
  kind: operationType,
  context: operationContext,
}

/* The result of the GraphQL operation. */
type operationResultJs<'dataJs> = {
  operation: operation,
  data: Js.Nullable.t<'dataJs>,
  error: option<CombinedError.t>,
  extensions: option<Js.Dict.t<string>>,
  stale: option<bool>,
}

type operationResponse<'data> =
  | Data('data)
  | Error(CombinedError.t)
  | Empty

/* The result of the GraphQL operation. */
type operationResult<'data> = {
  data: option<'data>,
  error: option<CombinedError.t>,
  extensions: option<Js.Dict.t<string>>,
  response: operationResponse<'data>,
  stale: option<bool>,
}

let operationResultToReason = (~response: operationResultJs<'dataJs>, ~parse: 'dataJs => 'data) => {
  let {error, extensions, stale}: operationResultJs<'dataJs> = response
  let data = response.data->Js.Nullable.toOption->Belt.Option.map(parse)

  let response = switch (data, error) {
  | (Some(data), _) => Data(data)
  | (None, Some(error)) => Error(error)
  | (None, None) => Empty
  }

  {data: data, error: error, extensions: extensions, stale: stale, response: response}
}

/* The GraphQL request object.
 Consists of a query, a unique key for the event, and, optionally, variables. */
type graphqlRequest = {
  key: int,
  query: string,
  variables: option<Js.Json.t>,
}

/* The signature of a graphql-ppx module. */
module type Operation = {
  module Raw: {
    type t
    type t_variables
  }
  type t
  type t_variables

  let query: string
  let parse: Raw.t => t
  let serializeVariables: t_variables => Raw.t_variables
  let variablesToJson: Raw.t_variables => Js.Json.t
}

module Hooks = {
  /* The response variant wraps the parsed result of executing a GraphQL operation. */
  type response<'data> =
    | Fetching
    | Data('data)
    | PartialData('data, array<GraphQLError.t>)
    | Error(CombinedError.t)
    | Empty

  type hookResponse<'data> = {
    operation: operation,
    fetching: bool,
    data: option<'data>,
    error: option<CombinedError.t>,
    response: response<'data>,
    extensions: option<Js.Json.t>,
    stale: bool,
  }

  type hookResponseJs<'dataJs> = {
    operation: operation,
    fetching: bool,
    data: Js.Nullable.t<'dataJs>,
    error: option<CombinedError.t>,
    extensions: option<Js.Json.t>,
    stale: bool,
  }

  @ocaml.doc(
    "
   * A function for converting the response to an urql hook from its
   * JavaScript representation to a typed Reason record.
   "
  )
  let hookResponseToReason:
    type dataJs data. (
      ~response: hookResponseJs<dataJs>,
      ~parse: dataJs => data,
    ) => hookResponse<data> =
    (~response, ~parse) => {
      let {operation, fetching, error, extensions, stale} = response

      let data = response.data->Js.Nullable.toOption->Belt.Option.map(parse)

      let response = switch (fetching, data, error) {
      | (true, None, _) => Fetching
      | (_, Some(d), None) => Data(d)
      | (_, Some(d), Some(e)) => PartialData(d, e.graphQLErrors)
      | (false, _, Some(e)) => Error(e)
      | (false, None, None) => Empty
      }

      {
        operation: operation,
        fetching: fetching,
        data: data,
        error: error,
        response: response,
        extensions: extensions,
        stale: stale,
      }
    }
}

type graphqlDefinition<'parseResult, 'composeReturnType, 'hookReturnType> = (
  // `parse`
  Js.Json.t => 'parseResult,
  // `query`
  string,
  // `composeVariables`
  (Js.Json.t => 'composeReturnType) => 'hookReturnType,
)

/* The result of executing a GraphQL request.
 Consists of optional data and errors fields. */
type executionResult = {
  errors: option<array<GraphQLError.t>>,
  data: option<Js.Json.t>,
}
