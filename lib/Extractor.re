open Reason_parser;
open Migrate_parsetree.Ast_408;
open Ast_helper;
open Ast_mapper;
open Asttypes;
open Parsetree;
open Longident;

module SetString = Set.Make(String);

let get = fileContents => {
  let translations = ref(SetString.empty);
  let mapper = {
    ...default_mapper,
    expr: (mapper, item) =>
      // Match the following patterns:
      // - `T.__("translation")`
      // - `T.__({js|translation|js)`
      switch (item) {
      | {
          pexp_desc:
            Pexp_apply(
              {pexp_desc: Pexp_ident({txt: Ldot(Lident("T"), "__")})},
              [
                (
                  Nolabel,
                  {
                    pexp_desc:
                      Pexp_constant(
                        Pconst_string(translation, None | Some("js")),
                      ),
                  },
                ),
              ],
            ),
        } as x =>
        translations := translations.contents |> SetString.add(translation);
        x;
      | anythingElse => default_mapper.expr(mapper, anythingElse)
      },
  };
  let astAndComments =
    fileContents |> Reason_toolchain.RE.implementation_with_comments;
  let (ast, _) = astAndComments;
  mapper.structure(mapper, ast) |> ignore;
  translations.contents;
};
