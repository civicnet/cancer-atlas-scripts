type latlng = {
  lat: float,
  lng: float,
};
type bbox = {
  northeast: latlng,
  southwest: latlng,
};
type geometry = {
  location: latlng,
  location_type: string,
  viewport: bbox,
};
type address_component = {
  long_name: string,
  short_name: string,
  types: list(string),
};
type plus_code = {
  compound_code: string,
  global_code: string,
};
type geocode_result = {
  address_components: list(address_component),
  formatted_address: string,
  geometry,
  place_id: option(string),
  plus_code: option(plus_code),
  types: list(string),
};
type geocode_results = {
  results: list(geocode_result),
  status: string,
};

module Decode = {
  let latlng = json =>
    Json.Decode.{
      lat: json |> field("lat", float),
      lng: json |> field("lng", float),
    };
  let bbox = json =>
    Json.Decode.{
      northeast: json |> field("northeast", latlng),
      southwest: json |> field("southwest", latlng),
    };
  let geometry = json =>
    Json.Decode.{
      location: json |> field("location", latlng),
      location_type: json |> field("location_type", string),
      viewport: json |> field("viewport", bbox),
    };
  let address_component = json =>
    Json.Decode.{
      long_name: json |> field("long_name", string),
      short_name: json |> field("short_name", string),
      types: json |> field("types", list(string)),
    };
  let plus_code = json =>
    Json.Decode.{
      compound_code: json |> field("compound_code", string),
      global_code: json |> field("global_code", string),
    };
  let geocode_result = json =>
    Json.Decode.{
      address_components:
        json |> field("address_components", list(address_component)),
      formatted_address: json |> field("formatted_address", string),
      geometry: json |> field("geometry", geometry),
      place_id: json |> optional(field("place_id", string)),
      plus_code: json |> optional(field("plus_code", plus_code)),
      types: json |> field("types", list(string)),
    };
  let geocode_results = json =>
    Json.Decode.{
      results: json |> field("results", list(geocode_result)),
      status: json |> field("status", string),
    };
};