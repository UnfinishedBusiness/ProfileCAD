class JetCad {

  constructor(options)
  {
    this.View = {};
    this.Stack = [];
		this.ToolPaths = {};
		this.UndoStack = [];
		this.CopiedIndex = [];
		this.Model = {};
		this.StaticStack = ""; //This stays as SVG Code, no array
		this.LiveStack = [];
		this.HotKeys = [];
		this.SearchTools = [];
		this.ModifierKeys = {};
    this.AutoSaveHook = null;
		this.Callbacks = {};
		this.Callbacks.Escape = function(){};
		this.Callbacks.onTab = function(){};
		this.Callbacks.onMouse = null;
    this.Mouse = { x: 0, y: 0 };
		this.ClientMouse = { x: 0, y: 0 };
    this.DrawSpace;
		this.DrawingName = "";
    this.DrawPoint;
    this.Options = options;
    this.MakerJS = require('makerjs');
    this.AlreadyAttached = false;
		this.Debug = false;
		this.DisableHotkeys = false;
		this.DoubleClickSeconds = Number(new Date().getTime() / 1000);
		this.AutoSaveFlag = false;
		//Callbacks for drawing tools
		this.NextClick = null;
		this.ToolActive = null;
		this.SnappedMouse = null;
		this.ToolPoints = [];
		this.isMouseDown = false;

		//Hander bits
		this.HandlersAlreadyAttached = false;
		this.HandlersActive = false;

		this.StackChanged = true;
  }
	init()
  {
    $("#Content").html("<svg style='width: " + ($(window).width() - 30) + "px; height: " + ($(window).height() - 30) + "px; overflow: hidden;' id='draw'><g id='draw-group'></g></svg><div id='draw-info' style='position: absolute; bottom: 20px; right: 40px; font-weight: bold;'></div><input id='draw-search' size='35' style='position: absolute; bottom: 20px; left: 80px; font-weight: bold;'></input>");
    this.DrawSpace = document.getElementById("draw");
    this.DrawPoint = this.DrawSpace.createSVGPoint();
		this.View.last_zoom_time = 0;
		this.View.pan = {};
		this.View.pan.x = 600;
		this.View.pan.y = -400;
		this.View.zoom = 1;
		this.View.padding = {};
		this.View.padding.x = 0;
		this.View.padding.y = 0;
		this.View.zoom_sensititivy = 0.150;
		this.View.min_zoom = 0.1;
		this.View.max_zoom = 40000;
		this.View.MouseOverPadding = 10;
		this.View.MouseOverRadius = 4;
		this.View.line_width = 1.5;
		this.View.ZoomFitPadding = 0.1;
    this.live_entity_cutoff = 500;
		this.View.BoundingBox = {};
		var el = document.getElementById("draw");
		var rect = el.getBoundingClientRect();
		this.DrawPoint.x = rect.width;
		this.DrawPoint.y = rect.height;
    var point = this.DrawPoint.matrixTransform(this.DrawSpace.getScreenCTM().inverse());
		this.View.BoundingBox.height = point.y;
		this.View.BoundingBox.width = point.x;

		this.ModifierKeys.Shift = false;
		this.ModifierKeys.Ctrl = false;

    this.LatheMode = false;

		this.Debug = true;
		this.DragSelect = false;
		this.DragBox = {};
		this.isMouseDown = false;
		this.HandlersActive = true;
		document.getElementById("draw").addEventListener("DOMMouseScroll", this.UpdateZoom.bind(this)); //For FireFox scroll support
		document.getElementById("draw").addEventListener("mousewheel", this.UpdateZoom.bind(this));
		document.getElementById("draw-search").addEventListener("keyup", this.SearchBoxKeyUp.bind(this));
		document.getElementById("draw-search").addEventListener("keydown", this.SearchBoxKeyDown.bind(this));
		if (!this.HandlersAlreadyAttached)
		{
			setInterval(this.IdleLoop.bind(this), 2000);
			setInterval(this.RenderLoop.bind(this), 100);
			document.addEventListener("keyup", this.KeyUp.bind(this));
			document.addEventListener("keydown", this.KeyDown.bind(this));
			document.addEventListener("mousemove", this.UpdateMouse.bind(this));
			document.addEventListener("mousedown", this.MouseDown.bind(this));
			document.addEventListener("mouseup", this.MouseUp.bind(this));
			this.HandlersAlreadyAttached = true;
		}
		$("#draw-search").hide();
  }
	RenderLoop()
	{
		if (this.LiveStack.length === 0)
		{
			this.render();
		}

	}
	IdleLoop()
	{
		if (this.HandlersActive)
		{
			//console.log("Idle!");
			if (this.AutoSaveFlag)
			{
				console.log("Auto-Save!");
				this.AutoSaveFlag = false;
        if (this.AutoSaveHook != null)
        {
          this.AutoSaveHook();
        }
			}
		}
	}
	close()
	{
		this.HandlersActive = false;
	}
	OpenDrawing(drawing_name)
	{
		this.DrawingName = drawing_name;
		//$("#drawing_name").html(this.DrawingName);
		/*backend_cmd_async({cmd: "read_drawing", drawing_name: drawing_name }, function(data){
			console.log(drawing_name);
			jetcad.Stack = JSON.parse(data.data);
			jetcad.zoomExtents();
			//jetcad.render();
		});*/
	}
	RegisterUndo()
	{
		this.AutoSaveFlag = true;
		this.UndoStack = JSON.parse(JSON.stringify(this.Stack));
	}
	CommitUndo()
	{
		this.AutoSaveFlag = true;
		this.Stack = JSON.parse(JSON.stringify(this.UndoStack));
		this.render();
	}
	log(text)
	{
		if (this.Debug)
		{
			console.log(text);
		}
	}
	SearchBoxKeyDown(e)
	{
		if (e.key == "Tab")
		{
			e.preventDefault();
		}
	}
	SearchBoxKeyUp(e)
	{
		//console.log(e);
		this.HandlersActive = false;
		if (e.key == "Tab")
		{
			e.preventDefault();
			var search = $("#draw-search").val();
			search = search.toLowerCase();
			var search_tools = JSON.parse(JSON.stringify(this.SearchTools));
			for (var i = 0; i < search_tools.length; i++)
			{
				var score = 0;
				for (var x = 0; x < search_tools[i].tool.length; x++)
				{
					if (search[x] == search_tools[i].tool[x]) score += 1;
				}
				search_tools[i].score = score;
			}
			//console.log(search_tools);
			var winner = "";
			var bar = 0;
			for (var i = 0; i < search_tools.length; i++)
			{
				if (search_tools[i].score > bar)
				{
					bar = search_tools[i].score;
					winner = search_tools[i].tool;
				}
			}
			$("#draw-search").val(winner);
		}
		if (e.key == "Escape")
		{
			$("#draw-search").val("");
			$("#draw-search").hide();
			this.HandlersActive = true;
		}
		if (e.key == "Enter")
		{
			for (var i = 0; i < this.SearchTools.length; i++)
			{
				if (this.SearchTools[i].tool == $("#draw-search").val())
				{
					(this.SearchTools[i].run)();
				}
			}
			$("#draw-search").val("");
			$("#draw-search").hide();
			this.HandlersActive = true;
		}
	}
	ZoomIn()
	{
		this.View.zoom += (this.View.zoom_sensititivy * this.View.zoom);
		if (this.View.zoom > this.View.max_zoom)
		{
			this.View.zoom = this.View.max_zoom;
		}
		//this.render();
	}
	ZoomOut()
	{
		this.View.zoom -= (this.View.zoom_sensititivy * this.View.zoom);
		if (this.View.zoom < this.View.min_zoom)
		{
			this.View.zoom = this.View.min_zoom;
		}
		//this.render();
	}
	UpdateZoom(e)
	{
		e.preventDefault();
		var old_zoom = this.View.zoom;
		//console.log(e.detail);

		var direction = e.wheelDelta;
		if (direction === undefined)
		{
			//Must not be using chrome...
			direction = e.detail * -1;
		}

		if (direction > 0)
		{
			this.View.zoom += (this.View.zoom_sensititivy * this.View.zoom);
			if (this.View.zoom > this.View.max_zoom)
			{
				this.View.zoom = this.View.max_zoom;
			}
		}
		else
		{
			this.View.zoom -= (this.View.zoom_sensititivy * this.View.zoom);
			if (this.View.zoom < this.View.min_zoom)
			{
				this.View.zoom = this.View.min_zoom;
			}
		}
		var scalechange = this.View.zoom - old_zoom;
		this.panX((this.Mouse.x * scalechange) * -1);
		this.panY((this.Mouse.y * scalechange) * -1);
		this.updateView();
		this.View.last_zoom_time = Number(new Date().getTime() / 1000);
		//this.render();
		//this.log(this.View.zoom);
	}
  UpdateMouse(e)
  {
		if (!this.HandlersActive) return;
		var snapped = false;
		var snapped_point = {};
    var loc = this.cursorPoint(e);
    this.Mouse = { x:  loc.x, y: loc.y};
		this.Mouse.x = Number(this.Mouse.x.toFixed(5));
		this.Mouse.y = Number(this.Mouse.y.toFixed(5));

		if (this.DragSelect && this.isMouseDown)
		{
			this.DragSelectMoved = true;
			//Update selection box
			//console.log("Update box");
			var width = this.Mouse.x - this.ToolPoints[0].x;
			var height = this.Mouse.y - this.ToolPoints[0].y;

			var fill = "fill='#044B94' fill-opacity='0.4' stroke-width='" + this.View.line_width + "' vector-effect='non-scaling-stroke' ";

			if (width < 0 && height < 0)
			{
				this.StaticStack = '<rect ' + fill + ' stroke="blue" x="' + this.Mouse.x + '" y="' + this.Mouse.y + '" width="' + width * -1 + '" height="' + height * -1 + '" />';
				//console.log("1");
				this.DragBox.TopLeft = { x: this.Mouse.x , y: this.Mouse.y + (height * -1) };
				this.DragBox.BottomRight = { x: this.Mouse.x + (width * -1), y: this.Mouse.y };
			}
			else if (width > 0 && height > 0)
			{
				this.StaticStack = '<rect ' + fill + ' stroke="blue" x="' + this.ToolPoints[0].x + '" y="' + this.ToolPoints[0].y + '" width="' + width + '" height="' + height + '" />';
				//console.log("2");
				this.DragBox.TopLeft = { x: this.ToolPoints[0].x, y: this.ToolPoints[0].y + (height * -1) };
				this.DragBox.BottomRight = { x: this.ToolPoints[0].x + (width * -1), y: this.ToolPoints[0].y };
			}
			else if (width > 0 && height < 0)
			{
				this.StaticStack = '<rect ' + fill + ' stroke="blue" x="' + this.ToolPoints[0].x + '" y="' + this.Mouse.y + '" width="' + width + '" height="' + height * -1 + '" />';
				//console.log("3");
				this.DragBox.TopLeft = { x: this.ToolPoints[0].x, y: this.Mouse.y + (height * -1) };
				this.DragBox.BottomRight = { x: this.ToolPoints[0].x + (width), y: this.Mouse.y };
			}
			else if (width < 0 && height > 0)
			{
				this.StaticStack = '<rect ' + fill + ' stroke="blue" x="' + this.Mouse.x + '" y="' + this.ToolPoints[0].y + '" width="' + width * -1 + '" height="' + height + '" />';
				this.DragBox.TopLeft = { x: this.Mouse.x, y: this.ToolPoints[0].y + (height) };
				this.DragBox.BottomRight = { x: this.Mouse.x + (width * -1), y: this.ToolPoints[0].y };
				//console.log("4");
			}
		}



		this.SnappedMouse = this.Mouse; //Zero Entity bug, this was the issue...

    if (this.DragSelect === false && this.Callbacks.onMouse === null && this.LiveStack.length === 0 && this.isMouseDown)
		{
			//console.log("Begin Drag Select!");
			this.DragSelect = true;
			this.ToolPoints[0] = this.SnappedMouse; //Drag Select start point
		}

		for (var x = 0; x < this.Stack.length; x++)
		{
			if (this.Stack[x].meta === undefined)
			{
				break;
			}
			if (this.Stack[x].meta.SnapPoints !== undefined)
			{
				if (this.Stack[x].meta.SnapPoints.length > 0)
				{
					for (var i = 0; i < this.Stack[x].meta.SnapPoints.length; i++)
					{
						//Check if Snap Point is close to mouse point and move curser box to point
						if(this.distance(this.Mouse,  this.Stack[x].meta.SnapPoints[i]) < (this.View.MouseOverPadding / this.View.zoom) )
						{
							snapped = true;
							snapped_point = this.Stack[x].meta.SnapPoints[i];
              if (this.LatheMode === true) this.SetStatusText("D: " + (snapped_point.y * 2).toFixed(4) + " , Z: " + snapped_point.x.toFixed(4) );
              break;
						}
					}
				}
			}
		}
		if(this.distance(this.Mouse,  { x: 0, y: 0 }) < (this.View.MouseOverPadding / this.View.zoom) )
		{
				//Always Snap to origin!
				snapped = true;
				snapped_point = { x: 0, y: 0 };
		}
		if (snapped) this.SnappedMouse = snapped_point;
		if (this.Callbacks.onMouse != null) (this.Callbacks.onMouse)();
		this.render(true);
		if (this.Stack.length < this.live_entity_cutoff) //We need to change this to a time measurement!
		{
			this.render(false);
		}

  }
	MouseUp(e)
	{
		this.isMouseDown = false;
		if (this.DragSelect && this.DragSelectMoved === true)
		{
			var in_box = function(o){

				var p = { x: o[0], y: o[1] };

				if( p.x > jetcad.DragBox.TopLeft.x &&
					 	p.x < jetcad.DragBox.BottomRight.x &&
					 	p.y < jetcad.DragBox.TopLeft.y &&
					 	p.y > jetcad.DragBox.BottomRight.y )
				{
    			return true;
				}
				else
				{
					return false;
				}
			}
			this.DragSelect = false;
			this.DragSelectMoved = false;
			//Close selection rectangle and select entities that are within

			for (var i = 0; i < this.Stack.length; i++)
			{
				if (this.Stack[i].type == "line")
				{
					if (in_box(this.Stack[i].origin) && in_box(this.Stack[i].end))
					{
						this.Stack[i].meta.Selected = true;
					}
				}
				if (this.Stack[i].type == 'circle')
				{
					var radius = this.Stack[i].radius;
					var top_left = [ this.Stack[i].origin[0] - radius, this.Stack[i].origin[1] + radius ];
					var top_right = [ this.Stack[i].origin[0] + radius, this.Stack[i].origin[1] + radius ];
					var bottom_left = [ this.Stack[i].origin[0] - radius, this.Stack[i].origin[1] - radius ];
					var bottom_right = [ this.Stack[i].origin[0] + radius, this.Stack[i].origin[1] - radius ];

					if (in_box(top_left) && in_box(top_right) && in_box(bottom_left) && in_box(bottom_right))
					{
						this.Stack[i].meta.Selected = true;
					}
				}
				if (this.Stack[i].type == "arc")
				{
					var start_point = this.get_line_at_angle({ x: this.Stack[i].origin[0], y: this.Stack[i].origin[1]}, this.Stack[i].startAngle, this.Stack[i].radius).end;
					var end_point = this.get_line_at_angle({ x: this.Stack[i].origin[0], y: this.Stack[i].origin[1]}, this.Stack[i].endAngle, this.Stack[i].radius).end;
					if (in_box(start_point) && in_box(end_point))
					{
						this.Stack[i].meta.Selected = true;
					}
				}
			}

			this.StaticStack = "";
			//console.log("End selection!");
		}
	}
	MouseDown(e)
	{
		if (!this.HandlersActive) return;
		this.isMouseDown = true;

    //Drag select should only start when movement is seen while mousebutton is down
		/*if (this.DragSelect === false && this.Callbacks.onMouse === null && this.LiveStack.length === 0)
		{
			//console.log("Begin Drag Select!");
			this.DragSelect = true;
			this.ToolPoints[0] = this.Mouse; //Drag Select start point
		}*/

		//console.log("Click!");
		var seconds = Number(new Date().getTime() / 1000);
		//console.log(seconds - this.DoubleClickSeconds);
		if ((seconds - this.DoubleClickSeconds) < 0.3)
		{
			if (this.Callbacks.DoubleClick !== null) (this.Callbacks.DoubleClick)();
			return;
		}
		this.DoubleClickSeconds = seconds;
		if (this.NextClick != null)
		{
			for (var x = 0; x < this.NextClick.length; x++)
			{
				if (this.NextClick[x] != null)
				{
					(this.NextClick[x])();
					this.NextClick[x] = null;
					return;
				}
			}
		}
		//Determine if we are close or is we are close to a line
		for (var x = 0; x < this.Stack.length; x++)
		{
			if (this.metaIsMouseOver(x))
			{
				this.metaSelected(x, !this.metaIsSelected(x));
				this.Stack[x].meta.SelectedTimeStamp = Date.now(); //We need this to order selections
			}
		}
		//console.log(this.Mouse);
		//this.render();
		this.UpdateStatusText();
	}
	KeyDown(e)
	{
		if (!this.HandlersActive) return;
		//console.log(e.key);
		if (e.ctrlKey)
		{
			jetcad.ModifierKeys.Ctrl = true;
		}
		if (e.key == "Shift") jetcad.ModifierKeys.Shift = true;
		if (e.key == "Tab")
		{
			e.preventDefault();
			if (jetcad.Callbacks.onTab != null)
			{
				jetcad.Callbacks.onTab();
			}
		}
		if (e.key == "Enter")
		{
			e.preventDefault();
			if (jetcad.Callbacks.onEnter != null)
			{
				jetcad.Callbacks.onEnter();
			}
		}
		if (e.ctrlKey)
		{
			if (e.key == "s")
			{
				e.preventDefault();
			}
		}

		//console.log(e);
	}
	KeyUp(e)
	{
		if (!this.HandlersActive) return;

		if (e.key == "Tab")
		{
			e.preventDefault();
		}
		jetcad.ModifierKeys.Shift = false;
		//This function gets called outside of context, besure
		//jetcad.log(e);
		var parsed_key = "";
		if (e.ctrlKey)
		{
			parsed_key = "ctrl-" + e.key;
		}
		else
		{
			parsed_key = e.key;
			jetcad.ModifierKeys.Ctrl = false;
		}
		//console.log(parsed_key);
		if (this.DisableHotkeys && e.key != "Escape")
		{
			e.preventDefault();
			return;
		}
		for (var i = 0; i < jetcad.HotKeys.length; i++)
		{
			if (jetcad.HotKeys[i].key == parsed_key)
			{
				//console.log(parsed_key);
				(jetcad.HotKeys[i].run)(jetcad);
				e.preventDefault();
			}

		}
	}
	TouchStacks()
	{
		if (this.LiveStack.length > 0)
		{
			for (var i = 0; i < this.LiveStack.length; i++)
			{
				if (this.LiveStack[i] == null)
				{
					this.LiveStack[i].onMouse = function(pos){ return; };
				}
				var PreserveCallback = this.LiveStack[i].onMouse;
				this.LiveStack[i] = (this.LiveStack[i].onMouse)({x: Number(this.Mouse.x.toFixed(4)), y: Number(this.Mouse.y.toFixed(4))}, this);
				this.LiveStack[i].onMouse = PreserveCallback;
			}
		}
		//this.log(this.Mouse)
		//Iterate stack and toggle a "MouseOver" meta variable when mouse is within "MouseOver tolerance", then render. Renderer will overide color to switch to mouseover color
		for (var x = 0; x < this.Stack.length; x++)
		{
			if (this.Stack[x].type == "line")
			{
				var angle = this.get_line_angle({x: this.Stack[x].origin[0], y: this.Stack[x].origin[1] }, {x: this.Stack[x].end[0], y: this.Stack[x].end[1] }) + 90;
				if (angle >= 360) angle -= 360;
				var line1 = this.get_line_at_angle({x: this.Mouse.x, y: this.Mouse.y}, angle, (this.View.MouseOverPadding / this.View.zoom) / 2);
				var line2 = this.get_line_at_angle({x: this.Mouse.x, y: this.Mouse.y}, angle + 180, (this.View.MouseOverPadding / this.View.zoom) / 2);

				var actual_line =  { type: 'line', origin: line1.end, end: line2.end };
				let int = this.get_intersection(actual_line, this.Stack[x]);
				if (int)
				{
					this.metaMouseOver(x, true);
					var midpoint = this.midpoint({x: this.Stack[x].origin[0], y: this.Stack[x].origin[1]}, {x: this.Stack[x].end[0], y: this.Stack[x].end[1]});
					var intersections = this.get_all_intersections(this.Stack[x]);
					var snap_points = [ {x: this.Stack[x].origin[0], y: this.Stack[x].origin[1]}, {x: this.Stack[x].end[0], y: this.Stack[x].end[1]}, { x: midpoint.x, y: midpoint.y } ];
					for(var y = 0; y < intersections.length; y++)
					{
						if (intersections[y].intersectionPoints !== undefined)
						{
							var points = intersections[y].intersectionPoints
							for (var z = 0; z < points.length; z++)
							{
								snap_points.push({ x: points[z][0], y: points[z][1]});
							}
						}

					}
					this.metaSnapPoints(x, snap_points);

				}
				else
				{
					this.metaMouseOver(x, false);
				}

			}
			if (this.Stack[x].type == "circle")
			{
				if (this.distance(this.Mouse, { x: this.Stack[x].origin[0], y: this.Stack[x].origin[1]} ) < this.Stack[x].radius + (this.View.MouseOverPadding / this.View.zoom) &&
						this.distance(this.Mouse, { x: this.Stack[x].origin[0], y: this.Stack[x].origin[1]} ) > this.Stack[x].radius - (this.View.MouseOverPadding / this.View.zoom))
				{
					this.metaMouseOver(x, true);
					this.metaSnapPoints(x, [ {x: this.Stack[x].origin[0], y: this.Stack[x].origin[1]} ]);
				}
				else
				{
					this.metaMouseOver(x, false);
				}
			}
			if (this.Stack[x].type == "arc")
			{
				var drag_line =  { type: 'line', origin: this.Stack[x].origin, end: [ this.Mouse.x, this.Mouse.y] };
				let int = this.get_intersection(drag_line, this.Stack[x]);
				if (int)
				{
					if (this.distance(this.Mouse, { x: this.Stack[x].origin[0], y: this.Stack[x].origin[1]} ) < this.Stack[x].radius + (this.View.MouseOverPadding / this.View.zoom) &&
						this.distance(this.Mouse, { x: this.Stack[x].origin[0], y: this.Stack[x].origin[1]} ) > this.Stack[x].radius - (this.View.MouseOverPadding / this.View.zoom))
					{
						this.metaMouseOver(x, true);
						var start = this.polarToCartesian(this.Stack[x].origin[0], this.Stack[x].origin[1], this.Stack[x].radius, this.Stack[x].startAngle);
						var end = this.polarToCartesian(this.Stack[x].origin[0], this.Stack[x].origin[1], this.Stack[x].radius, this.Stack[x].endAngle);
						this.metaSnapPoints(x, [ {x: this.Stack[x].origin[0], y: this.Stack[x].origin[1]}, start, end ]);
					}
					else
					{
						this.metaMouseOver(x, false);
					}
				}
				else
				{
					this.metaMouseOver(x, false);
				}

			}
		}
	}
  cursorPoint(evt)
	{
		this.ClientMouse = {x: evt.clientX, y: evt.clientY };
    this.DrawPoint.x = evt.clientX; this.DrawPoint.y = evt.clientY;
		var svg = document.getElementById('draw-group');
    return this.DrawPoint.matrixTransform(svg.getScreenCTM().inverse());
  }
	panX(p)
	{
		this.View.pan.x += p;
		//this.render();
	}
	panY(p)
	{
		this.View.pan.y += p;
		//this.render();
	}
	getExtents()
	{
		var StaticStack = [];
		var x_array = [];
		var y_array = [];

		for (var x = 0; x < this.Stack.length; x++)
		{
			if (this.Stack[x].type === "line")
			{
				x_array.push(this.Stack[x].origin[0]);
				x_array.push(this.Stack[x].end[0]);

				y_array.push(this.Stack[x].origin[1]);
				y_array.push(this.Stack[x].end[1]);

				StaticStack.push({type: "circle", origin: this.Stack[x].origin, radius: 0.1});
				StaticStack.push({type: "circle", origin: this.Stack[x].end, radius: 0.1});

			}
			if (this.Stack[x].type === "arc")
			{
				var start_point = this.get_line_at_angle({ x: this.Stack[x].origin[0], y: this.Stack[x].origin[1]}, this.Stack[x].startAngle, this.Stack[x].radius).end;
				var end_point = this.get_line_at_angle({ x: this.Stack[x].origin[0], y: this.Stack[x].origin[1]}, this.Stack[x].endAngle, this.Stack[x].radius).end;

        var q1 = [ this.Stack[x].origin[0] , this.Stack[x].origin[1] + this.Stack[x].radius];
				var q2 = [ this.Stack[x].origin[0] + this.Stack[x].radius, this.Stack[x].origin[1]];
				var q3 = [ this.Stack[x].origin[0] - this.Stack[x].radius, this.Stack[x].origin[1]];
				var q4 = [ this.Stack[x].origin[0] , this.Stack[x].origin[1] - this.Stack[x].radius];

				//var origin = this.Stack[x].origin;

				StaticStack.push({type: "circle", origin: start_point, radius: 0.1});
				StaticStack.push({type: "circle", origin: end_point, radius: 0.1});
				//StaticStack.push({type: "circle", origin: origin, radius: 0.1});

				x_array.push(start_point[0]);
				y_array.push(start_point[1]);

				x_array.push(end_point[0]);
				y_array.push(end_point[1]);

        if (this.Stack.length < 300) //Dont bother wasting time with this many entities
        {
          if (this.get_all_intersections({ type: "line", origin: this.Stack[x].origin, end: q1}).length > 0)
          {
            x_array.push(q1[0]);
            y_array.push(q1[1]);
            StaticStack.push({type: "circle", origin: q1, radius: 0.1});
          }
          if (this.get_all_intersections({ type: "line", origin: this.Stack[x].origin, end: q2}).length > 0)
          {
            x_array.push(q2[0]);
            y_array.push(q2[1]);
            StaticStack.push({type: "circle", origin: q2, radius: 0.1});
          }
          if (this.get_all_intersections({ type: "line", origin: this.Stack[x].origin, end: q3}).length > 0)
          {
            x_array.push(q3[0]);
            y_array.push(q3[1]);
            StaticStack.push({type: "circle", origin: q3, radius: 0.1});
          }
          if (this.get_all_intersections({ type: "line", origin: this.Stack[x].origin, end: q4}).length > 0)
          {
            x_array.push(q4[0]);
            y_array.push(q4[1]);
            StaticStack.push({type: "circle", origin: q4, radius: 0.1});
          }
        }
			}
			if (this.Stack[x].type === "circle")
			{
				var q1 = [ this.Stack[x].origin[0] , this.Stack[x].origin[1] + this.Stack[x].radius];
				var q2 = [ this.Stack[x].origin[0] + this.Stack[x].radius, this.Stack[x].origin[1]];
				var q3 = [ this.Stack[x].origin[0] - this.Stack[x].radius, this.Stack[x].origin[1]];
				var q4 = [ this.Stack[x].origin[0] , this.Stack[x].origin[1] - this.Stack[x].radius];

				StaticStack.push({type: "circle", origin: q1, radius: 0.1});
				StaticStack.push({type: "circle", origin: q2, radius: 0.1});
				StaticStack.push({type: "circle", origin: q3, radius: 0.1});
				StaticStack.push({type: "circle", origin: q4, radius: 0.1});

				x_array.push(q1[0]);
				y_array.push(q1[1]);

				x_array.push(q2[0]);
				y_array.push(q2[1]);

				x_array.push(q3[0]);
				y_array.push(q3[1]);

				x_array.push(q4[0]);
				y_array.push(q4[1]);
			}
		}
		x_array.sort(function(a, b) {
    	return a - b;
		});
		y_array.sort(function(a, b) {
    	return a - b;
		});
    if (x_array.length > 1 && y_array.length > 1 )
    {
      var bigX = x_array[x_array.length - 1];
      var bigY = y_array[y_array.length - 1];
      var smallX = x_array[0];
      var smallY = y_array[0];
    }
		else
    {
      var bigX = 0;
      var bigY = 0;
      var smallX = 0;
      var smallY = 0;
    }

		StaticStack.push({type: "circle", origin: [((bigX + smallX) / 2), ((bigY + smallY) / 2)], radius: 0.1});

		StaticStack.push({type: "line", origin: [ smallX, bigY ], end: [ bigX, smallY ] });
    StaticStack.push({type: "line", origin: [ smallX, smallY ], end: [ bigX, bigY ] });

		//this.StaticStack = jetcad.StaticRender(StaticStack);

		return { smallX: smallX, bigX: bigX, smallY: smallY, bigY: bigY, origin: { x: ((bigX + smallX) / 2), y: ((bigY + smallY) / 2) } };
	}
	centerExtents()
	{
		var extents = this.getExtents();

		var inc_pan_x = (extents.origin.x * this.View.zoom) + this.View.pan.x;
		var inc_pan_y = (extents.origin.y * this.View.zoom) + this.View.pan.y;

		this.panX(inc_pan_x * -1); //This puts us to the left side based on the extents origin
		this.panY(inc_pan_y * -1); //This puts us to the top side based on the extents origin

		this.panX((this.View.BoundingBox.width / 2) + 40); //Now inc pan to half of the width
		this.panY(((this.View.BoundingBox.height / 2) * -1) - 40); //Now inc pan to half of the height
	}
	zoomExtents()
	{
		var extents = this.getExtents();
		var drawing_height_extent = extents.bigY - extents.smallY;
		var drawing_width_extent = extents.bigX - extents.smallX;

    if (this.Stack.length > 0)
    {
      var width_distance = (drawing_height_extent * this.View.zoom) / (this.View.BoundingBox.height);
      var height_distance = (drawing_width_extent * this.View.zoom) / (this.View.BoundingBox.width);

      console.log("Width Extent: " + width_distance);
      console.log("Height Extent: " + height_distance);

      if (height_distance < width_distance)
      {
        //We fit to height!
        var scale_factor = (drawing_height_extent * this.View.zoom) / (this.View.BoundingBox.height);
        this.View.zoom /= scale_factor;
        console.log("Fitting height!");
      }
      else
      {
        //we fit to width!
        var scale_factor = (drawing_width_extent * this.View.zoom) / (this.View.BoundingBox.width);
        this.View.zoom /= scale_factor;
        console.log("Fitting width!");
      }
    }
		this.centerExtents();
	}
	updateView()
	{
		document.getElementById("draw-group").setAttribute("transform", "scale(" + this.View.zoom + ", " + this.View.zoom * -1 + "), translate(" + (this.View.pan.x / this.View.zoom) + ", " + (this.View.pan.y / this.View.zoom) + ")");
	}
	StaticRender(Stack)
	{
		var color = "red";
		var svg = "";
		for (var x = 0; x < Stack.length; x++)
		{
      if (Stack[x].type == "arrow")
			{
				svg += this.svgLine({ x: Stack[x].origin[0], y: Stack[x].origin[1] }, { x: Stack[x].end[0], y: Stack[x].end[1] }, color);
        var midpoint = this.midpoint({ x: Stack[x].origin[0], y: Stack[x].origin[1] }, { x: Stack[x].end[0], y: Stack[x].end[1] });
        var line_angle = this.get_line_angle({ x: Stack[x].origin[0], y: Stack[x].origin[1] }, { x: Stack[x].end[0], y: Stack[x].end[1] });
        var first_line = this.get_line_at_angle(midpoint, line_angle - 135, 0.010);
        var second_line = this.get_line_at_angle(midpoint, line_angle + 135, 0.010);
        svg += this.svgLine({ x: first_line.origin[0], y: first_line.origin[1] }, { x: first_line.end[0], y: first_line.end[1] }, color);
        svg += this.svgLine({ x: second_line.origin[0], y: second_line.origin[1] }, { x: second_line.end[0], y: second_line.end[1] }, color);
			}
			if (Stack[x].type == "line")
			{
				svg += this.svgLine({ x: Stack[x].origin[0], y: Stack[x].origin[1] }, { x: Stack[x].end[0], y: Stack[x].end[1] }, color);
			}
			if (Stack[x].type == "circle")
			{
				svg += this.svgCircle({ x: Stack[x].origin[0], y: Stack[x].origin[1] }, Stack[x].radius, color);
			}
			if (Stack[x].type == "arc")
			{
				svg += this.svgArc({ x: Stack[x].origin[0], y: Stack[x].origin[1] }, Stack[x].radius, Stack[x].startAngle, Stack[x].endAngle, color);
			}
		}
		return svg;
	}
	ShiftStack(Stack, shift)
	{
		var new_stack = JSON.parse(JSON.stringify(Stack)); //Break link between original stack and new return stack
		for (var x = 0; x < new_stack.length; x++)
		{
			if (new_stack[x].type == "line")
			{
				new_stack[x].origin[0] += shift.x;
				new_stack[x].origin[1] += shift.y;
				new_stack[x].end[0] += shift.x;
				new_stack[x].end[1] += shift.y;
			}
			if (new_stack[x].type == "circle" || new_stack[x].type == "arc")
			{
				new_stack[x].origin[0] += shift.x;
				new_stack[x].origin[1] += shift.y;
			}
		}
		return new_stack;
	}
  render(live)
  {
		var svg = "";
		this.Model = {};
		this.Model.paths = [];
		var color = "black";
		if (live)
		{
			//console.log("live");
			if (this.LiveStack.length > 0)
			{
				this.TouchStacks();
				for (var x = 0; x < this.LiveStack.length; x++)
				{
					if (this.LiveStack[x].type == "line")
					{
						svg += this.svgLine({ x: this.LiveStack[x].origin[0], y: this.LiveStack[x].origin[1] }, { x: this.LiveStack[x].end[0], y: this.LiveStack[x].end[1] }, "green");
					}
					if (this.LiveStack[x].type == "circle")
					{
						svg += this.svgCircle({ x: this.LiveStack[x].origin[0], y: this.LiveStack[x].origin[1]}, this.LiveStack[x].radius, "green");
					}
				}
				this.View.live_svg = svg;
				document.getElementById("draw-group").innerHTML = this.View.svg + svg;
				//console.log("Rednering live!")
			}
			else
			{
				this.View.live_svg = "";
			}
			return;
		}
		if ((Number(new Date().getTime() / 1000) - this.View.last_zoom_time) > 0.5)
		{
			//console.log("Start rendering again!");
			/*if (this.LiveStack.length > 0)
			{
				return;
			}*/
		}
		else
		{
			return;
		}

		this.TouchStacks();

		/*
		 Room for improvement - Check if stack entity's have changed, if not, only update scale and transform!
		*/
		this.updateView();

		if (this.StackChanged === false)
		{
			return;
		}
		//console.log("rendering!");

		svg += this.svgCrosshair({x: 0, y: 0}, this.View.MouseOverRadius / this.View.zoom,"red");

		for (var x = 0; x < this.Stack.length; x++)
		{
			color = this.metaGetColor(x);
			if (this.metaIsSelected(x))
			{
				color = "limegreen";
			}
			if (this.metaIsMouseOver(x))
			{
				color = "blue";
				if (this.Stack[x].meta.SnapPoints.length > 0)
				{
					for (var i = 0; i < this.Stack[x].meta.SnapPoints.length; i++)
					{
						svg += this.svgCircle({ x: this.Stack[x].meta.SnapPoints[i].x, y: this.Stack[x].meta.SnapPoints[i].y }, this.View.MouseOverRadius / this.View.zoom, "red");
					}
				}
			}
			if (this.Stack[x].type == "line")
			{
				svg += this.svgLine({ x: this.Stack[x].origin[0], y: this.Stack[x].origin[1] }, { x: this.Stack[x].end[0], y: this.Stack[x].end[1] }, color, this.metaGetStyle(x));
			}
			if (this.Stack[x].type == "circle")
			{
				svg += this.svgCircle({ x: this.Stack[x].origin[0], y: this.Stack[x].origin[1] }, this.Stack[x].radius, color, this.metaGetStyle(x));
			}
			if (this.Stack[x].type == "arc")
			{
				svg += this.svgArc({ x: this.Stack[x].origin[0], y: this.Stack[x].origin[1] }, this.Stack[x].radius, this.Stack[x].startAngle, this.Stack[x].endAngle, color, this.metaGetStyle(x));
			}
			this.Model.paths.push(this.Stack[x]);
		}
		svg += this.StaticStack;
		this.View.svg = svg;
		document.getElementById("draw-group").innerHTML = svg + this.View.live_svg;
  }
	get_input_box_value(id)
	{
		var raw_value = $("#" + id).val();
		if (raw_value == undefined)
		{
			return 0;
		}
		if (raw_value.indexOf(" ") != -1)
		{
			//console.log("Has space");
			var text_value = raw_value.split(" ")[0];
			var number_value = eval(text_value);
			return Number(number_value);
		}
		else
		{
			//console.log("No space");
			return Number(eval(raw_value));
		}
	}
	input_box(id, text, static_, position)
	{
		$("#input_boxes").append("<input value='" + text + " " + static_ + "' size='18' id='" + id + "' type='text' style='position: absolute; top: " + position.y + "px; left: " + position.x + "px;' ></input>");
		//$("#" + id).css({left: position.x, top: position.y});
	}
	clear_input_boxes()
	{
		$("#input_boxes").html(" ");
	}
  test_drawings()
  {
    /*var line = {
        type: 'line',
        origin: [0, 0],
        end: [0, 8]
       };
		this.Stack.push(line);*/


      var circle = {
        type: 'circle',
        origin: [0, 0],
        radius: 1
       };

    //this.Stack.push(circle);

		var arc = {
        type: 'arc',
        origin: [0, 0],
        radius: 5,
				startAngle: 0,
				endAngle: 180
       };

    this.Stack.push(arc);

		var arc = {
        type: 'arc',
        origin: [0, 0],
        radius: 1,
				startAngle: 180,
				endAngle: 90
       };

    //this.Stack.push(arc);

		var arc = {
        type: 'arc',
        origin: [1, 1],
        radius: 1,
				startAngle: 45,
				endAngle: 135
       };

    this.Stack.push(arc);


		var line_endpoint = this.polarToCartesian(0, 0, 10, 0)

		var line = {
        type: 'line',
        origin: [0, 0],
        end: [line_endpoint.x, line_endpoint.y]
       };
		this.Stack.push(line);

		var line_endpoint = this.polarToCartesian(0, 0, 10, 90)

		var line = {
        type: 'line',
        origin: [0, 0],
        end: [line_endpoint.x, line_endpoint.y]
       };
		this.Stack.push(line);
  }
	test_live()
	{
		this.pushLive(function(m){
			var circle = {
        type: 'circle',
        origin: [m.x, m.y],
        radius: 5
       };
			//console.log(circle);
			return circle;
		});
		/*this.pushLive(function(m){
			var e = {
        type: 'line',
        origin: [0, 0],
				end: [m.x, m.y]
       };
			//console.log(circle);
			return e;
		});*/

	}
  drawLine(l)
  {
    var line = {
        type: 'line',
        origin: [l.origin.x, l.origin.y],
        end: [l.end.x, l.end.y]
       };
    this.Stack.push(line);
		//this.render();
  }
  drawCircle(c)
  {
    var circle = {
        type: 'circle',
        origin: [c.origin.x, c.origin.y],
        radius: c.radius
       };
    this.Stack.push(circle);
		//this.render();
  }
	test_expand(p, i)
	{
		var expaned = this.MakerJS.path.expand(this.Model.paths[p], i);
		this.Stack.push(expaned.paths.Ring_inner);
		this.Stack.push(expaned.paths.Ring_outer);
		//this.render();
	}
	pushLive(onMouse)
  {
    this.LiveStack.push({ onMouse: onMouse });
		//this.render();
  }
	clearLive()
	{
		this.LiveStack = [];
		//this.render();
	}
  breakLink(obj)
  {
    return JSON.parse(JSON.stringify(obj));
  }
	//Math functions specific to drawing
	offset_selected(dist)
	{
		var Selected = jetcad_tools.getSelected(); //Get Selected in order
		var NewStack = [];
		var distance = dist;
		var direction = false; //Outside
		if (distance < 0) direction = true;
		var distance = Math.abs(distance);
		var model = {};
		model.models = {};
		model.models.drawing = {};
		model.models.drawing.paths = [];
		for (var i = 0; i < Selected.length; i++)
		{
			model.models.drawing.paths.push(Selected[i]);
		}
		//console.log(model);
		var offset = jetcad.MakerJS.model.outline(model, distance, 0, direction);
  	//var offset = jetcad.MakerJS.model.expandPaths(model, distance, 0);
		//console.log(offset);
		for (var model_number in offset.models)
		{
			for (var pathname in offset.models[model_number].paths)
			{
				var offset_path = offset.models[model_number].paths[pathname];
				NewStack.push(offset_path);
			}
		}

		return NewStack;
	}
	polarToCartesian(centerX, centerY, radius, angleInDegrees) {
		var angleInRadians = (angleInDegrees) * Math.PI / 180.0;

		return {
			x: centerX + (radius * Math.cos(angleInRadians)),
			y: centerY + (radius * Math.sin(angleInRadians))
		};
	}

	describeArc(x, y, radius, startAngle, endAngle){
			//console.log("Arc - x: " + x + " y: " + y + " radius: " + radius + " startAngle: " + startAngle + " endAngle: " + endAngle);
			var end = this.polarToCartesian(x, y, radius, startAngle);
			var start = this.polarToCartesian(x, y, radius, endAngle);

			var arcSweep = endAngle - startAngle <= 180 ? "0" : "1";

			var d = [
					"M", start.x.toFixed(4), start.y.toFixed(4),
					"A", radius.toFixed(4), radius.toFixed(4), 0, arcSweep, 0, end.x.toFixed(5), end.y.toFixed(4)
			].join(" ");

			return d;
	}
	to_radians(degrees)
	{
  	return degrees * Math.PI / 180;
	}
	to_degrees(radians)
	{
  	return radians * 180 / Math.PI;
	}
	midpoint(p1, p2)
	{
  	return {x: (p1.x+p2.x)/2, y:  (p1.y+p2.y)/2};
	}
	distance(p1, p2)
	{
		return Math.sqrt( Math.pow((p1.x-p2.x), 2) + Math.pow((p1.y-p2.y), 2));
	}
	get_line_angle(p1, p2)
	{
		var angle = this.to_degrees(Math.atan2(p1.y - p2.y, p1.x - p2.x));
		angle += 180;
		if (angle >= 360) angle -= 360;
		return angle;
	}
	get_all_intersections(e)
	{
		var Intersections = [];
		for (var i = 0; i < jetcad.Stack.length; i++)
		{
			var int = jetcad.MakerJS.path.intersection(jetcad.Stack[i], e);
			if (int)
			{
				Intersections.push(int);
			}
		}
		return Intersections;
	}
	get_intersection(l1, l2)
	{
		var int = this.MakerJS.path.intersection(l1, l2);
		if (int)
		{
			return int.intersectionPoints[0];
		}
		else
		{
			return int;
		}
	}
	rotate_line(p1, p2, origin, angle)
	{
		var line1 = new this.MakerJS.paths.Line([p1.x, p1.y], [p2.x, p2.y]);
		var paths = [this.MakerJS.path.rotate(line1, angle, [origin.x, origin.y])];
		return paths[0];
	}
	rotate_point(p, origin, angle)
	{
		var radians = (Math.PI / 180) * angle,
    cos = Math.cos(radians),
    sin = Math.sin(radians),
    nx = (cos * (p.x - origin.x)) + (sin * (p.y - origin.y)) + origin.x,
    ny = (cos * (p.y - origin.y)) - (sin * (p.x - origin.x)) + origin.y;
		return { x: nx, y: ny };
	}
	get_line_at_angle(origin, angle, distance)
	{
		var new_endpoint = { x: origin.x + (Math.abs(distance) * Math.cos(this.to_radians(angle))), y: origin.y + (Math.abs(distance) * Math.sin(this.to_radians(angle))) };
  	return { type: 'line', origin: [origin.x, origin.y], end: [new_endpoint.x, new_endpoint.y] };
	}
	mirror_point_about_line(p, p0, p1)
	{
     var dx, dy, a, b, x, y;
     dx = p1.x - p0.x;
     dy = p1.y - p0.y;
     a = (dx * dx - dy * dy) / (dx * dx + dy * dy);
     b = 2 * dx * dy / (dx * dx + dy * dy);
     x = a * (p.x - p0.x) + b * (p.y - p0.y) + p0.x;
     y = b * (p.x - p0.x) - a * (p.y - p0.y) + p0.y;
     return { x:x, y:y };
  }
	CircleCenterFromThreePoints(A,B,C)
	{
    var yDelta_a = B.y - A.y;
    var xDelta_a = B.x - A.x;
    var yDelta_b = C.y - B.y;
    var xDelta_b = C.x - B.x;

    var center = [];

    var aSlope = yDelta_a / xDelta_a;
    var bSlope = yDelta_b / xDelta_b;

    center.x = (aSlope*bSlope*(A.y - C.y) + bSlope*(A.x + B.x) - aSlope*(B.x+C.x) )/(2* (bSlope-aSlope) );
    center.y = -1*(center.x - (A.x+B.x)/2)/aSlope +  (A.y+B.y)/2;
    return center;


	}
	//Functions related to entity meta data
	metaSetStyle(x, style)
	{
		if (this.Stack[x].meta !== undefined)
		{
			this.Stack[x].meta.Style = style;
		}
	}
	metaGetColor(x)
	{
		if (this.Stack[x].meta !== undefined)
		{
			if (this.Stack[x].meta.Color === undefined)
			{
				this.Stack[x].meta.Color = "black";
				return this.Stack[x].meta.Color;
			}
			else
			{
				return this.Stack[x].meta.Color;
			}
		}
	}
	metaGetStyle(x)
	{
		if (this.Stack[x].meta !== undefined)
		{
			if (this.Stack[x].meta.Style === undefined)
			{
				this.Stack[x].meta.Style = "solid";
				return this.Stack[x].meta.Style;
			}
			else
			{
				return this.Stack[x].meta.Style;
			}
		}
	}
	metaIsMouseOver(x)
	{
		if (this.Stack[x].meta !== undefined)
		{
			return this.Stack[x].meta.MouseOver;
		}
		else
		{
			return false;
		}
	}
	metaMouseOver(x, value)
	{
		if (this.Stack[x].meta === undefined)
		{
			this.Stack[x].meta = {};
		}
		this.Stack[x].meta.MouseOver = value;
	}
	metaIsSelected(x)
	{
		if (this.Stack[x].meta !== undefined)
		{
			return this.Stack[x].meta.Selected;
		}
		else
		{
			return false;
		}
	}
	metaSelected(x, value)
	{
		if (this.Stack[x].meta === undefined)
		{
			this.Stack[x].meta = {};
		}
		this.Stack[x].meta.Selected = value;
	}
	metaSnapPoints(x, value)
	{
		if (this.Stack[x].meta === undefined)
		{
			this.Stack[x].meta = {};
		}
		this.Stack[x].meta.SnapPoints = value;
	}
	SetStatusText(text)
	{
		if (text == undefined)
		{
			$("#draw-info").html(" ");
			return;
		}
		$("#draw-info").html(text);
	}
	UpdateStatusText()
	{
		var found = false;
		for (var x = 0; x < this.Stack.length; x++)
		{
      if (this.metaIsSelected(x))
      {
				if (this.Stack[x].type == "circle")
				{
					this.SetStatusText(Number(this.Stack[x].radius * 2).toFixed(4) + " Inch Diameter");
					found = true;
					break;
				}
				if (this.Stack[x].type == "arc")
				{
					this.SetStatusText(Number(this.Stack[x].radius).toFixed(4) + " Inch Radius");
					found = true;
					break;
				}
				if (this.Stack[x].type == "line")
				{
					var text = this.distance({ x: this.Stack[x].origin[0], y: this.Stack[x].origin[1] }, { x: this.Stack[x].end[0], y: this.Stack[x].end[1] }).toFixed(4) + " Inch Length @ ";
					text += this.get_line_angle({ x: this.Stack[x].origin[0], y: this.Stack[x].origin[1] }, { x: this.Stack[x].end[0], y: this.Stack[x].end[1] }).toFixed(2) + " Degrees";
					this.SetStatusText(text);
					found = true;
					break;
				}
      }
    }
		if (found === false)
		{
			this.SetStatusText();
		}
	}
	//Svg Markup
	svgCircle(origin, radius, color, style)
	{
		if (style === "dashed")
		{
			return "<circle stroke-dasharray='5, 5' r='" + radius + "' cx='" + origin.x + "' cy='" + origin.y + "' stroke='" + color + "' stroke-width='" + this.View.line_width + "' vector-effect='non-scaling-stroke' fill='none'/>\n";
		}
		return "<circle r='" + radius + "' cx='" + origin.x + "' cy='" + origin.y + "' stroke='" + color + "' stroke-width='" + this.View.line_width + "' vector-effect='non-scaling-stroke' fill='none'/>\n";
	}
	svgCrosshair(origin, length, color)
	{
		var markup = "<line x1='" + origin.x + "' y1='" + origin.y + "' x2='" + (origin.x + length) + "' y2='" + origin.y + "' stroke='" + color + "' stroke-width='" + this.View.line_width + "' vector-effect='non-scaling-stroke' fill='none'/>\n";
		markup += "<line x1='" + origin.x + "' y1='" + origin.y + "' x2='" + (origin.x - length) + "' y2='" + origin.y + "' stroke='" + color + "' stroke-width='" + this.View.line_width + "' vector-effect='non-scaling-stroke' fill='none'/>\n";
		markup += "<line x1='" + origin.x + "' y1='" + (origin.y + length) + "' x2='" + origin.x + "' y2='" + origin.y + "' stroke='" + color + "' stroke-width='" + this.View.line_width + "' vector-effect='non-scaling-stroke' fill='none'/>\n";
		markup += "<line x1='" + origin.x + "' y1='" + (origin.y - length) + "' x2='" + origin.x + "' y2='" + origin.y + "' stroke='" + color + "' stroke-width='" + this.View.line_width + "' vector-effect='non-scaling-stroke' fill='none'/>\n";
		return markup;
	}
	svgLine(origin, end, color, style)
	{
		if (style === "dashed")
		{
			return "<line stroke-dasharray='5, 5' x1='" + origin.x + "' y1='" + origin.y  + "' x2='" + end.x + "' y2='" + end.y + "' stroke='" + color + "' stroke-width='" + this.View.line_width + "' vector-effect='non-scaling-stroke' fill='none'/>\n";
		}
		return "<line x1='" + origin.x + "' y1='" + origin.y  + "' x2='" + end.x + "' y2='" + end.y + "' stroke='" + color + "' stroke-width='" + this.View.line_width + "' vector-effect='non-scaling-stroke' fill='none'/>\n";
	}
	svgArc(origin, radius, startAngle, endAngle, color, style)
	{
		if (style === "dashed")
		{
			return "<path stroke-dasharray='5, 5' stroke='" + color + "' stroke-width='" + this.View.line_width + "' vector-effect='non-scaling-stroke' fill='none' d='" + this.describeArc(origin.x, origin.y, radius, startAngle, endAngle) + "'></path>";
		}
		return "<path stroke='" + color + "' stroke-width='" + this.View.line_width + "' vector-effect='non-scaling-stroke' fill='none' d='" + this.describeArc(origin.x, origin.y, radius, startAngle, endAngle) + "'></path>";
}
	//End svgSpline related functions
	svgGrid()
	{
		var text = "";
		text += "<defs>";
		text += '<pattern id="smallGrid" width="8" height="8" patternUnits="userSpaceOnUse">';
		text += '<path d="M 8 0 L 0 0 0 8" fill="none" stroke="gray" stroke-width="0.5"/>';
		text += '</pattern>';
		text += '<pattern id="grid" width="80" height="80" patternUnits="userSpaceOnUse">';
		text += '<rect width="80" height="80" fill="url(#smallGrid)"/>';
		text += '<path d="M 80 0 L 0 0 0 80" fill="none" stroke="gray" stroke-width="1"/>';
		text += '</pattern>';
   	text += '</defs>';
    text += '<rect width="100%" height="100%" fill="url(#grid)" />';
		return text;
	}
	//Export & Import Functions
	CubicBezierToLines(control_points, smoothing)
	{
		//console.log(control_points);
		var points = control_points.map(function(vec){
        return new THREE.Vector2(vec[0], vec[1]);
     });
		var interpolatedPoints = [];
		var curve = new THREE.SplineCurve(points);
    interpolatedPoints = curve.getPoints( Math.round( control_points.length * smoothing ) );

		var points = interpolatedPoints.map(function(vec){
        return vec.C;
     });
		return interpolatedPoints;
	}
	ImportDXF(data, ret_stack)
	{
		var imported_stack = [];
		var parser = new DxfParser();
		try {
				var dxf = parser.parseSync(data);

				console.log(dxf);
				for (var i = 0; i < dxf.entities.length; i++)
				{
					if (dxf.entities[i].type == "CIRCLE")
					{
						imported_stack.push({ type: "circle", origin: [dxf.entities[i].center.x, dxf.entities[i].center.y], radius: dxf.entities[i].radius });
					}
					else if (dxf.entities[i].type == "LINE")
					{
						imported_stack.push({ type: "line", origin: [dxf.entities[i].vertices[0].x, dxf.entities[i].vertices[0].y], end: [dxf.entities[i].vertices[1].x, dxf.entities[i].vertices[1].y] });
					}
					else if (dxf.entities[i].type == "ARC")
					{
						imported_stack.push({ type: "arc", origin: [dxf.entities[i].center.x, dxf.entities[i].center.y], startAngle: this.to_degrees(dxf.entities[i].startAngle), endAngle: this.to_degrees(dxf.entities[i].endAngle), radius: dxf.entities[i].radius });
					}
					else if (dxf.entities[i].type == "SPLINE") //Not the propper way to draw splines
					{
						if (dxf.entities[i].degreeOfSplineCurve == 3)
						{
							var control_points = [];
							for (var x = 0; x < dxf.entities[i].controlPoints.length; x++)
							{
								control_points.push(dxf.entities[i].controlPoints[x].x);
								control_points.push(dxf.entities[i].controlPoints[x].y);
								//control_points.push([dxf.entities[i].controlPoints[x].x, dxf.entities[i].controlPoints[x].y]);
							}
							var curve = new Bezier(control_points);
							try {
								var arcs = curve.arcs(0.1);
								for (var x = 0; x < arcs.length; x++)
								{
									imported_stack.push({ type: "arc", origin: [arcs[x].x, arcs[x].y], radius: arcs[x].r, startAngle: this.to_degrees(arcs[x].s), endAngle: this.to_degrees(arcs[x].e) });
								}
							}
							catch(err) {
									//When arc approximation fails, revert to line approximation!

									var points = curve.getLUT(100);
									for (var x = 1; x < points.length; x++)
									{
										var origin = points[x - 1];
										var end = points[x];

										imported_stack.push({ type: "line", origin: [origin.x, origin.y], end: [end.x, end.y] });
									}
									//console.log(err);
							}
						}
						else
						{
							notification.push("Only cubic splines are supported right now!");
						}
					}
					else if (dxf.entities[i].type == "LWPOLYLINE")
					{
						for (var x = 1; x < dxf.entities[i].vertices.length; x++)
						{
							var origin = dxf.entities[i].vertices[x - 1];
							var end = dxf.entities[i].vertices[x];
							if (dxf.entities[i].vertices[x - 1].bulge === undefined) imported_stack.push({ type: "line", origin: [origin.x, origin.y], end: [end.x, end.y] });
						}
						if (dxf.entities[i].vertices[dxf.entities[i].vertices.length - 1].bulge === undefined) imported_stack.push({ type: "line", origin: [dxf.entities[i].vertices[0].x, dxf.entities[i].vertices[0].y], end: [dxf.entities[i].vertices[dxf.entities[i].vertices.length - 1].x, dxf.entities[i].vertices[dxf.entities[i].vertices.length - 1].y] });

						//Sort Bulges out
						for (var x = 0; x < dxf.entities[i].vertices.length - 1; x++)
						{
							if (dxf.entities[i].vertices[x].bulge !== undefined)
							{
								var bulgeStart = dxf.entities[i].vertices[x];
								var bulgeEnd = dxf.entities[i].vertices[x + 1];
								var midpoint = this.midpoint(bulgeStart, bulgeEnd);
								var distance = this.distance(bulgeStart, midpoint);
								var sagitta = dxf.entities[i].vertices[x].bulge * distance;
								//console.log(sagitta);
								if (sagitta < 0)
								{
									var bulgeLine = this.get_line_at_angle(midpoint, this.get_line_angle(bulgeStart, bulgeEnd) + 90, sagitta);
									var arc_center = this.CircleCenterFromThreePoints(bulgeStart, {x: bulgeLine.end[0], y: bulgeLine.end[1] }, bulgeEnd);
									var arc_endAngle = this.get_line_angle(arc_center, bulgeStart);
									var arc_startAngle = this.get_line_angle(arc_center, bulgeEnd);
									imported_stack.push({ type: 'arc', origin: [arc_center.x, arc_center.y], radius: this.distance(arc_center, bulgeStart), startAngle: arc_startAngle, endAngle: arc_endAngle });
								}
								else
								{
									var bulgeLine = this.get_line_at_angle(midpoint, this.get_line_angle(bulgeStart, bulgeEnd) - 90, sagitta);
									var arc_center = this.CircleCenterFromThreePoints(bulgeStart, {x: bulgeLine.end[0], y: bulgeLine.end[1] }, bulgeEnd);
									var arc_startAngle = this.get_line_angle(arc_center, bulgeStart);
									var arc_endAngle = this.get_line_angle(arc_center, bulgeEnd);
									imported_stack.push({ type: 'arc', origin: [arc_center.x, arc_center.y], radius: this.distance(arc_center, bulgeStart), startAngle: arc_startAngle, endAngle: arc_endAngle });
								}

							}
						}
						if (dxf.entities[i].vertices[dxf.entities[i].vertices.length - 1].bulge !== undefined)
						{
							var bulgeStart = dxf.entities[i].vertices[0];
							var bulgeEnd = dxf.entities[i].vertices[dxf.entities[i].vertices.length - 1];
							var midpoint = this.midpoint(bulgeStart, bulgeEnd);
							var distance = this.distance(bulgeStart, midpoint);
							var sagitta = dxf.entities[i].vertices[dxf.entities[i].vertices.length - 1].bulge * distance;
							//console.log(sagitta);
							if (sagitta > 0)
							{
								var bulgeLine = this.get_line_at_angle(midpoint, this.get_line_angle(bulgeStart, bulgeEnd) + 90, sagitta);
								var arc_center = this.CircleCenterFromThreePoints(bulgeStart, {x: bulgeLine.end[0], y: bulgeLine.end[1] }, bulgeEnd);
								var arc_endAngle = this.get_line_angle(arc_center, bulgeStart);
								var arc_startAngle = this.get_line_angle(arc_center, bulgeEnd);
								imported_stack.push({ type: 'arc', origin: [arc_center.x, arc_center.y], radius: this.distance(arc_center, bulgeStart), startAngle: arc_startAngle, endAngle: arc_endAngle });
							}
							else
							{
								var bulgeLine = this.get_line_at_angle(midpoint, this.get_line_angle(bulgeStart, bulgeEnd) - 90, sagitta);
								var arc_center = this.CircleCenterFromThreePoints(bulgeStart, {x: bulgeLine.end[0], y: bulgeLine.end[1] }, bulgeEnd);
								var arc_startAngle = this.get_line_angle(arc_center, bulgeStart);
								var arc_endAngle = this.get_line_angle(arc_center, bulgeEnd);
								imported_stack.push({ type: 'arc', origin: [arc_center.x, arc_center.y], radius: this.distance(arc_center, bulgeStart), startAngle: arc_startAngle, endAngle: arc_endAngle });
							}
						}

					}
					else if (dxf.entities[i].type == "POLYLINE")
					{
						for (var x = 1; x < dxf.entities[i].vertices.length; x++)
						{
							var origin = dxf.entities[i].vertices[x - 1];
							var end = dxf.entities[i].vertices[x];

							imported_stack.push({ type: "line", origin: [origin.x, origin.y], end: [end.x, end.y] });
						}
					}
					else
					{
						notification.push(dxf.entities[i].type + " - Not Supported Yet");
					}
				}

		}catch(err) {
				return console.error(err.stack);
		}
		if (ret_stack)
		{
			console.log("Returning stack!");
			return imported_stack;
		}
		else
		{
			for (var i = 0; i < imported_stack.length; i++)
			{
				this.Stack.push(imported_stack[i]);
			}
		}
    jetcad_tools.RemoveDuplicates(); //This is a dirty fix, need to figure out what the real issue is!!
	}
  ImportSVG_NF(data, ret_stack) //Not finished, holds the key to importing svg with path data...
  {
    var svg  = jQuery.parseHTML(data);
    console.log(svg);
    var path = svg[0].innerHTML;
    console.log(path);
    //var data = path.getAttribute('d');
    var model_stack = jetcad.MakerJS.importer.fromSVGPathData(path);
    console.log(model_stack);
    var imported_stack = [];
    //First import lines
    for (var line_paths in model_stack.paths)
		{
      var path_object = model_stack.paths[line_paths];
			imported_stack.push(path_object);
		}
    //Import arc that are converted from Bezier lines
    for (var arc_paths in model_stack.models)
		{
      var model_object = model_stack.models[arc_paths];
      for (var arc in model_object.paths)
      {
        var arc_object = model_object.paths[arc];
        imported_stack.push(arc_object);
      }
		}
    if (ret_stack)
		{
			console.log("Returning stack!");
			return imported_stack;
		}
		else
		{
      jetcad.Stack = [];
			for (var i = 0; i < imported_stack.length; i++)
			{
				jetcad.Stack.push(imported_stack[i]);
			}
		}
  }
  ImportSVG(data, ret_stack) //Not finished, holds the key to importing svg with path data...
  {
    var svg  = jQuery.parseHTML(data);
    console.log(svg);
    var nodes = svg[0].children;
    console.log(nodes);

    var imported_stack = [];
    for (var x = 0; x < nodes.length; x++)
		{
      var type = nodes[x].tagName;
      if (type == "circle")
			{
				imported_stack.push({ type: "circle", origin: [parseFloat(nodes[x].attributes.cx.value), parseFloat(nodes[x].attributes.cy.value)], radius: parseFloat(nodes[x].attributes.r.value) });
			}
			else if (type == "line")
			{
				imported_stack.push({ type: "line", origin: [parseFloat(nodes[x].attributes.x1.value), parseFloat(nodes[x].attributes.y1.value)], end: [parseFloat(nodes[x].attributes.x2.value), parseFloat(nodes[x].attributes.y2.value) ] });
			}
		}

    if (ret_stack)
		{
			console.log("Returning stack!");
			return imported_stack;
		}
		else
		{
      jetcad.Stack = [];
			for (var i = 0; i < imported_stack.length; i++)
			{
				jetcad.Stack.push(imported_stack[i]);
			}
		}
  }
	ExportSVG()
	{
		return this.MakerJS.exporter.toSVG(this.Model);
	}
	ExportDXF()
	{
    var DxfModel = {};
    DxfModel.paths = [];
    for (var x = 0; x < jetcad.Stack.length; x++)
    {
      var e = JSON.parse(JSON.stringify(this.Stack[x]));
      if (e.type == "line")
      {
        e.origin[0] = e.origin[0].toFixed(6);
        e.origin[1] = e.origin[1].toFixed(6);
        e.end[0] = e.end[0].toFixed(6);
        e.end[1] = e.end[1].toFixed(6);
        DxfModel.paths.push(e);
      }
      if (e.type == "arc" && e.startAngle.toFixed(6) != e.endAngle.toFixed(6))
      {
        e.origin[0] = e.origin[0].toFixed(6);
        e.origin[1] = e.origin[1].toFixed(6);
        e.radius = e.radius.toFixed(6);
        e.startAngle = e.startAngle.toFixed(6);
        e.endAngle = e.endAngle.toFixed(6);
        DxfModel.paths.push(e);
      }
      if (e.type == "circle")
      {
        e.origin[0] = e.origin[0].toFixed(6);
        e.origin[1] = e.origin[1].toFixed(6);
        e.radius = e.radius.toFixed(6);
        DxfModel.paths.push(e);
      }
    }
		return this.MakerJS.exporter.toDXF(DxfModel);
		/*var Drawing = require('Drawing');
		var draw = new Drawing();
		draw.addLayer('0', Drawing.ACI.BLACK, 'CONTINUOUS');
		draw.setActiveLayer('0');

		for (var i = 0; i < this.Stack.length; i++)
		{
			if (this.Stack[i].type == "line")
			{
				draw.drawLine(this.Stack[i].origin[0], this.Stack[i].origin[1], this.Stack[i].end[0], this.Stack[i].end[1]);
			}
			if (this.Stack[i].type == "arc")
			{
				draw.drawArc(this.Stack[i].origin[0], this.Stack[i].origin[1], this.Stack[i].radius, this.Stack[i].startAngle, this.Stack[i].endAngle);
			}
			if (this.Stack[i].type == "circle")
			{
				draw.drawCircle(this.Stack[i].origin[0], this.Stack[i].origin[1], this.Stack[i].radius);
			}
		}
		return draw.toDxfString();
	}*/
	}
}
