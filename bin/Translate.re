module SetString = Set.Make(String);

let readStdin = () => {
  let set = ref(SetString.empty);
  let rec read = () =>
    try (
      {
        set := set.contents |> SetString.add(stdin |> input_line);
        read();
      }
    ) {
    | End_of_file => ()
    };
  read();
  set.contents |> SetString.elements;
};

switch (Sys.argv) {
| [|_, "--help"|] =>
  Console.log(<Pastel color=Blue> "Reason i18n extractor" </Pastel>);
  Console.log(
    "Finds occurences of `T.__(translation)` in the list of files you feed in stdin",
  );
  Console.log(
    "Usage: find src/**/*.re | write-translations path/to/translations.json",
  );
| [|_, destinationFile|] =>
  Lib.Translations.translate(
    ~sourcePaths=readStdin(),
    ~existingFilePath=destinationFile,
    ~destinationFilePath=destinationFile,
    (),
  )
| _ => failwith("No destination file provided")
};
