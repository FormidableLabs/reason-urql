/* Urql's <Connect /> component, which takes a render prop argument. */
[@bs.module "urql"] external connect : ReasonReact.reactClass = "Connect";

/* The pattern below allows us to type the query prop as a polymorphic variant,
   which can be either a single Urql query or an array of Urql queries. */
type jsUnsafe;

external toJsUnsafe : 'a => jsUnsafe = "%identity";

let unwrapQuery =
    (
      ~q:
         option(
           [
             | `Query(UrqlQuery.urqlQuery)
             | `QueryArray(array(UrqlQuery.urqlQuery))
           ],
         ),
    ) =>
  switch (q) {
  | Some(`Query(q)) => toJsUnsafe(q)
  | Some(`QueryArray(qa)) => toJsUnsafe(qa)
  | None => toJsUnsafe("")
  };

/* Mutation types */
type mutationMap = Js.Dict.t(UrqlMutation.urqlMutation);

let unwrapMutation = (~m: option(mutationMap)) =>
  switch (m) {
  | Some(o) => o
  | None => Js.Dict.empty()
  };

/* Render prop types */
[@bs.deriving abstract]
type error = {message: string};

/* Helper function to convert Urql errors to option */
let convertJsErrorToReason = (err: Js.Nullable.t(error)) =>
  err |> Js.Nullable.toOption;

[@bs.deriving abstract]
type skipFetch = {
  [@bs.optional]
  skipFetch: bool,
};

type refetch = (~options: skipFetch, ~initial: bool=?) => unit;

type refreshAllFromCache = unit => unit;

[@bs.deriving abstract]
type renderArgs('data) = {
  fetching: bool,
  loaded: bool,
  data: 'data,
  error: Js.Nullable.t(error),
  refetch,
  refreshAllFromCache,
};

type siRes;

type siData;

type shouldInvalidate =
  option(
    (
      ~changedTypes: array(string),
      ~typenames: array(string),
      ~response: siRes,
      ~data: siData
    ) =>
    bool,
  );

let component = ReasonReact.statelessComponent("Connect");

let make =
    (
      ~query:
         option(
           [
             | `Query(UrqlQuery.urqlQuery)
             | `QueryArray(array(UrqlQuery.urqlQuery))
           ],
         )=?,
      ~mutation: option(mutationMap)=?,
      ~renderProp: (~result: renderArgs('data)) => ReasonReact.reactElement,
      ~cache: bool=true,
      ~typeInvalidation: bool=true,
      ~shouldInvalidate: shouldInvalidate=?,
      _children,
    ) =>
  ReasonReact.wrapJsForReason(
    ~reactClass=connect,
    ~props={
      "query": unwrapQuery(~q=query),
      "mutation": unwrapMutation(~m=mutation),
      "cache": cache,
      "typeInvalidation": typeInvalidation,
      "shouldInvalidate": shouldInvalidate,
      "children": renderProp,
    },
    _children,
  );