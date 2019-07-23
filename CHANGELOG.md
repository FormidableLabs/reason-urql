# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.0.0-beta.2] – CombinedError and FetchOption Function - 2019-07-22

This release adds small improvements around `client.fetchOptions` and the `CombinedError` API.

### Changed

- The `FetchFn` variant of the client's `fetchOptions` argument now supports calling the function at runtime of the client rather than invoking it early in order to pass a `fetchOptions` object to `urql`.
- `UrqlCombinedError` is now properly bound using OCaml classes. A helper was added to convert the `CombinedError` `Js.t` to a record type to allow for nicer, typesafe access.

#### Diff

https://github.com/FormidableLabs/reason-urql/compare/v1.0.0-beta.1...v1.0.0-beta.2

## [1.0.0-beta.1] – 🎣 Hooks - 2019-07-06

This release includes support for `urql` hooks, including `useQuery`, `useMutation`, and `useSubscription`. It also includes additions to support proper type inference of GraphQL requests, such that users get compile-time guarantees that they are accessing data in a fully type-safe way.

### Added

- Bindings for `useQuery`, `useMutation`, and `useSubscription`.
- Bindings for `SubscriptionWithHandler`, which is to be used in cases where a custom `handler` function is passed to the `Subscription` component to accumulate subscriptions.
- The top-level `urql` context object, which exposes a `Consumer` and `Provider`, is now exposed to the user.

### Changed

- Migrated all components and examples to latest ReasonReact syntax (`"jsx: 3"`).
- Add proper type inference for hooks and components. This is achieved by passing the entire `graphql_ppx` module to the hooks, which allows them to use the `parse` method to infer the return type of data from the request.

#### Diff

https://github.com/parkerziegler/reason-urql/compare/v1.0.0-beta...v1.0.0-beta.1

## [1.0.0-beta] - 🎉 v1 (Beta) - 2019-05-16

This represents the beta release of v1, which provides support for [`urql` v1](https://github.com/FormidableLabs/urql/tree/v1.0.5).

### Added

- Bindings for `Query`, `Mutation`, and `Subscription` components.
- Bindings for `Client` with support for **exchanges**.
- Bindings for `Provider`.
- Bindings for `urql`'s type interfaces, utils, and helpers.
- Examples for `Query`, `Mutation`, `Subscription`, exposed exchanges, custom exchanges, and imperative `Client` methods.
- First tests for `UrqlClient`.

#### Diff

https://github.com/parkerziegler/reason-urql/compare/v0.1.1...v1.0.0-beta

## [0.1.1] - Coverage 💅 - 2019-01-29

### Added

- Additional tests for `Query`, `Mutation`, `Connect`, and `Client`.

### Fixed

- The `invalidate` cache operation supplied to `Connect` now properly accepts a single, optional labelled argument `~query: Query.urqlQuery`. This will now type check properly thanks to a final positional `unit` parameter in the type definition.

#### Diff

https://github.com/parkerziegler/reason-urql/compare/v0.1.0...v0.1.1

## [0.1.0] – We're Stable(ish)! - 2018-12-07

Welcome to v0.1.0 of `reason-urql`. With this release, we are moving into a stable state and will continue onwards with standard [semantic versioning](https://semver.org/).

### Added

- Automated builds with Travis CI.
- Automated coverage reporting with Coveralls.
- Unit tests for `UrqlQuery`, `UrqlMutation`, `UrqlConnect`, `UrqlClient`, and `UrqlProvider`.
- Codified utilities for making tests more concise – see `TestUtils.re`.
- Zero-config, hot reloading in the example project.
- Small, non-breaking changes to the API.

#### Diff

https://github.com/parkerziegler/reason-urql/compare/v0.1.0-beta...v0.1.0

## [0.1.0-beta] - GraphQL, meet Reason - 2018-10-07

This is the initial beta release of reason-urql! These bindings are currently untested and should not be used in production just yet. Come help us by writing tests, opening issues, and filing PRs. Thanks!
