open ReasonUrql;
open PokemonStyles;

module GetPokemon = [%graphql
  {|
  query pokemon($name: String!) {
    pokemon(name: $name) {
      name
      classification
      height {
        maximum
      }
      weight {
        maximum
      }
      image
    }
  }
|}
];

[@react.component]
let make = (~pokemon: string) => {
  let request = GetPokemon.make(~name=pokemon, ());
  let (Hooks.{response}, _) = Hooks.useQuery(~request, ());

  switch (response) {
  | Fetching => <div> "Loading"->React.string </div>
  | Data(data)
  | PartialData(data, _) =>
    switch (data##pokemon) {
    | Some(pokemon) =>
      switch (
        pokemon##image,
        pokemon##classification,
        pokemon##name,
        pokemon##height,
        pokemon##weight,
      ) {
      | (
          Some(image),
          Some(classification),
          Some(name),
          Some(height),
          Some(weight),
        ) =>
        <section className=Styles.dexContainer>
          <div className=Styles.dex>
            <div className=Styles.dexImageContainer>
              <img className=Styles.dexImage src=image />
            </div>
            <div className=Styles.dexText>
              <h1 className=Styles.dexTitle> name->React.string </h1>
              <h2 className=Styles.dexSubTitle>
                classification->React.string
              </h2>
              {switch (height##maximum, weight##maximum) {
               | (Some(heightMax), Some(weightMax)) =>
                 <div>
                   <p> {("Height: " ++ heightMax)->React.string} </p>
                   <p> {("Weight: " ++ weightMax)->React.string} </p>
                 </div>
               | (_, _) => React.null
               }}
            </div>
          </div>
        </section>
      | (_, _, _, _, _) => React.null
      }
    | None => React.null
    }
  | Error(e) => <div> e.message->React.string </div>
  | Empty => <div> "Not Found"->React.string </div>
  };
};
