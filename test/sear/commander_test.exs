defmodule Sear.CommanderTest do
  use ExUnit.Case, async: true
  alias Sear.Commander

  describe "peek" do

    test "cuando tiene fotos encoladas nos devuelve la posicion de la proxima" do
      {:ok, commander} = Commander.start_link([{0,0}])
      assert {0, 0} == Commander.peek(commander)
      assert {0, 0} == Commander.peek(commander)
    end  

    test "devuelve nop cuando no tiene fotos encoladas" do
      {:ok, commander} = Commander.start_link([])
      assert "nop" == Commander.peek(commander)
    end
  end

  test "push agrega una foto" do
    {:ok, commander} = Commander.start_link([])
    Commander.push(commander, {1,1})
    assert {1,1} == Commander.peek(commander)
  end

  test "pop quita la proxima foto" do
    {:ok, commander} = Commander.start_link([{0, 0}, {1, 1}])
    assert {0, 0} == Commander.peek(commander)
    Commander.pop(commander)
    assert {1, 1} == Commander.peek(commander)
  end
end
