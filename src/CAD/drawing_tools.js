function UnSelectAll()
{
	for (var x = 0; x < render.Stack.length; x++)
	{
		var part = render.Stack[x];
		for (var i = 0; i < part.entities.length; i++)
		{
			if (part.entities[i].meta.selected == true)
			{
				part.entities[i].meta.selected = false;
				render.updatePart(part);
			}
		}
	}
}
function SelectAll()
{
	for (var x = 0; x < render.Stack.length; x++)
	{
		var part = render.Stack[x];
		for (var i = 0; i < part.entities.length; i++)
		{
			if (part.entities[i].meta.selected == false)
			{
				part.entities[i].meta.selected = true;
				render.updatePart(part);
			}
		}
	}
}
function showSnapPointer()
{
  var part = render.newLivePart("snap_pointer");
  var e = {
    type: "circle",
    origin: [0, 0],
    radius: render._mouseOverPadding,
    meta: render.copy_obj(render._liveMeta)
  }
  e.meta.on_mouse_motion = function(e){
    e.origin = [render.snapPosition.x, render.snapPosition.y];
    var z_factor = render.camera.position.z * 0.1;
    e.radius = (z_factor * render._mouseOverPadding) / 2;
  };
  e.meta.on_mouse_click = function(e){
    e.meta.on_mouse_motion = null;
    var new_entity = render.copy_obj(e);
    //console.log("Creating line: " + JSON.stringify(new_entity));
    render.removePartFromScene("snap_pointer");
    return true; //true - Delete the callback after it returns
  };
  part.entities.push(e);
  render.Stack.push(part);
}
function hideSnapPointer()
{
	render.removePartFromStack("snap_pointer");
}
function Line()
{
	showSnapPointer();
  var part = render.newLivePart("line_tool");
  var e = {
    type: "line",
    origin: [0, 0],
    end: [0, 0],
    meta: render.copy_obj(render._liveMeta)
  }
  e.meta.on_mouse_motion = function(e){
    e.end = [render.snapPosition.x, render.snapPosition.y];
  };
  e.meta.on_mouse_click = function(e){
    e.meta.on_mouse_motion = null;
    var new_entity = render.copy_obj(e);
    //console.log("Creating line: " + JSON.stringify(new_entity));
    render.removePartFromScene("line_tool");
		hideSnapPointer();
    return true; //true - Delete the callback after it returns
  };
  part.entities.push(e);
  render.Stack.push(part);
}
