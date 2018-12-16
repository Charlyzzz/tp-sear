defmodule Sear.Commander do
  use GenServer

  alias Sear.Location

  def start_link(_) do
    GenServer.start_link(__MODULE__, :ok, name: __MODULE__)
  end

  def start_link, do: GenServer.start_link(__MODULE__, :ok)

  @impl true
  def init(_) do
    {:ok, {{4,4}, true}}
  end

  @impl true
  def handle_call({:snap, new_photo_location}, _from, _old_photo_location) do
    {:reply, 0, {new_photo_location, false}}
  end  

  @impl true
  def handle_call({:next_movement, location}, _from, {location, false}) do
    {:reply, "@snap", {location, true}}
  end  

  @impl true
  def handle_call({:next_movement, camera_location}, _from, {photo_location, false} = state) do
    {:reply, Location.move_closer(photo_location, camera_location), state}
  end  

  @impl true
  def handle_call({:next_movement, _}, _from, {_, true} = state) do
    {:reply, Location.nop, state}
  end  

  def snap_photo(position) do
    GenServer.call(__MODULE__, {:snap, position})
  end

  def next_movement(current_position) do
    GenServer.call(__MODULE__, {:next_movement, current_position})
  end
end
