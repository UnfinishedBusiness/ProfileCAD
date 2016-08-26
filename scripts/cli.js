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
  var KeypadLookupTable = { 390: '-', 391: '.', 388: '+', 324: '0', 325: '1', 326: '2', 327: '3', 328: '4', 329: '5', 330: '6', 331: '7', 332: '8', 333: '9' };
  var IsKeypad = false;
  var letter;
  if (KeypadLookupTable[c] != undefined)
  {
    letter = KeypadLookupTable[c];
    IsKeypad = true;
  }
  else
  {
    letter = String.fromCharCode(c);
  }
  if (c == 8) //Backspace
  {
    Input = Input.slice(0, -1);
    SetStatusText(Prompt + Input);
  }
  else if (c == 13 || c == 370)
  {
    var e = Input;
    Input = "";
    SetStatusText("");
    cliReturn(e);
  }
  if (isLetter(letter) || isNumber(letter) || IsKeypad || c == 61 || c == 45 || c == 46 || c == 47 || c == 40 || c == 41 || c == 59 || c == 39 || c == 91 || c == 93)
  {
    if (mod == "None")
    {
      Input += letter.toLowerCase();
    }
    else if (mod == "Shift" || c == 61) // + keyPressed
    {
      Input += '+';
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
  SetStatusText(Prompt + " " + Input);
}
function cliReturn(str)
{
  if (Callback != "")
  {
    Callback(str);
    Callback = "";
    Prompt = "";
    Input = "";
    SetStatusText("");
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
