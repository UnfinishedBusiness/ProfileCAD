var snap_pointer;
var active_part = "test"
function DeleteSelected()
{
	printf("Deleting selected entities!\n\r");
	var new_stack = [];
	for (var x = 0; x < render.Stack.length; x++)
	{
		var part = render.copy_obj(render.Stack[x]);
		var entity_stack = [];
		for (var i = 0; i < part.entities.length; i++)
		{
			var entity = part.entities[i];
			if (entity.meta.selected == false)
			{
				entity_stack.push(entity);
			}
		}
		part.entities = entity_stack;
		part.updateRender = true;
		new_stack.push(render.copy_obj(part));
	}
	render.Stack = new_stack;
	ret();
}
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
  snap_pointer = render.newLivePart("snap_pointer");
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
  snap_pointer.entities.push(e);
  render.Stack.push(snap_pointer);
}
function hideSnapPointer()
{
	render.removePartFromStack("snap_pointer");
}
function DrawLine()
{
	printf("Drawing line!\n\r-> Click start point of line!\n\r");
	showSnapPointer();
  var part = render.newLivePart("line_tool");
  var e = {
    type: "line",
    origin: [0, 0],
    end: [0, 0],
    meta: render.copy_obj(render._liveMeta)
  }
	showSnapPointer();
	e.meta.on_mouse_click = function(e){
		printf("-> Click end point of line!\n\r");
		on_tab_key = function(){
			save_terminal_focus();
			hideSnapPointer(); //Hide snap pointer because we are using manual input
			e.meta.on_mouse_motion = null; //cancel mouse motion callback so not to overide manual input
			printf("\r\nLength? ");
			getline(function(length){
				e.tool = {}; //Create a tmp varariable in the entity to store "global" data for this tool
				e.tool.length  = parseFloat(length);
				printf("Angle? ");
				getline(function(angle){
					e.tool.angle  = parseFloat(angle);
					var line = render.geometry.get_line_at_angle({x: e.origin[0], y: e.origin[1]}, e.tool.angle, e.tool.length);
					line.meta = render.copy_obj(render._defaultMeta);
					render.removePartFromStack("line_tool");
					hideSnapPointer();
					render.addEntityToPart(active_part, line);
					printf("Created Line -> origin: [" + line.origin[0].toFixed(4) + ", " + line.origin[1].toFixed(4) + "] end: [" + line.end[0].toFixed(4) + ", " + line.end[1].toFixed(4) + "]\n\r");
					ret();
					return_terminal_focus();
					on_tab_key = null;
				});
			});
		};
		e.origin = [render.snapPosition.x, render.snapPosition.y];
		e.end = [render.snapPosition.x, render.snapPosition.y];
		e.meta.on_mouse_motion = function(e){
	    e.end = [render.snapPosition.x, render.snapPosition.y];
			printf("\r-> [" + e.end[0].toFixed(4) + ", " + e.end[1].toFixed(4) + "] @ angle: " + render.geometry.get_line_angle({x: e.origin[0], y: e.origin[1]}, {x: e.end[0], y: e.end[1]}).toFixed(2) + "\r");
	  };
	  e.meta.on_mouse_click = function(e){
	    e.meta.on_mouse_motion = null;
	    var new_entity = render.copy_obj(e);
			new_entity.meta.color = "white";
	    //console.log("Creating line: " + JSON.stringify(new_entity));
	    render.removePartFromStack("line_tool");
			hideSnapPointer();
			render.addEntityToPart(active_part, new_entity);
			printf("Created Line -> origin: [" + new_entity.origin[0].toFixed(4) + ", " + new_entity.origin[1].toFixed(4) + "] end: [" + new_entity.end[0].toFixed(4) + ", " + new_entity.end[1].toFixed(4) + "]\n\r");
			ret(); //Return terminal stack
	    return true; //true - Delete the callback after it returns
	  };
    return false; //true - Delete the callback after it returns
  };
  part.entities.push(e);
  render.Stack.push(part);
}
function DrawCircle()
{
	printf("Drawing circle!\n\r-> Click origin point of circle!\n\r");
	showSnapPointer();
  var part = render.newLivePart("circle_tool");
  var e = {
    type: "circle",
    origin: [0, 0],
    radius: 0,
    meta: render.copy_obj(render._liveMeta)
  }
	showSnapPointer();
	e.meta.on_mouse_click = function(e){
		printf("-> Click endpoint to create circle or tab for manual radius input\n\r");
		on_tab_key = function(){
			save_terminal_focus();
			hideSnapPointer(); //Hide snap pointer because we are using manual input
			e.meta.on_mouse_motion = null; //cancel mouse motion callback so not to overide manual input
			printf("\r\nRadius? ");
			getline(function(radius){
				e.radius = parseFloat(radius);
				var new_entity = render.copy_obj(e);
				render.removePartFromStack("circle_tool");
				hideSnapPointer();
				render.addEntityToPart(active_part, new_entity);
				printf("Created Circle -> origin: [" + e.origin[0].toFixed(4) + ", " + e.origin[1].toFixed(4) + "] radius: " + e.radius + "\n\r");
				ret();
				return_terminal_focus();
				on_tab_key = null;
			});
		};
		e.origin = [render.snapPosition.x, render.snapPosition.y];
		e.radius = 0;
		e.meta.on_mouse_motion = function(e){
			var radius = render.geometry.distance({x: e.origin[0], y: e.origin[1]}, {x: render.snapPosition.x, y: render.snapPosition.y});
			printf("\r-> [" + e.origin[0].toFixed(4) + ", " + e.origin[1].toFixed(4) + "] @ radius: " + e.radius + "\r");
	  };
	  e.meta.on_mouse_click = function(e){
	    e.meta.on_mouse_motion = null;
	    var new_entity = render.copy_obj(e);
			new_entity.meta.color = "white";
	    //console.log("Creating line: " + JSON.stringify(new_entity));
	    render.removePartFromStack("circle_tool");
			hideSnapPointer();
			render.addEntityToPart(active_part, new_entity);
			printf("Created Circle -> origin: [" + new_entity.origin[0].toFixed(4) + ", " + new_entity.origin[1].toFixed(4) + "] radius: " + e.radius + "\n\r");
			ret(); //Return terminal stack
	    return true; //true - Delete the callback after it returns
	  };
    return false; //true - Delete the callback after it returns
  };
  part.entities.push(e);
  render.Stack.push(part);
}
