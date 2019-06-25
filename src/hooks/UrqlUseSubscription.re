open UrqlTypes;

type handler('acc, 'resp, 'ret) =
  | Handler((option('acc), 'resp) => 'acc): handler('acc, 'resp, 'acc)
  | NoHandler: handler(_, 'resp, 'resp);

[@bs.deriving abstract]
type useSubscriptionArgs = {
  query: string,
  [@bs.optional]
  variables: Js.Json.t,
};

[@bs.deriving abstract]
type useSubscriptionResponseJs('ret) = {
  fetching: bool,
  [@bs.optional]
  data: 'ret,
  [@bs.optional]
  error: UrqlCombinedError.t,
};

[@bs.module "urql"]
external useSubscriptionJs:
  (useSubscriptionArgs, option((option('acc), Js.Json.t) => 'acc)) =>
  array(useSubscriptionResponseJs('ret)) =
  "useSubscription";

let useSubscriptionResponseToRecord = (parse, result): hookResponse('a) => {
  let data = result->dataGet->Belt.Option.map(parse);
  let error = result->errorGet;
  let fetching = result->fetchingGet;

  let response =
    switch (fetching, data, error) {
    | (true, None, _) => UrqlTypes.Fetching
    | (true, Some(data), _) => Data(data)
    | (false, Some(data), _) => Data(data)
    | (false, _, Some(error)) => Error(error)
    | (false, None, None) => NotFound
    };

  {fetching, data, error, response};
};

let useSubscription =
    (
      type acc,
      type resp,
      type ret,
      ~handler: handler(acc, resp, ret),
      ~request: request(resp),
    )
    : hookResponse(ret) => {
  let parse = request##parse;
  let args =
    useSubscriptionArgs(
      ~query=request##query,
      ~variables=request##variables,
      (),
    );

  let state: hookResponse(ret) =
    switch (handler) {
    | NoHandler =>
      useSubscriptionJs(args, None)[0]
      |> useSubscriptionResponseToRecord(parse)
    | Handler(handler_fn) =>
      useSubscriptionJs(
        args,
        Some((acc, data) => handler_fn(acc, parse(data))),
      )[0]
      |> useSubscriptionResponseToRecord(x => x)
    };

  state;
};
