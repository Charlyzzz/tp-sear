defmodule Sear.Commander do
  use GenServer

  alias Sear.Location

  def start_link(_) do
    GenServer.start_link(__MODULE__, :ok, name: __MODULE__)
  end

  @impl true
  def init(_) do
    {:ok, {0,0}}
  end

  @impl true
  def handle_call({:snap, new_photo_location}, _from, _old_photo_location) do
    {:reply, 0, new_photo_location}
  end  

  @impl true
  def handle_call({:next_movement, camera_location}, _from, photo_location) do
    {:reply, Location.move_closer(photo_location, camera_location), photo_location}
  end  

  def snap_photo(position) do
    GenServer.call(__MODULE__, {:snap, position})
  end

  def next_movement(current_position) do
    GenServer.call(__MODULE__, {:next_movement, current_position})
  end
end
