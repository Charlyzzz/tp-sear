defmodule SearWeb.CameraController do
  use SearWeb, :controller

  alias Sear.{Commander, Photos}
  alias SearWeb.Endpoint

  def command(conn, _params) do
    movement = case Commander.peek(Commander) do
      {x, y} -> "#{x}#{y}"
      comando -> comando
    end
    send_resp(conn, :ok, "@" <> movement)
  end

  def request_photo(conn, %{"x" => x, "y" => y}) do
    Commander.push(Commander, {x, y})
    send_resp(conn, :created, "")
  end

  def photo_snapped(conn, %{"filename" => filename}) do
    Commander.pop(Commander)
    new_photo = Photos.new(filename)
    Photos.store(new_photo)
    Endpoint.broadcast!("photos", "new_photo", new_photo)
    send_resp(conn, :ok, "")
  end

  def all_photos(conn, _) do
    json(conn, %{ photos: Photos.all() })
  end

  def ping(conn, _params) do
    send_resp(conn, :ok, "pong")
  end
end
