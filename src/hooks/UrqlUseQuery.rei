open UrqlTypes;

type partialOperationContextFn =
  option(UrqlClient.Types.partialOperationContext) => unit;

type useQueryResponse('response) = (
  hookResponse('response),
  partialOperationContextFn,
);

let useQuery:
  (
    ~request: {
                .
                "parse": Js.Json.t => 'response,
                "query": string,
                "variables": Js.Json.t,
              },
    ~requestPolicy: requestPolicy=?,
    ~pause: bool=?,
    unit
  ) =>
  useQueryResponse('response);
