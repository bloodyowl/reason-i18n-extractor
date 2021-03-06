# reason-i18n-extractor

[![Build Status](https://dev.azure.com/mlbli/reason-i18n-extractor/_apis/build/status/bloodyowl.reason-i18n-extractor?branchName=master)](https://dev.azure.com/mlbli/reason-i18n-extractor/_build/latest?definitionId=5&branchName=master)

Extract translations from your ReasonML codebase.

## Installation

```console
$ yarn add reason-i18n-extractor
```

## Usage

### Using translations in your codebase

This binary assumes that your translations are using a `T.__` function to identify translations.

```reason
let element = <div> {T.__("My translation")->React.string} </div>;
```

This pairs nicely with [`i18n-webpack-plugin`](https://github.com/webpack-contrib/i18n-webpack-plugin) and the following external:

```reason
// T.re
[@bs.val] external __: string => string = "__";
```

### Binary

You feed the list of files you have translations in to the script's sdtin, and pass as first argument the destination of the JSON file you want to store the translations in.

```console
$ find ./src -iname '*.re' | reason-i18n-extractor ./i18n/fr.json
```

Let's say your codebase contains the following code:

```reason
let x = <div> {T.__("My translation")->React.string} </div>;
let y = <div> {T.__("My translation 2")->React.string} </div>;
```

If the translation file doesn't exist, it'll write:

```json
{
  "My translation": "My translation",
  "My translation 2": "My translation 2"
}
```

If the translation file does exist and already contains:

If the translation file doesn't exist, it'll write:

```json
{
  "My translation": "Ma traduction",
}
```

The script will merge the existing with the new keys and write:

```json
{
  "My translation": "Ma traduction",
  "My translation 2": "My translation 2"
}
```

Then, you can check the diff and write the missing translations.

```diff
 {
   "My translation": "Ma traduction",
+  "My translation 2": "My translation 2"
 }
```
