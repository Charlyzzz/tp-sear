defmodule SearWeb.PageController do
  use SearWeb, :controller

  def index(conn, _params) do
    render(conn, "index.html")
  end
end
