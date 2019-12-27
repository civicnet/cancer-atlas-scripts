Dotenv.config();
[%raw "require('isomorphic-fetch')"];

exception FileError(string);
let fileError = msg => raise(FileError(msg));

let read_path =
  switch ([%bs.node __dirname]) {
  | Some(dirname) =>
    Node.Path.resolve(
      dirname,
      "../data/json/mf_caras_severin.json",
    )
  | None => fileError("Dirname missing")
  };

let write_path =
  switch ([%bs.node __dirname]) {
  | Some(dirname) =>
    Node.Path.resolve(
      dirname,
      "../data/json/mf_caras_severin_with_loc.json",
    )
  | None => fileError("Dirname missing")
  };

type medical_service_with_location = {
  supplier: FamilyMedic.supplier,
  location: GoogleGeocodeResult.geocode_results,
};

let data = Node.Fs.readFileSync(read_path, `utf8);
let suppliers = data |> Json.parseOrRaise |> FamilyMedic.Decode.suppliers;

let decodeGeometry = data =>
  data |> Json.parseOrRaise |> GoogleGeocodeResult.Decode.geocode_results;

let env = Node.Process.process##env;
let key = Js.Dict.get(env, "GOOGLE_API_KEY");

let geocode = address =>
  Js.Promise.(
    Fetch.fetch(
      {j|https://maps.googleapis.com/maps/api/geocode/json?address=$address+Bucuresti&key=$key|j},
    )
    |> then_(Fetch.Response.text)
    |> then_(text => decodeGeometry(text) |> resolve)
  );

[@bs.val] external encodeURIComponent: string => string = "encodeURIComponent";
let rec suppliersWithAddress = (supps: list(FamilyMedic.supplier), acc) =>
  switch (supps) {
  | [hd, ...tl] =>
    geocode(encodeURIComponent(hd.address))
    |> Js.Promise.then_(value => {
         let name = hd.medicName;
         let parsedCount = List.length(acc);
         let totalCount = List.length(supps);
         Js.log({j| $parsedCount / $totalCount: Looking at $name |j});

         let supp = {supplier: hd, location: value};
         suppliersWithAddress(tl, List.append(acc, [supp]));
       })
    |> Js.Promise.catch(err => {
         let name = hd.medicName;
         let address = hd.address;
         Js.log2({j|Geocoding fail for $name @ $address!|j}, err);
         suppliersWithAddress(tl, acc);
       })
  | [] => Js.Promise.make((~resolve, ~reject) => resolve(. acc))
  };

suppliersWithAddress(suppliers.list, [])
|> Js.Promise.then_(value => {
     Js.log2("Length of list: ", List.length(value));
     let st = Js.Json.stringifyAny(Array.of_list(value));
     switch (st) {
     | Some(json) => Node.Fs.writeFileSync(write_path, json, `utf8)
     | _ => fileError("Can't write json out")
     };
     Js.Promise.resolve();
   });