open TestFramework;

let root = GetProjectRoot.get();

describe("Translations", ({test}) => {
  test("creates file if not existant", ({expect}) => {
    Lib.Translations.translate(
      ~sourcePaths=[root ++ "/test/cases/App.re"],
      ~existingFilePath=root ++ "/test/cases/translations-doesnt-exist.json",
      ~destinationFilePath=
        root ++ "/test/cases/translations-doesnt-exist.json",
      ()
    );
    expect.file(root ++ "/test/cases/translations-doesnt-exist.json").
      toMatchSnapshot();
    Sys.remove(root ++ "/test/cases/translations-doesnt-exist.json");
  });
  test("merges with previous translations", ({expect}) => {
    Lib.Translations.translate(
      ~sourcePaths=[root ++ "/test/cases/App.re"],
      ~existingFilePath=root ++ "/test/cases/translations.json",
      ~destinationFilePath=
        root ++ "/test/cases/translations-output.json",
      ()
    );
    expect.file(root ++ "/test/cases/translations-output.json").
      toMatchSnapshot();
    Sys.remove(root ++ "/test/cases/translations-output.json");
  });
});
