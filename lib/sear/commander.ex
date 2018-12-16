defmodule Sear.Commander do
  use GenServer

  alias Sear.Commander

  def start_link(estado, opts \\ []) do
    GenServer.start_link(__MODULE__, estado, opts)
  end

  def child_spec(estado, opts) do
    %{
      id: Commander,
      start: {Commander, :start_link, [estado, opts]}
    }
  end

  #CALLBACKS

  @impl true
  def init(estado) do
    {:ok, estado}
  end

  @impl true
  def handle_cast({:push, new_photo_location}, photos) do
    {:noreply, photos ++ [new_photo_location]}
  end  

  @impl true
  def handle_cast(:pop, [_ | rest]) do
    {:noreply, rest}
  end  

  @impl true
  def handle_cast(:pop, [] = state) do
    {:noreply, state}
  end   

  @impl true
  def handle_call(:peek, _from, [next_photo | _] = fotos) do
    {:reply, next_photo, fotos}
  end  

  @impl true
  def handle_call(:peek, _from, [] = state) do
    {:reply, "nop", state}
  end  

  #API

  def push(commander, foto) do
    GenServer.cast(commander, {:push, foto})
  end

  def pop(commander) do
    GenServer.cast(commander, :pop)
  end

  def peek(commander) do
    GenServer.call(commander, :peek)
  end
end
