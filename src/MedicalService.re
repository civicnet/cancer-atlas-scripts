type suppliers = {list: list(supplier)}
and supplier = {
  name: string,
  specialty: string,
  address: string,
  phone: string,
};

module Decode = {
  let supplier = json =>
    Json.Decode.{
      name: json |> field("name", string),
      specialty: json |> field("specialty", string),
      address: json |> field("address", string),
      phone: json |> field("phone", string),
    };
  let suppliers = json =>
    Json.Decode.{list: json |> field("list", list(supplier))};
};