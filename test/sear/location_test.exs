defmodule Sear.LocationTest do
  use ExUnit.Case, async: true
  
  alias Sear.Location

  @destination {4, 4}

  describe "move_closer" do
    
    test "se mueve a la derecha porque X es mas bajo" do
      assert Location.right == Location.move_closer(@destination, {3, 4})
    end
    
    test "se mueve a la izquierda porque X es mas alto" do
      assert Location.left == Location.move_closer(@destination, {5, 4})
    end
    
    test "se mueve arriba porque Y es mas bajo" do
      assert Location.up == Location.move_closer(@destination, {4, 3})
    end
    
    test "se mueve abajo porque Y es mas alto" do
      assert Location.down == Location.move_closer(@destination, {4, 5})
    end
    
    test "no se mueve porque es la misma ubicacion" do
      assert Location.nop == Location.move_closer(@destination, @destination)
    end

    test "primero se mueve en el eje x" do
      assert Location.left == Location.move_closer(@destination, {5, 5})
    end
  end
end
