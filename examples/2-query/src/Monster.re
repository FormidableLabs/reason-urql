open ReasonUrql;
open Hooks;
open MonsterStyles;

type maximum = {maximum: int};

type pokemon = {
  id: string,
  number: string,
  height: maximum,
  weight: maximum,
};

/* graphql query for GetPokemon */
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
  /* We set up the query here as we need access to the pokemon
     value passed in from GetAll */
  let request = GetPokemon.make(~name=pokemon, ());
  let ({response}, _) = useQuery(~request, ());

  switch (response) {
  | Data(data) =>
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
                 <div className=Styles.dexGrid>
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
  | Fetching => <div> "Loading"->React.string </div>
  | Error(e) =>
    switch (e##networkError->Js.Nullable.toOption) {
    | Some(_exn) => <div> "Network Error"->React.string </div>
    | None => <div> "No Network Error"->React.string </div>
    }
  | NotFound => <div> "Not Found"->React.string </div>
  };
};
