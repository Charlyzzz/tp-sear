defmodule SearWeb.CameraControllerTest do
  use SearWeb.ConnCase

  test "GET /ping", %{conn: conn} do
    conn = get(conn, "/ping")
    assert response(conn, 200) =~ "pong"
  end
end
