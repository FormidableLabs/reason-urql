[@bs.deriving abstract]
type subscriptionRenderPropsJs('a) = {
  fetching: bool,
  [@bs.optional]
  data: 'a,
  [@bs.optional]
  error: UrqlCombinedError.t,
};

type subscriptionRenderProps('a) = {
  fetching: bool,
  data: option('a),
  error: option(UrqlCombinedError.t),
  response: UrqlTypes.response('a),
};

type handler('a, 'b) =
  (~prevSubscriptions: option('a), ~subscription: 'b) => 'a;

module SubscriptionJs = {
  [@bs.module "urql"] [@react.component]
  external make:
    (
      ~query: string,
      ~variables: option(Js.Json.t),
      ~handler: option(handler('a, 'b)),
      ~children: subscriptionRenderPropsJs('a) => React.element
    ) =>
    React.element =
    "Subscription";
};

let urqlDataToRecord = (result: subscriptionRenderPropsJs('a)) => {
  let data = result->dataGet;
  let error = result->errorGet;
  let fetching = result->fetchingGet;

  let response: UrqlTypes.response('a) =
    switch (fetching, data, error) {
    | (true, _, _) => Fetching
    | (false, Some(data), _) => Data(data)
    | (false, _, Some(error)) => Error(error)
    | (false, None, None) => NotFound
    };

  {fetching, data, error, response};
};

[@react.component]
let make =
    (
      ~query: string,
      ~variables: Js.Json.t=?,
      ~handler: handler('a, 'b)=?,
      ~children: subscriptionRenderProps('a) => React.element,
    ) =>
  <SubscriptionJs query ?variables ?handler>
    {result => result |> urqlDataToRecord |> children}
  </SubscriptionJs>;