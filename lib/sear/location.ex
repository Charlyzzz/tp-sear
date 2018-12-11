defmodule Sear.Location do

  @nop "@nop"
  def nop, do: @nop

  @up "@up"
  def up, do: @up

  @down "@down"
  def down, do: @down

  @left "@left"
  def left, do: @left

  @right "@right"
  def right, do: @right

  def move_closer(destination, destination), do:  @nop
  def move_closer({photo_x, _}, {camera_x, _}) when camera_x > photo_x, do: @left
  def move_closer({photo_x, _}, {camera_x, _}) when camera_x < photo_x, do: @right
  def move_closer({_, photo_y}, {_, camera_y}) when camera_y > photo_y, do: @down
  def move_closer({_, photo_y}, {_, camera_y}) when camera_y < photo_y, do: @up
end
