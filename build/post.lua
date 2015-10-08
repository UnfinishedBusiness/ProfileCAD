HOME = "X0Y0Z1"
BLOCKPLUS = 10
CURRENTBLOCK = 0
CYCLE_COUNT = 0
function N()
    CURRENTBLOCK = CURRENTBLOCK + BLOCKPLUS
    if CURRENTBLOCK == BLOCKPLUS
    then
      return "N" .. CURRENTBLOCK;
    else
      return "\nN" .. CURRENTBLOCK;
    end
end

function Header()
  return N() .. "G20G54" .. " (Header)";
end

function Footer()
  return Home() .. N() .. "M30" .. " (End Program)";
end

function Home()
  return N() .. "G0" .. HOME;
end

function Stop()
  return N() .. "M0";
end

function Toolchange(toolnum)
    return Home() .. N() .. "M6T" .. toolnum .. Stop();
end

function string:split(delimiter)
  local result = { }
  local from  = 1
  local delim_from, delim_to = string.find( self, delimiter, from  )
  while delim_from do
    table.insert( result, string.sub( self, from , delim_from-1 ) )
    from  = delim_to + 1
    delim_from, delim_to = string.find( self, delimiter, from  )
  end
  table.insert( result, string.sub( self, from  ) )
  return result
end

function tprint (tbl, indent)
  if not indent then indent = 0 end
  for k, v in pairs(tbl) do
    formatting = string.rep("  ", indent) .. k .. ": "
    if type(v) == "table" then
      print(formatting)
      tprint(v, indent+1)
    elseif type(v) == 'boolean' then
      print(formatting .. tostring(v))
    else
      print(formatting .. v)
    end
  end
end

function Contour(coolant, spindle_speed, start_z, plunge_feed, cross_feed, retract_feed, path)
  CYCLE_COUNT = CYCLE_COUNT + 1
  GCODE = N() .. " (Begin Cycle " .. CYCLE_COUNT .. ")" .. N() .. "M3S" .. spindle_speed;
  POINT_COUNT = 0
  end_table = {};
  --print("\n");
  --tprint(path)
  for key, value in pairs(path)
  do
    if POINT_COUNT == 0
    then
        GCODE = GCODE .. N() .. "G0X" .. value.X .. "Y" .. value.Y .. "Z" .. (value.Z + start_z);
        GCODE = GCODE .. N() .. "G1X" .. value.X .. "Y" .. value.Y .. "Z" .. value.Z .. "F" .. plunge_feed;
    else
        if value.Type == "Line"
        then
          GCODE = GCODE .. N() .. "G1X" .. value.X .. "Y" .. value.Y .. "Z" .. value.Z .. "F" .. cross_feed;
        end
        if value.Type == "Arc"
        then
            if value.Direction == "CW"
            then
                GCODE = GCODE .. N() .. "G2X" .. value.X .. "Y" .. value.Y .. "Z" .. value.Z .. "R" .. value.R .. "F" .. cross_feed;
            else
                GCODE = GCODE .. N() .. "G3X" .. value.X .. "Y" .. value.Y .. "Z" .. value.Z .. "R" .. value.R .. "F" .. cross_feed;
            end
        end
    end
    end_table = value;
    POINT_COUNT = POINT_COUNT + 1;
  end
  GCODE = GCODE .. N() .. "G1X" .. end_table.X .. "Y" .. end_table.Y .. "Z" .. (end_table.Z + start_z) .. "F" .. retract_feed;
  return GCODE
end
