open TestFramework;

let root = GetProjectRoot.get();

let readFile = path => {
  let ic = open_in_bin(path);
  let lexbuf = Lexing.from_channel(ic);
  lexbuf;
};

describe("should parse correctly", ({test}) =>
  test("StatelessComponentWithoutLifecycleWithChildren.re", ({expect}) => {
    let file = readFile(root ++ "/test/cases/App.re");
    let parsed = Lib.Extractor.get(file);
    expect.bool(
      parsed |> Lib.Extractor.SetString.exists(item => item == "Basic"),
    ).
      toBe(
      true,
    );
    expect.bool(
      parsed |> Lib.Extractor.SetString.exists(item => item == "Js Tagged"),
    ).
      toBe(
      true,
    );
    expect.bool(
      parsed |> Lib.Extractor.SetString.exists(item => item == "Should not"),
    ).
      toBe(
      false,
    );
  })
);
