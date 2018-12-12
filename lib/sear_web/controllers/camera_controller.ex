defmodule SearWeb.CameraController do
  use SearWeb, :controller

  alias Sear.{Commander, Photos}
  alias SearWeb.Endpoint

  def command(conn, _params) do
    %{"x" => x, "y" => y} = conn.query_params
    current_position = {String.to_integer(x), String.to_integer(y)}
    movement = Commander.next_movement(current_position)
    send_resp(conn, :ok, movement)
  end

  def request_photo(conn, %{"x" => x, "y" => y}) do
    Commander.snap_photo({x, y})
    send_resp(conn, :created, "")
  end

  def photo_snapped(conn, %{"filename" => new_photo}) do
    Photos.store(new_photo)
    Endpoint.broadcast!("photos", "new_photo", %{filename: new_photo})
    send_resp(conn, :ok, "")
  end

  def all_photos(conn, _) do
    json(conn, %{ photos: Photos.all() })
  end


  def ping(conn, _params) do
    send_resp(conn, :ok, "pong")
  end
end
