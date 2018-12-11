defmodule SearWeb.Router do
  use SearWeb, :router

  pipeline :api do
    plug :accepts, ["json"]
  end

  scope "/", SearWeb do
    pipe_through :api

    get "/command", CameraController, :command
    get "/ping", CameraController, :ping
    post "/photo", CameraController, :request_photo
  end

  # Other scopes may use custom stacks.
  # scope "/api", SearWeb do
  #   pipe_through :api
  # end
end
