// open Reason_parser;
// open Migrate_parsetree.Ast_404;
// open Ast_helper;
// open Ast_mapper;
// open Asttypes;
// open Parsetree;
// open Longident;
// type moduleLocation =
//   | TopLevel
//   | Nested(array(string));
// module StringArrayMap =
//   Map.Make({
//     type t = moduleLocation;
//     let compare = compare;
//   });
// let printModuleLocation =
//   fun
//   | TopLevel => "TopLevel"
//   | Nested(keys) =>
//     keys
//     |> Array.fold_left(
//          (acc, value) => (acc == "" ? acc : acc ++ ".") ++ value,
//          "",
//        );
// let childrenUsageMap = ref(StringArrayMap.empty);
// let logPrefix =
//   Pastel.(<Pastel backgroundColor=Cyan color=Black> " UpgradeRR " </Pastel>);
// let rec implementionMapStructureItem = (key, mapper, item) =>
//   switch (item) {
//   | {
//       pstr_desc:
//         Pstr_module(
//           {
//             pmb_name: {txt: moduleName},
//             pmb_expr:
//               {
//                 pmod_desc:
//                   Pmod_functor(
//                     name,
//                     moduleType,
//                     {pmod_desc: Pmod_structure(structure)} as moduleExpression,
//                   ),
//               } as functorExpression,
//           } as moduleBinding,
//         ),
//     } as structureItem => {
//       ...structureItem,
//       pstr_desc:
//         Pstr_module({
//           ...moduleBinding,
//           pmb_expr: {
//             ...functorExpression,
//             pmod_desc:
//               Pmod_functor(
//                 name,
//                 moduleType,
//                 {
//                   ...moduleExpression,
//                   pmod_desc:
//                     Pmod_structure(
//                       structure
//                       |> List.map(structureItem =>
//                            implementionMapStructureItem(
//                              switch (key) {
//                              | TopLevel => Nested([|moduleName|])
//                              | Nested(keys) =>
//                                Nested(Array.concat([keys, [|moduleName|]]))
//                              },
//                              mapper,
//                              structureItem,
//                            )
//                          ),
//                     ),
//                 },
//               ),
//           },
//         }),
//     }
//   | {
//       pstr_desc:
//         Pstr_module(
//           {
//             pmb_name: {txt: moduleName},
//             pmb_expr:
//               {pmod_desc: Pmod_structure(structure)} as moduleExpression,
//           } as moduleBinding,
//         ),
//     } as structureItem => {
//       ...structureItem,
//       pstr_desc:
//         Pstr_module({
//           ...moduleBinding,
//           pmb_expr: {
//             ...moduleExpression,
//             pmod_desc:
//               Pmod_structure(
//                 structure
//                 |> List.map(structureItem =>
//                      implementionMapStructureItem(
//                        switch (key) {
//                        | TopLevel => Nested([|moduleName|])
//                        | Nested(keys) =>
//                          Nested(Array.concat([keys, [|moduleName|]]))
//                        },
//                        mapper,
//                        structureItem,
//                      )
//                    ),
//               ),
//           },
//         }),
//     }
//   | {
//       pstr_desc:
//         Pstr_value(
//           recFlag,
//           [
//             {
//               pvb_pat: {ppat_desc: Ppat_var({txt: "make"})},
//               pvb_expr: expression,
//             } as value,
//           ],
//         ),
//     } as letBinding =>
//     let foundReturnComponent = ref(false);
//     let rec mapBody = expression =>
//       switch (expression) {
//       | {
//           pexp_desc:
//             Pexp_fun(Nolabel, None, {ppat_desc: Ppat_any} as pattern, body),
//         } as item => {
//           ...item,
//           pexp_desc: Pexp_fun(Nolabel, None, pattern, mapBody(body)),
//         }
//       | {pexp_desc: Pexp_fun(label, defaultValue, pattern, body)} as item => {
//           ...item,
//           pexp_desc: Pexp_fun(label, defaultValue, pattern, mapBody(body)),
//         }
//       | {pexp_desc: Pexp_let(recFlag, valueBindings, next)} as item => {
//           ...item,
//           pexp_desc: Pexp_let(recFlag, valueBindings, mapBody(next)),
//         }
//       | {
//           pexp_desc:
//             Pexp_record(
//               items,
//               Some({pexp_desc: Pexp_ident({txt: Lident("component")})}),
//             ),
//         } as record =>
//         foundReturnComponent := true;
//         {
//           pexp_loc: Location.none,
//           pexp_attributes: [],
//           pexp_desc:
//             Pexp_apply(
//               {
//                 pexp_desc:
//                   Pexp_ident({
//                     txt: Lident("ReactCompat.useRecordApi"),
//                     loc: Location.none,
//                   }),
//                 pexp_loc: Location.none,
//                 pexp_attributes: [],
//               },
//               [(Nolabel, record)],
//             ),
//         };
//       | _ as expression => expression
//       };
//     let body = mapBody(expression);
//     let rec mapChildren = expression =>
//       switch (expression) {
//       | {
//           pexp_desc:
//             Pexp_fun(Nolabel, None, {ppat_desc: Ppat_any} as pattern, body),
//         } as item =>
//         if (foundReturnComponent^) {
//           childrenUsageMap.contents =
//             childrenUsageMap.contents |> StringArrayMap.add(key, false);
//           {
//             ...item,
//             pexp_desc:
//               Pexp_fun(
//                 Nolabel,
//                 None,
//                 {
//                   ...pattern,
//                   ppat_desc:
//                     Ppat_construct(
//                       {loc: Location.none, txt: Lident("()")},
//                       None,
//                     ),
//                 },
//                 mapChildren(body),
//               ),
//           };
//         } else {
//           item;
//         }
//       | {
//           pexp_desc:
//             Pexp_fun(
//               Nolabel,
//               None,
//               {ppat_desc: Ppat_var({txt})} as pattern,
//               body,
//             ),
//         } as item
//           when txt.[0] == '_' =>
//         if (foundReturnComponent^) {
//           childrenUsageMap.contents =
//             childrenUsageMap.contents |> StringArrayMap.add(key, false);
//           {
//             ...item,
//             pexp_desc:
//               Pexp_fun(
//                 Nolabel,
//                 None,
//                 {
//                   ...pattern,
//                   ppat_desc:
//                     Ppat_construct(
//                       {loc: Location.none, txt: Lident("()")},
//                       None,
//                     ),
//                 },
//                 mapChildren(body),
//               ),
//           };
//         } else {
//           item;
//         }
//       | {
//           pexp_desc:
//             Pexp_fun(
//               Nolabel,
//               None,
//               {ppat_desc: Ppat_var({txt})} as pattern,
//               body,
//             ),
//         } as item =>
//         if (foundReturnComponent^) {
//           childrenUsageMap.contents =
//             childrenUsageMap.contents |> StringArrayMap.add(key, true);
//           {
//             ...item,
//             pexp_desc:
//               Pexp_fun(
//                 Labelled("children"),
//                 None,
//                 {...pattern, ppat_desc: Ppat_var({loc: Location.none, txt})},
//                 {
//                   ...item,
//                   pexp_desc:
//                     Pexp_fun(
//                       Nolabel,
//                       None,
//                       {
//                         ...pattern,
//                         ppat_desc:
//                           Ppat_construct(
//                             {loc: Location.none, txt: Lident("()")},
//                             None,
//                           ),
//                       },
//                       {
//                         ...item,
//                         pexp_desc:
//                           Pexp_let(
//                             Nonrecursive,
//                             [
//                               {
//                                 pvb_pat: {
//                                   ppat_desc:
//                                     Ppat_var({loc: Location.none, txt}),
//                                   ppat_loc: Location.none,
//                                   ppat_attributes: [],
//                                 },
//                                 pvb_expr: {
//                                   pexp_desc:
//                                     Pexp_apply(
//                                       {
//                                         pexp_desc:
//                                           Pexp_ident({
//                                             loc: Location.none,
//                                             txt:
//                                               Lident(
//                                                 "React.Children.toArray",
//                                               ),
//                                           }),
//                                         pexp_loc: Location.none,
//                                         pexp_attributes: [],
//                                       },
//                                       [
//                                         (
//                                           Nolabel,
//                                           {
//                                             pexp_desc:
//                                               Pexp_ident({
//                                                 loc: Location.none,
//                                                 txt: Lident(txt),
//                                               }),
//                                             pexp_loc: Location.none,
//                                             pexp_attributes: [],
//                                           },
//                                         ),
//                                       ],
//                                     ),
//                                   pexp_loc: Location.none,
//                                   pexp_attributes: [],
//                                 },
//                                 pvb_attributes: [],
//                                 pvb_loc: Location.none,
//                               },
//                             ],
//                             mapChildren(body),
//                           ),
//                       },
//                     ),
//                 },
//               ),
//           };
//         } else {
//           item;
//         }
//       | {pexp_desc: Pexp_fun(label, defaultValue, pattern, body)} as item => {
//           ...item,
//           pexp_desc:
//             Pexp_fun(label, defaultValue, pattern, mapChildren(body)),
//         }
//       | {pexp_desc: Pexp_let(recFlag, valueBindings, next)} as item => {
//           ...item,
//           pexp_desc: Pexp_let(recFlag, valueBindings, mapChildren(next)),
//         }
//       | _ as expression => expression
//       };
//     let body = mapChildren(body);
//     {
//       ...letBinding,
//       pstr_desc:
//         Pstr_value(
//           recFlag,
//           [
//             {
//               ...value,
//               pvb_expr: body,
//               pvb_attributes:
//                 foundReturnComponent^ ?
//                   [
//                     ({txt: "react.component", loc: Location.none}, PStr([])),
//                     ...value.pvb_attributes,
//                   ] :
//                   value.pvb_attributes,
//             },
//           ],
//         ),
//     };
//   | _ => default_mapper.structure_item(mapper, item)
//   };
// let implementationRefactorMapper = {
//   ...default_mapper,
//   structure_item: implementionMapStructureItem(TopLevel),
// };
// let rec interfaceMapSignatureItem = (key, mapper, item) =>
//   switch (item) {
//   | {
//       psig_desc:
//         Psig_module(
//           {
//             pmd_name: {txt: moduleName},
//             pmd_type:
//               {
//                 pmty_desc:
//                   Pmty_functor(
//                     name,
//                     moduleTypeDef,
//                     {pmty_desc: Pmty_signature(signatures)} as moduleType,
//                   ),
//               } as functorType,
//           } as moduleDef,
//         ),
//     } => {
//       ...item,
//       psig_desc:
//         Psig_module({
//           ...moduleDef,
//           pmd_type: {
//             ...functorType,
//             pmty_desc:
//               Pmty_functor(
//                 name,
//                 moduleTypeDef,
//                 {
//                   ...moduleType,
//                   pmty_desc:
//                     Pmty_signature(
//                       signatures
//                       |> List.map(signature =>
//                            interfaceMapSignatureItem(
//                              switch (key) {
//                              | TopLevel => Nested([|moduleName|])
//                              | Nested(keys) =>
//                                Nested(Array.concat([keys, [|moduleName|]]))
//                              },
//                              mapper,
//                              signature,
//                            )
//                          ),
//                     ),
//                 },
//               ),
//           },
//         }),
//     }
//   | {
//       psig_desc:
//         Psig_module(
//           {
//             pmd_name: {txt: moduleName},
//             pmd_type: {pmty_desc: Pmty_signature(signatures)} as moduleType,
//           } as moduleDef,
//         ),
//     } => {
//       ...item,
//       psig_desc:
//         Psig_module({
//           ...moduleDef,
//           pmd_type: {
//             ...moduleType,
//             pmty_desc:
//               Pmty_signature(
//                 signatures
//                 |> List.map(signature =>
//                      interfaceMapSignatureItem(
//                        switch (key) {
//                        | TopLevel => Nested([|moduleName|])
//                        | Nested(keys) =>
//                          Nested(Array.concat([keys, [|moduleName|]]))
//                        },
//                        mapper,
//                        signature,
//                      )
//                    ),
//               ),
//           },
//         }),
//     }
//   | {
//       psig_desc:
//         Psig_value(
//           {pval_name: {txt: "make"}, pval_type} as valueDescription,
//         ),
//     } as makeType =>
//     let rec returnsAComponent = ({ptyp_desc}) =>
//       switch (ptyp_desc) {
//       | Ptyp_constr(
//           {
//             txt:
//               Ldot(Lident("ReasonReact"), "componentSpec" | "component") |
//               Lapply(_) |
//               Lident(_),
//           },
//           _,
//         ) =>
//         true
//       | Ptyp_arrow(Nolabel, arg, coreType) => returnsAComponent(coreType)
//       | Ptyp_arrow(_, arg, coreType) => returnsAComponent(coreType)
//       | _ => false
//       };
//     let doesReturnAComponent = returnsAComponent(pval_type);
//     if (doesReturnAComponent) {
//       let rec replaceReturnedComponent = ({ptyp_desc} as t) =>
//         switch (ptyp_desc) {
//         | Ptyp_constr(
//             {
//               txt:
//                 Ldot(Lident("ReasonReact"), "componentSpec" | "component") |
//                 Lapply(_) |
//                 Lident(_),
//             },
//             _,
//           ) => {
//             ...t,
//             ptyp_desc:
//               Ptyp_constr(
//                 {loc: Location.none, txt: Ldot(Lident("React"), "element")},
//                 [],
//               ),
//           }
//         | Ptyp_arrow(Nolabel, arg, coreType) =>
//           switch (
//             try (Some(childrenUsageMap.contents |> StringArrayMap.find(key))) {
//             | _ => None
//             }
//           ) {
//           | Some(true) => {
//               ...t,
//               ptyp_desc:
//                 Ptyp_arrow(
//                   Labelled("children"),
//                   switch (arg) {
//                   | {
//                       ptyp_desc:
//                         Ptyp_constr(
//                           {txt: Lident("array")},
//                           [
//                             {
//                               ptyp_desc:
//                                 Ptyp_constr(
//                                   {
//                                     txt:
//                                       Ldot(
//                                         Lident("ReasonReact"),
//                                         "reactElement",
//                                       ),
//                                   },
//                                   [],
//                                 ),
//                             },
//                           ],
//                         ),
//                     } => {
//                       ...arg,
//                       ptyp_desc:
//                         Ptyp_constr(
//                           {txt: Lident("React.element"), loc: Location.none},
//                           [],
//                         ),
//                     }
//                   | _ => arg
//                   },
//                   {
//                     ...t,
//                     ptyp_desc:
//                       Ptyp_arrow(
//                         Nolabel,
//                         {
//                           ...arg,
//                           ptyp_desc:
//                             Ptyp_constr(
//                               {txt: Lident("unit"), loc: Location.none},
//                               [],
//                             ),
//                         },
//                         replaceReturnedComponent(coreType),
//                       ),
//                   },
//                 ),
//             }
//           | _ => {
//               ...t,
//               ptyp_desc:
//                 Ptyp_arrow(
//                   Nolabel,
//                   {
//                     ...arg,
//                     ptyp_desc:
//                       Ptyp_constr(
//                         {txt: Lident("unit"), loc: Location.none},
//                         [],
//                       ),
//                   },
//                   replaceReturnedComponent(coreType),
//                 ),
//             }
//           }
//         | Ptyp_arrow(label, arg, coreType) => {
//             ...t,
//             ptyp_desc:
//               Ptyp_arrow(label, arg, replaceReturnedComponent(coreType)),
//           }
//         | x => t
//         };
//       {
//         ...makeType,
//         psig_desc:
//           Psig_value({
//             ...valueDescription,
//             pval_attributes: [
//               ({txt: "react.component", loc: Location.none}, PStr([])),
//               ...valueDescription.pval_attributes,
//             ],
//             pval_type: replaceReturnedComponent(valueDescription.pval_type),
//           }),
//       };
//     } else {
//       item;
//     };
//   | _ => item
//   };
// let interfaceRefactorMapper = {
//   ...default_mapper,
//   signature_item: interfaceMapSignatureItem(TopLevel),
// };
// module StringSet = Set.Make(String);
// let read = () => {
//   let set = ref(StringSet.empty);
//   let rec read = () =>
//     try (
//       {
//         set := set^ |> StringSet.add(stdin |> input_line);
//         read();
//       }
//     ) {
//     | End_of_file => ()
//     };
//   read();
//   set^;
// };
// let transform = (args, fileName) =>
//   try (
//     {
//       let outputDir =
//         args |> Array.exists(item => item == "--demo") ? "output/" : "";
//       let file = fileName |> Filename.remove_extension;
//       let ic = open_in_bin(file ++ ".re");
//       let lexbuf = Lexing.from_channel(ic);
//       let (ast, comments) =
//         Reason_toolchain.RE.implementation_with_comments(lexbuf);
//       let newAst =
//         implementationRefactorMapper.structure(
//           implementationRefactorMapper,
//           ast,
//         );
//       let target = outputDir ++ file ++ ".re";
//       let oc = open_out_bin(target);
//       if (Sys.file_exists(file ++ ".rei")) {
//         let ic = open_in_bin(file ++ ".rei");
//         let lexbuf = Lexing.from_channel(ic);
//         let (ast, comments) =
//           Reason_toolchain.RE.interface_with_comments(lexbuf);
//         let newAst =
//           interfaceRefactorMapper.signature(interfaceRefactorMapper, ast);
//         let target = outputDir ++ file ++ ".rei";
//         let oc = open_out_bin(target);
//         let formatter = Format.formatter_of_out_channel(oc);
//         Reason_toolchain.RE.print_interface_with_comments(
//           formatter,
//           (newAst, comments),
//         );
//         Format.print_flush();
//         Console.log(
//           Pastel.(
//             <Pastel>
//               logPrefix
//               <Pastel backgroundColor=Green color=Black> " Done " </Pastel>
//               " "
//               target
//             </Pastel>
//           ),
//         );
//         close_out(oc);
//       };
//       let formatter = Format.formatter_of_out_channel(oc);
//       Reason_toolchain.RE.print_implementation_with_comments(
//         formatter,
//         (newAst, comments),
//       );
//       Format.print_flush();
//       Console.log(
//         <Pastel>
//           logPrefix
//           <Pastel backgroundColor=Green color=Black> " Done " </Pastel>
//           " "
//           target
//         </Pastel>,
//       );
//       close_out(oc);
//     }
//   ) {
//   | error =>
//     let outputDir =
//       args |> Array.exists(item => item == "--demo") ? "output/" : "";
//     let file = fileName |> Filename.remove_extension;
//     let target = outputDir ++ file ++ ".re";
//     Console.log(
//       <Pastel>
//         logPrefix
//         <Pastel backgroundColor=Red color=White> " Error " </Pastel>
//         " "
//         target
//       </Pastel>,
//     );
//     let err = Printexc.to_string(error);
//     Console.log(err);
//   };
// let main = () => {
//   switch (Sys.argv) {
//   | [||]
//   | [|"help" | "-help" | "--help"|] =>
//     Console.log("upgrade-reason-react");
//     Console.log("Helps you migrate ReasonReact from 0.6 to 0.7");
//     Console.log("Usage: find src/**/*.re | Upgrade");
//     Console.log("Usage: pass a list of .re files you'd like to convert.");
//   | args =>
//     read()
//     |> StringSet.filter(item => Filename.extension(item) == ".re")
//     /* Uncomment next line for debug */
//     /* && ! String.contains(item, '_') */
//     |> StringSet.iter(transform(args));
//     Console.log("Done!");
//   };
// };
