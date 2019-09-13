let getEnvOpt = s =>
  try (Some(Sys.getenv(s))) {
  | Not_found => None
  };

let get = () => {
  switch (getEnvOpt("REASON_NATIVE_ROOT")) {
  | Some(dir) => dir
  | None =>
    failwith(
      "Expected `REASON_NATIVE_ROOT` environment variable to be set "
      ++ "before running tests.",
    )
  };
};
