defmodule Sear.CommanderTest do
  use ExUnit.Case, async: false
  
  alias Sear.{Commander, Location}
    
  test "delega en Location.move_closer/2 si no saco la foto" do
    Commander.snap_photo({0, 0})
    assert Location.left == Commander.next_movement({1, 0})
  end

  test "saca la foto si coincide la posicion y no fué tomada todavía" do
    Commander.snap_photo({0, 0})
    assert "@snap" == Commander.next_movement({0,0})
    assert Location.nop == Commander.next_movement({0,0})
  end
end
