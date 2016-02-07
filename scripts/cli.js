Input = "";
Prompt = "";
Callback = "";
function isLetter(str) {
  return str.length === 1 && str.match(/[a-z]/i);
}
function isNumber(str) {
  return str.length === 1 && str.match(/[0-9]/i);
}

function cliPush(mod, c)
{
  if (Callback == "")
  {
    Prompt = "> ";
  }
  var letter = String.fromCharCode(c);
  if (c == 8) //Backspace
  {
    Input = Input.slice(0, -1);
    SetStatusText(Prompt + Input);
  }
  else if (c == 13)
  {
    var e = Input;
    Input = "";
    SetStatusText("");
    cliReturn(e);
  }
  if (isLetter(letter) || isNumber(letter) || c == 40 || c == 41 || c == 59 || c == 39 || c == 91 || c == 93)
  {
    if (mod == "None")
    {
      Input += letter.toLowerCase();
    }
    else if (mod == "Shift")
    {
      Input += letter;
    }

    cliPrompt();
  }
}
function cliPrompt(p, c)
{
  if (p != undefined)
  {
    Prompt = p;
  }
  if (c != undefined)
  {
    Callback = c;
  }
  SetStatusText(Prompt + Input);
}
function cliReturn(str)
{
  if (Callback != "")
  {
    Callback(str);
    Callback = "";
    Prompt = "";
    Input = "";
  }
  else
  {
      cliCommands(str);
  }
}
function cliCommands(str)
{
  if (str == "clvo")
  {
    DrawLineVerticalFromOrigin();
  }
  if (str == "clho")
  {
    DrawLineHorizontalFromOrigin();
  }
}
