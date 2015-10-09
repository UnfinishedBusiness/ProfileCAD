function Xinc(i)
  print("Lua => Xinc => " .. i .. "\n");
  return "G20 G91 G0 X" .. i .. "\nG90";
end
function Yinc(i)
  print("Lua => Yinc => " .. i .. "\n");
  return "G20 G91 G0 Y" .. i .. "\nG90";
end
function SetXYZ()
  return "G10 L2 P1 X0 Y0 Z0";
end
function Loading()
  return "G20 G53 G20 G0 X2.5Y2.5 Z1";
end
function Home()
  return "G28.2 X0Y0";
end
function Pause()
  return "M30";
end
function Clear()
  return "$clear";
end
