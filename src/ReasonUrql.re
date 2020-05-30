module UrqlTypes = UrqlTypes;

module Client = UrqlClient;
module ClientTypes = UrqlClientTypes;

module Context = UrqlContext;
module Provider = UrqlContext.Provider;
module Consumer = UrqlContext.Consumer;

module Request = UrqlRequest;

module CombinedError = UrqlCombinedError;

module Exchanges = UrqlExchanges;

module Hooks = {
  type hookResponse('ret, 'extensions) =
    UrqlTypes.hookResponse('ret, 'extensions) = {
      fetching: bool,
      data: option('ret),
      error: option(UrqlCombinedError.t),
      response: UrqlTypes.response('ret),
      extensions: option('extensions),
    };

  include UrqlUseMutation;
  include UrqlUseQuery;
  include UrqlUseSubscription;
  include UrqlUseClient;
};
