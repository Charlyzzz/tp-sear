defmodule SearWeb.Router do
  use SearWeb, :router

  pipeline :browser do
    plug :accepts, ["html"]
    plug :fetch_session
    plug :fetch_flash
    plug :protect_from_forgery
    plug :put_secure_browser_headers
  end

  pipeline :api do
    plug :accepts, ["json"]
  end

  scope "/", SearWeb do
    pipe_through :browser

    get "/", PageController, :index
  end

  scope "/api", SearWeb do
    pipe_through :api

    get "/command", CameraController, :command
    put "/photo", CameraController, :photo_snapped
    post "/photo", CameraController, :request_photo
    get "/photos", CameraController, :all_photos
    
    get "/ping", CameraController, :ping
  end
end
