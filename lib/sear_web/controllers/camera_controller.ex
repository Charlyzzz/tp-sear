defmodule SearWeb.CameraController do
  use SearWeb, :controller

  alias Sear.Commander

  def command(conn, _params) do
    %{"x" => x, "y" => y} = conn.query_params
    current_position = {String.to_integer(x), String.to_integer(y)}
    movement = Commander.next_movement(current_position)
    send_resp(conn, :ok, movement)
  end

  def ping(conn, _params) do
    send_resp(conn, :ok, "pong")
  end
end
