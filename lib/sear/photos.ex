defmodule Sear.Photos do

  @table_name :photos
  @key_name :photos
  
  def init, do:  :ets.new(@table_name, [:duplicate_bag, :public, :named_table])

  def new(filename), do: %{filename: filename, moment: DateTime.utc_now}

  def store(photo) do
     :ets.insert(@table_name, {@key_name, photo})
  end

  def all, do: :ets.lookup(@table_name, @key_name) |> Keyword.values
end
