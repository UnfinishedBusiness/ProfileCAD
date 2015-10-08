HOME = "X0Y0Z1"
BLOCKPLUS = 10
CURRENTBLOCK = 0

function N()
    CURRENTBLOCK = CURRENTBLOCK + BLOCKPLUS
    if CURRENTBLOCK == BLOCKPLUS
    then
      return "N" .. CURRENTBLOCK;
    else
      return "\nN" .. CURRENTBLOCK;
    end
end
function End()
  return Home() .. N() .. "M30";
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
function Contour(plunge_feed, cross_feed, retract_feed, path)
  GCODE = "";
  for key,value in pairs(path)
  do
     -- print(key,value)
     GCODE = GCODE .. N() .. "G1" .. value .. "F" .. cross_feed
  end
  return GCODE
end
