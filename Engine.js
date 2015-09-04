canvas = "";
ctx = "";
function main()
{
  if (canvas.getContext){
    // use getContext to use the canvas for drawing

  }
}
function G0(x, y)
{
  canvas = document.getElementById('Canvas')
  ctx = canvas.getContext('2d');
  ctx.moveTo(x, y);
}
function G1(x, y)
{
  canvas = document.getElementById('Canvas')
  ctx = canvas.getContext('2d');
  ctx.lineTo(x, y);
}
function path()
{
  canvas = document.getElementById('Canvas')
  ctx = canvas.getContext('2d');
  ctx.beginPath();
}
function draw()
{
  canvas = document.getElementById('Canvas')
  ctx = canvas.getContext('2d');
  ctx.closePath();
  ctx.stroke();
}
