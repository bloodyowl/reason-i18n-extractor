module MapString = Map.Make(String);
module SetString = Set.Make(String);

let translate = (~sourcePaths, ~existingFilePath, ~destinationFilePath, ()) => {
  let oldTranslations =
    try (
      switch (Yojson.Basic.from_file(existingFilePath)) {
      | `Assoc(list) =>
        let rec aggregate = (translations, l) =>
          switch (l) {
          | [] => translations
          | [(key, `String(value)), ...tl] =>
            aggregate(translations |> MapString.add(key, value), tl)
          | _ => failwith("Malformed destination file")
          };
        aggregate(MapString.empty, list);
      | _ => failwith("Malformed destination file")
      }
    ) {
    | _ =>
      Console.log("Destination file does not exit");
      MapString.empty;
    };
  let translations =
    sourcePaths
    |> List.fold_left(
         (set, file) => {
           let ic = open_in_bin(file);
           Console.log("Parsing " ++ file);
           let lexbuf = Lexing.from_channel(ic);
           let found = Extractor.get(lexbuf);
           close_in(ic);
           set |> SetString.union(found);
         },
         SetString.empty
       );
  let json =
    `Assoc(
      translations
      |> SetString.elements
      |> List.map(key => {
           let value =
             switch (oldTranslations |> MapString.find_opt(key)) {
             | Some(key) => key
             | None => key
             };
           (key, `String(value));
         }),
    );
  let oc = open_out(destinationFilePath);
  output_string(oc, Yojson.pretty_to_string(json));
  output_string(oc, "\n");
  close_out(oc);
  ();
};
