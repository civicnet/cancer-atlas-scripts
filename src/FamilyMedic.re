type suppliers = {list: list(supplier)}
and supplier = {
  contractNo: string,
  supplierName: string,
  medicName: string,
  address: string,
  phone: string,
  email: string,
};

module Decode = {
  let supplier = json =>
    Json.Decode.{
      contractNo: json |> field("contractNo", string),
      supplierName: json |> field("supplierName", string),
      medicName: json |> field("medicName", string),
      address: json |> field("address", string),
      phone: json |> field("phone", string),
      email: json |> field("email", string),
    };
  let suppliers = json =>
    Json.Decode.{list: json |> field("list", list(supplier))};
};