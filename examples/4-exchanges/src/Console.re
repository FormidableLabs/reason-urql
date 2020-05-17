open ReasonUrql;

module GetAllDogs = [%graphql
  {|
  query dogs {
    dogs {
      key
      name
      breed
      likes
    }
  }
|}
];

let queryRequest = GetAllDogs.make();

module LikeDog = [%graphql
  {|
  mutation likeDog($key: ID!) {
    likeDog(key: $key) {
      name
      likes
    }
  }
  |}
];

[@react.component]
let make = (~client) => {
  React.useEffect1(
    () => {
      let mutSub = ref(() => {});

      let subscription =
        Client.executeQuery(~client, ~request=queryRequest, ())
        |> Wonka.subscribe((. data) =>
             switch (ClientTypes.(data.response)) {
             | Data(d) =>
               Js_global.setInterval(
                 () => {
                   d##dogs->Belt.Array.shuffleInPlace;
                   let mutationRequest =
                     LikeDog.make(~key=d##dogs[0]##key, ());

                   let mutationSubscription =
                     Client.executeMutation(
                       ~client,
                       ~request=mutationRequest,
                       (),
                     )
                     |> Wonka.subscribe((. response) => Js.log(response));

                   mutSub := mutationSubscription;
                   ();
                 },
                 5000,
               )
               |> ignore
             | _ => ()
             }
           );

      Some(
        () => {
          subscription.unsubscribe();
          mutSub^.unsubscribe();
        },
      );
    },
    [|client|],
  );

  <div className=ConsoleStyles.console>
    <h1 className=ConsoleStyles.title>
      "Open your console to see the "->React.string
      <code className=ConsoleStyles.code> "debugExchange"->React.string </code>
      " printing operations."->React.string
    </h1>
  </div>;
};
