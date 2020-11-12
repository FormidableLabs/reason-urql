[@bs.module "urql"]
external stringifyVariablies: 'a => string = "stringifyVariables";

[@bs.module "urql"]
external createRequest:
  (~query: string, ~variables: Js.Json.t=?, unit) => Types.graphqlRequest =
  "createRequest";

[@bs.module "urql"]
external makeResult:
  (Types.operation, 'a, option('b)) => Types.operationResultJs('c) =
  "makeResult";

[@bs.module "urql"]
external makeErrorResult:
  (Types.operation, Js.Exn.t, option('a)) => Types.operationResultJs('c) =
  "makeErrorResult";

[@bs.module "urql"] external formatDocument: 'a => 'a = "formatDocument";

[@bs.module "urql"] external maskTypename: 'a => 'a = "maskTypename";
