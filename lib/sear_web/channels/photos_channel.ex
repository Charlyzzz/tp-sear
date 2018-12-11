defmodule SearWeb.PhotosChannel do
  use Phoenix.Channel

  def join("photos", _message, socket) do
    {:ok, socket}
  end
end
