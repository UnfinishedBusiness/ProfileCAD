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

function Contour(coolant, spindle_speed, plunge_feed, cross_feed, retract_feed, path)
  CYCLE_COUNT = CYCLE_COUNT + 1
  GCODE = N() .. " (Begin Cycle " .. CYCLE_COUNT .. ")" .. N() .. "M3S" .. spindle_speed;
  for key,value in pairs(path)
  do
     -- print(key,value)
     if string.match(value, "[LINE]")
     then
        Table = string.split(value, "]");
        GCODE = GCODE .. N() .. "G1" .. Table[2] .. "F" .. cross_feed
     end
     if string.match(value, "[ARC CW]")
     then
        Table = string.split(value, "]");
        GCODE = GCODE .. N() .. "G2" .. Table[2] .. "F" .. cross_feed
     end
     if string.match(value, "[ARC CCW]")
     then
        Table = string.split(value, "]");
        GCODE = GCODE .. N() .. "G3" .. Table[2] .. "F" .. cross_feed
     end

  end
  return GCODE
end
