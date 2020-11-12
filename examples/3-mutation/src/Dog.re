open ReasonUrql;

module LikeDog = [%graphql
  {|
    mutation likeDog($key: ID!) {
      likeDog(key: $key) {
        likes
      }
    }
  |}
];

module TreatDog = [%graphql
  {|
    mutation treatDog($key: ID!) {
      treatDog(key: $key) {
        treats
      }
    }
  |}
];

module PatDog = [%graphql
  {|
    mutation patDog($key: ID!) {
      patDog(key: $key) {
        pats
      }
    }
  |}
];

module BellyscratchDog = [%graphql
  {|
    mutation bellyscratchDog($key: ID!) {
      bellyscratchDog(key: $key) {
        bellyscratches
      }
    }
  |}
];

[@react.component]
let make =
    (
      ~id: string,
      ~name: string,
      ~imageUrl: string,
      ~likes: int,
      ~pats: int,
      ~treats: int,
      ~bellyscratches: int,
      ~description: string,
    ) => {
  let (likeState, executeLikeMutation) =
    Hooks.useMutation(~request=LikeDog.make(~key=id, ()));

  // Example of using hooks without graphql_ppx_re (or graphql_ppx).
  let (treatState, executeTreatMutation) =
    Hooks.useMutation(~request=TreatDog.make(~key=id, ()));

  /* Example of using hooks where the variables are only known when the
     mutation runs. */
  let (patState, executePatMutation) =
    Hooks.useDynamicMutation(PatDog.definition);
  let (bellyscratchState, executeBellyscratchMutation) =
    Hooks.useDynamicMutation(BellyscratchDog.definition);

  <div className="dog">
    <img src=imageUrl alt=name className="dog__image" />
    <h3 className="dog__title"> {j|$name|j}->React.string </h3>
    <div className="dog__buttons">
      {likeState.fetching
       || treatState.fetching
       || patState.fetching
       || bellyscratchState.fetching
         ? <Loading />
         : <>
             <EmojiButton
               emoji={j|👍|j}
               count={string_of_int(likes)}
               className="emoji-button--like"
               onClick={_ => executeLikeMutation() |> ignore}
             />
             <EmojiButton
               emoji={j|✋|j}
               count={string_of_int(pats)}
               className="emoji-button--pat"
               onClick={_ => executePatMutation(~key=id, ()) |> ignore}
             />
             <EmojiButton
               emoji={j|🍖|j}
               count={string_of_int(treats)}
               className="emoji-button--treat"
               onClick={_ => executeTreatMutation() |> ignore}
             />
             <EmojiButton
               emoji={j|🐾|j}
               count={string_of_int(bellyscratches)}
               className="emoji-button--bellyscratch"
               onClick={_ =>
                 executeBellyscratchMutation(~key=id, ()) |> ignore
               }
             />
           </>}
    </div>
    <p className="dog__description"> description->React.string </p>
  </div>;
};
