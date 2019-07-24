class JetCad_Tools{

  constructor(options)
  {
    jetcad.HotKeys = [
			{ key: "ctrl-a", run: function(context){ jetcad_tools.SelectAll() } },
			{ key: "ctrl-s", run: function(context){ jetcad_tools.SaveDrawing() } },
			{ key: "ctrl-c", run: function(context){ jetcad_tools.CopySelected() } },
			{ key: "ctrl-v", run: function(context){ jetcad_tools.PasteSelected() } },
			{ key: "ctrl-z", run: function(context){ jetcad_tools.Undo() } },
      { key: "ctrl-d", run: function(context){ jetcad_tools.DeleteSelected() } },
			{ key: "l", run: function(context){ jetcad_tools.DrawLine() } },
			{ key: "r", run: function(context){ jetcad_tools.DrawRectangle() } },
			{ key: "c", run: function(context){ jetcad_tools.DrawCircle() } },
      { key: "t", run: function(context){ jetcad_tools.Trim() } },
			{ key: "f", run: function(context){ jetcad_tools.Fillet() } },
			{ key: "o", run: function(context){ jetcad_tools.Offset() } },
			{ key: "m", run: function(context){ jetcad_tools.Move() } },
			{ key: "x", run: function(context){ jetcad_tools.ToggleConstructionStyle() } },
			{ key: "s", run: function(context){ jetcad_tools.SearchToolBox() } },
			{ key: "p", run: function(context){ jetcad_tools.ParallelLine() } },
      { key: "Delete", run: function(context){ jetcad_tools.DeleteSelected() } },
			//{ key: "Backspace", run: function(context){ jetcad_tools.DeleteSelected() } },
			{ key: "Escape", run: function(context){ jetcad_tools.Escape() } },
			{ key: " ", run: function(context){ jetcad_tools.Space() } },
		];
		jetcad.SearchTools = [
			{ tool: "rotate", run: function(context){ jetcad_tools.Rotate() } },
			{ tool: "circular_pattern", run: function(context){ jetcad_tools.Circular_Pattern() } },
			{ tool: "scale", run: function(context){ jetcad_tools.Scale() } },
			{ tool: "mirror", run: function(context){ jetcad_tools.Mirror() } },
			{ tool: "color", run: function(context){ jetcad_tools.Color() } },
			{ tool: "layer", run: function(context){ jetcad_tools.Layer() } },
			{ tool: "text", run: function(context){ jetcad_tools.DrawText() } },
			{ tool: "path_text", run: function(context){ jetcad_tools.DrawTextAlongPath() } }, //Not finished yet
			{ tool: "clean", run: function(context){ jetcad_tools.CleanDrawing() } },
			{ tool: "slot", run: function(context){ jetcad_tools.Slot() } },
			{ tool: "chamfer", run: function(context){ jetcad_tools.Chamfer() } },
			{ tool: "fit", run: function(context){ jetcad.zoomExtents() } },
			{ tool: "invert_delete", run: function(context){ jetcad_tools.InvertDelete() } },
      { tool: "remove_duplicates", run: function(context){ jetcad_tools.RemoveDuplicates() } },
			//Experimental
      { tool: "lathe_mode", run: function(context){ jetcad.LatheMode = true } },
			{ tool: "jet_operation", run: function(context){ jetcad_tools.JetOperation() } },
			{ tool: "post", run: function(context){ jetcad_tools.PostToolpaths() } },
      { tool: "sim", run: function(context){ jetcad_tools.PostToolpaths(false) } },
      { tool: "copy", run: function(context){ jetcad_tools.CopySelected() } },
      { tool: "paste", run: function(context){ jetcad_tools.PasteSelected() } },
		];
		jetcad.Callbacks.DoubleClick = function()
		{
			//console.log("Double Click!");
			var Selected = jetcad_tools.getSelected();
			var chains = jetcad.MakerJS.model.findChains(jetcad.Model);

			var selected_chain = null;
			for (var x = 0; x < chains.length; x++)
			{
				for (var i = 0; i < chains[x].links.length; i++)
				{
					var entity_index = Number(chains[x].links[i].walkedPath.route[1]);
					//console.log("Index: " + entity_index);
					if (jetcad.metaIsSelected(entity_index))
					{
						selected_chain = chains[x].links;
						break;
					}

				}
			}
			//console.log("Selected chain: ");
			//console.log(selected_chain);
			if (selected_chain == null)
			{
				return;
			}
			for (var x = 0; x < selected_chain.length; x++)
			{
				var index = Number(selected_chain[x].walkedPath.route[1]);
				jetcad.metaSelected(index, true);
				jetcad.Stack[index].meta.SelectedTimeStamp = x;
			}
			jetcad.render(true);
		}
  }
  PostToolpaths(d = true)
  {
    $.getScript("/xmotion_post.js", function() {
        var StaticStack = [];
        PostHeader();
        for (var op_name in jetcad.ToolPaths)
        {
          var operation = jetcad.ToolPaths[op_name];
          PostComment(op_name + ", Tool Diamter: " + operation.tool_diameter);
          if (operation.type == "jet")
          {
            for (var i = 0; i < operation.path.length; i++)
            {
              var path = operation.path[i];
              var lead_in_start_point = [];
              var lead_out_end_point = []
              //Lead-in
              if (path.length > 2)
              {
                var line_angle = jetcad.get_line_angle({ x: path[0][0] , y: path[0][1] }, { x: path[1][0], y: path[1][1] });
                var lead_in = jetcad.get_line_at_angle({ x: path[0][0] , y: path[0][1] }, line_angle + 100, operation.lead_in);
                lead_in_start_point = lead_in.end;
                StaticStack.push({ type: "arrow", origin: lead_in_start_point, end: path[0] });
              }
              for (var x = 1; x < path.length; x++)
              {
                StaticStack.push({ type: "arrow", origin: path[x -1], end: path[x] });
              }
              StaticStack.push({ type: "arrow", origin: path[path.length - 1], end: path[0] });
              //Lead-Out
              if (path.length > 2)
              {
                var line_angle = jetcad.get_line_angle({ x: path[path.length - 2][0] , y: path[path.length - 2][1] }, { x: path[path.length - 1][0], y: path[path.length - 1][1] });
                var lead_out = jetcad.get_line_at_angle({ x: path[path.length - 1][0], y: path[path.length - 1][1] }, line_angle + 80, operation.lead_in);
                lead_out_end_point = lead_out.end;
                StaticStack.push({ type: "arrow", origin: path[0], end: lead_out_end_point });
              }

              PostRapid({ z: operation.retract_height });
              PostRapid({ x: lead_in_start_point[0], y: lead_in_start_point[1] }); //Go to our start point
              PostRaw("o<touchoff> call [" + operation.pierce_height + "] [" + operation.pierce_delay + "] [" + operation.cut_height + "]");
              PostDwell(operation.pierce_delay);

              for (var x = 0; x < path.length; x++)
              {
                //StaticStack.push({type: "circle", origin: path[x], radius: 0.1});
                PostFeed({ x: path[x][0], y: path[x][1] }, operation.feedrate);
              }
              PostFeed({ x: path[0][0], y: path[0][1] }, operation.feedrate); //Return to start point
              PostFeed({ x: lead_out_end_point[0], y: lead_out_end_point[1] }, operation.feedrate); //Lead-Out
              PostRaw("M5");
              PostDwell(operation.finish_delay);
              PostRapid({ z: operation.retract_height });


            }

          }
        }
        PostFooter();
        jetcad.StaticStack = jetcad.StaticRender(StaticStack);
        if (d) download(jetcad.DrawingName + '.ngc', Gcode);
    });
  }
	PostToolpaths_old()
	{
		$.getScript("/xmotion_post.js", function() {
			var StaticStack = [];
      PostHeader();
			for (var op_name in jetcad.ToolPaths)
			{
				var operation = jetcad.ToolPaths[op_name];
				PostComment(op_name + ", Tool Diamter: " + operation.tool_diamter);
				if (operation.type == "jet")
				{
					for (var x = 0; x < operation.path.length; x++)
					{
						if (operation.path[x].type == "line")
						{
							if (x == 0) //First Move
							{
								PostRapid({ z: operation.retract_height });
								PostRapid({ x: operation.path[x].origin[0], y: operation.path[x].origin[1] }); //Go to our start point
								PostRaw("o<touchoff> call [" + operation.pierce_height + "] [" + operation.pierce_delay + "] [" + operation.cut_height + "]");
								PostDwell(operation.pierce_delay);
							}
							else if (x == operation.path.length - 1) //Last Move
							{
								PostRaw("M5");
								PostDwell(operation.finish_delay);
								PostRapid({ z: operation.retract_height });
							}
							else
							{
								PostFeed({ x: operation.path[x].end[0], y: operation.path[x].end[1] }, operation.feedrate);
							}
              StaticStack.push({type: "circle", origin: operation.path[x].end, radius: 0.1});
              console.log("Plotting Point");
						}
						if (operation.path[x].type == "arc")
						{
							var start_line = jetcad.get_line_at_angle({ x: operation.path[x].origin[0], y: operation.path[x].origin[1] }, operation.path[x].startAngle, operation.path[x].radius);
							var end_line = jetcad.get_line_at_angle({ x: operation.path[x].origin[0], y: operation.path[x].origin[1] }, operation.path[x].endAngle, operation.path[x].radius);
							//console.log(end_line);
							if (x == 0) //First Move
							{
								PostRapid({ z: operation.retract_height });

								PostRapid({ x: start_line.end[0], y: start_line.end[1] }); //Go to our start point
								PostRaw("o<touchoff> call [" + operation.pierce_height + "] [" + operation.pierce_delay + "] [" + operation.cut_height + "]");
							}
							else if (x == operation.path.length - 1) //Last Move
							{
								PostRaw("M5");
								PostDwell(operation.finish_delay);
								PostRapid({ z: operation.retract_height });
							}
							else
							{
								//function(arc, direction, feedrate)
								PostArc({ x: end_line.end[0], y: end_line.end[1], radius:  operation.path[x].radius }, "clockwise", operation.feedrate);
							}
						}
					}
				}
			}
			PostFooter();
			//console.log(Gcode);
      jetcad.StaticStack = jetcad.StaticRender(StaticStack);
		});
	}
	JetOperation()
	{
		jetcad.DisableHotkeys = true;
		var markup = "";
		markup += "<b>Opperation Name:</b><input id='name_text' type='text' value='Cut'><br>";
		markup += "<b>Offset Side:</b><input id='offset_text' type='text' value='Outside'><br>";
		markup += "<b>Tool Diameter:</b><input id='diameter_text' type='text' value='0.030'><br>";
		markup += "<b>Feedrate:</b><input id='feedrate_text' type='text' value='70'><br>";
		markup += "<input id='tool_ok' type='button' value='OK'><br>";
		$("#tool-settings").show();
		$("#tool-settings").html(markup);

		var name_text = document.getElementById("name_text");
		//var layer_text = document.getElementById("layer_text");
		var offset_text = document.getElementById("offset_text");
		var diameter_text = document.getElementById("diameter_text");
		var feedrate_text = document.getElementById("feedrate_text");
		var tool_ok = document.getElementById("tool_ok");

		tool_ok.onclick = function(){
			jetcad.RegisterUndo();

			var offset = Math.abs(Number(diameter_text.value));
			console.log(offset);
			if (offset_text.value == "Inside") offset *= -1;

			jetcad.ToolPaths[name_text.value] = {};
			jetcad.ToolPaths[name_text.value].feedrate = Number(feedrate_text.value);
			jetcad.ToolPaths[name_text.value].type = "jet";
			jetcad.ToolPaths[name_text.value].retract_height = 0.5;
			jetcad.ToolPaths[name_text.value].pierce_delay = 0.5;
			jetcad.ToolPaths[name_text.value].finish_delay = 1;
			jetcad.ToolPaths[name_text.value].pierce_height = 0.2;
			jetcad.ToolPaths[name_text.value].cut_height = 0.175;
      jetcad.ToolPaths[name_text.value].lead_in = 0.0625;
      jetcad.ToolPaths[name_text.value].lead_out = 0.0625;
			jetcad.ToolPaths[name_text.value].tool_diameter = offset;

      var offset_chain = jetcad.offset_selected(offset / 2);

      var Model = {};
      Model.paths = [];

      for (var x = 0; x < offset_chain.length; x++)
      {
        Model.paths.push(offset_chain[x]);
      }
      var chains = jetcad.MakerJS.model.findChains(Model);
      jetcad.ToolPaths[name_text.value].path = [];
      for (var x = 0; x < chains.length; x++)
      {
        if (offset > 0) //We are an outside contour
        {
          if (!jetcad.MakerJS.measure.isChainClockwise(chains[x]))
          {
            console.log("Chain is count-clockwise, reversing!");
            chains[x] = jetcad.MakerJS.chain.reverse(chains[x]);
          }
          var keyPoints = jetcad.MakerJS.chain.toKeyPoints(chains[x], 0.010);
          jetcad.ToolPaths[name_text.value].path[x] = keyPoints;
        }
        else //We are inside contour
        {
          if (jetcad.MakerJS.measure.isChainClockwise(chains[x]))
          {
            console.log("Chain is clockwise, reversing!");
            chains[x] = jetcad.MakerJS.chain.reverse(chains[x]);
          }
          var keyPoints = jetcad.MakerJS.chain.toKeyPoints(chains[x], 0.010);
          jetcad.ToolPaths[name_text.value].path[x] = keyPoints;
        }

      }


			jetcad_tools.Escape();
		};
	}
	Slot()
	{
		jetcad.input_box("slot", "0", "slot diameter", { x: jetcad.ClientMouse.x + 20, y: jetcad.ClientMouse.y + 20 });
		jetcad.Callbacks.onMouse = function(){
					var slot = $("#slot").val();
					jetcad.clear_input_boxes();
					jetcad.input_box("slot", slot, "", { x: jetcad.ClientMouse.x + 20, y: jetcad.ClientMouse.y + 20 });
					jetcad.DisableHotkeys = true;
		};
		jetcad.Callbacks.onTab = function(){
					$("#slot").focus();
					$("#slot").select();
		};
		jetcad.Callbacks.onEnter = function(){
			jetcad.RegisterUndo();

			var Selected = jetcad_tools.getSelected(); //Get Selected in order
			var distance = jetcad.get_input_box_value("slot");

			var model = {};
			model.models = {};
			model.models.drawing = {};
			model.models.drawing.paths = [];
			for (var i = 0; i < Selected.length; i++)
			{
				model.models.drawing.paths.push(Selected[i]);
			}
			//console.log(model);
			//var offset = jetcad.MakerJS.path.expand(model, distance, 2);
			var offset = jetcad.MakerJS.model.expandPaths(model, (distance / 2), 0);
			console.log(offset);
			for (var model_number in offset.models.expansions.models)
			{
				var expanded_model = offset.models.expansions.models[model_number];
				for (var path_name in expanded_model.paths)
				{
					var offset_path = expanded_model.paths[path_name];
					jetcad.Stack.push(offset_path);
				}
			}
			for (var model_number in offset.models.caps.models)
			{
				var expanded_model = offset.models.caps.models[model_number];
				for (var model_name in expanded_model.models)
				{
					var offset_path = expanded_model.models[model_name];
					for (var path_name in offset_path.paths)
					{
						var offset_path = offset_path.paths[path_name];
						jetcad.Stack.push(offset_path);
					}
				}
			}
			jetcad_tools.Escape();
		};
	}
	ParallelLine()
	{
		var Selected = jetcad_tools.getSelected();
		if (Selected < 1)
		{
			jetcad.SetStatusText("You must first select a line!");
			jetcad_tools.Escape();
			return;
		}
		jetcad.input_box("distance", "0", "distance", { x: jetcad.ClientMouse.x + 20, y: jetcad.ClientMouse.y + 20 });
		jetcad.NextClick = [ function(){
			jetcad.RegisterUndo();
			jetcad.Stack.push(jetcad.ToolPoints[0]);
			jetcad_tools.Escape();
		} ];
		jetcad.Callbacks.onTab = function(){
					$("#distance").focus();
					$("#distance").select();
		};
		jetcad.Callbacks.onEnter = function(){
			jetcad.RegisterUndo();

			var base_line = jetcad.ToolPoints[2];
			var base_line_length = jetcad.distance({x: base_line.origin[0], y: base_line.origin[1]}, {x: base_line.end[0], y: base_line.end[1]});
			var base_line_midpoint = jetcad.midpoint({ x: base_line.origin[0], y: base_line.origin[1]}, { x: base_line.end[0], y: base_line.end[1]});
			var base_line_angle = jetcad.get_line_angle({ x: base_line.origin[0], y: base_line.origin[1]}, { x: base_line.end[0], y: base_line.end[1]});
			var square_line_from_baseline_midpoint = jetcad.get_line_at_angle(base_line_midpoint, base_line_angle + jetcad.ToolPoints[1], jetcad.get_input_box_value("distance"));
			var parallel_first_half = jetcad.get_line_at_angle({x: square_line_from_baseline_midpoint.end[0], y: square_line_from_baseline_midpoint.end[1]}, (base_line_angle + jetcad.ToolPoints[1]) + 90, base_line_length / 2);
			var parallel_second_half = jetcad.get_line_at_angle({x: square_line_from_baseline_midpoint.end[0], y: square_line_from_baseline_midpoint.end[1]}, (base_line_angle + jetcad.ToolPoints[1]) + 270, base_line_length / 2);
			var parallel_line = { type: "line", origin: parallel_first_half.end, end: parallel_second_half.end };
			jetcad.Stack.push(parallel_line);

			jetcad_tools.Escape();
		};
		jetcad.Callbacks.onMouse = function(){
					var distance;
					for (var i = 0; i < Selected.length; i++)
					{
						if (Selected[i].type == "line")
						{
              jetcad.SetStatusText("Click to create line!");
							var StaticStack = [];
							var base_line = Selected[i];
							var base_line_length = jetcad.distance({x: base_line.origin[0], y: base_line.origin[1]}, {x: base_line.end[0], y: base_line.end[1]});
							var base_line_midpoint = jetcad.midpoint({ x: base_line.origin[0], y: base_line.origin[1]}, { x: base_line.end[0], y: base_line.end[1]});
							var base_line_angle = jetcad.get_line_angle({ x: base_line.origin[0], y: base_line.origin[1]}, { x: base_line.end[0], y: base_line.end[1]});
							var midpoint_to_mouse_distance = jetcad.distance(base_line_midpoint, jetcad.SnappedMouse);
							//Note,
							var parallel_90_intersection_line = jetcad.get_line_at_angle(jetcad.SnappedMouse, base_line_angle + 90, (midpoint_to_mouse_distance * 2)); //Distance is this way because we need to ensure an intersection point!
							var parallel_270_intersection_line = jetcad.get_line_at_angle(jetcad.SnappedMouse, base_line_angle + 270, (midpoint_to_mouse_distance * 2));

							var square_line_angle = 270;
							var intersection = jetcad.get_intersection(parallel_90_intersection_line, base_line);
							if (intersection === null)
							{
								square_line_angle = 90;
								intersection = jetcad.get_intersection(parallel_270_intersection_line, base_line);
								if (intersection === null)
								{
									//We are outside of our parallel box...
                  jetcad.SetStatusText("You are outside parallel box!");
									break;
								}
							}
							var square_snap_line = { type: "line", origin: intersection, end: [jetcad.SnappedMouse.x, jetcad.SnappedMouse.y] };
							//StaticStack.push(square_snap_line);
							var square_line_distance = jetcad.distance({ x: square_snap_line.origin[0], y: square_snap_line.origin[1]}, { x: square_snap_line.end[0], y: square_snap_line.end[1]});
							distance = square_line_distance.toFixed(4);
							var square_line_from_baseline_midpoint = jetcad.get_line_at_angle(base_line_midpoint, base_line_angle + square_line_angle, square_line_distance);

							var parallel_first_half = jetcad.get_line_at_angle({x: square_line_from_baseline_midpoint.end[0], y: square_line_from_baseline_midpoint.end[1]}, (base_line_angle + square_line_angle) + 90, base_line_length / 2);
							var parallel_second_half = jetcad.get_line_at_angle({x: square_line_from_baseline_midpoint.end[0], y: square_line_from_baseline_midpoint.end[1]}, (base_line_angle + square_line_angle) + 270, base_line_length / 2);

							//StaticStack.push(square_line_from_baseline_midpoint);
							//StaticStack.push(parallel_first_half);
							//StaticStack.push(parallel_second_half);

							var parallel_line = { type: "line", origin: parallel_first_half.end, end: parallel_second_half.end };
							jetcad.ToolPoints[0] = parallel_line; //A click end to this tool places the line where the live entity sits.
							jetcad.ToolPoints[1] = square_line_angle;
							jetcad.ToolPoints[2] = base_line;

							StaticStack.push(parallel_line);

							jetcad.StaticStack = jetcad.StaticRender(StaticStack);
							jetcad.render(true);
						}
					}
					//distance = $("#distance").val();
					jetcad.clear_input_boxes();
					jetcad.input_box("distance", distance, "", { x: jetcad.ClientMouse.x + 20, y: jetcad.ClientMouse.y + 20 });
					jetcad.DisableHotkeys = true;



		};
	}
	CleanDrawing()
	{
		jetcad.RegisterUndo();
		for (var x = 0; x < jetcad.Stack.length; x++)
		{
			if (jetcad.metaGetStyle(x) == "dashed")
			{
				jetcad.metaSelected(x, true);
			}
		}
		jetcad_tools.DeleteSelected();
		jetcad_tools.Escape();
	}
	InvertDelete()
	{
		jetcad.RegisterUndo();
		for (var x = 0; x < jetcad.Stack.length; x++)
		{
			jetcad.metaSelected(x, !jetcad.metaIsSelected(x));
		}
		jetcad_tools.DeleteSelected();
		jetcad_tools.Escape();
	}
  RemoveDuplicates()
  {
    jetcad.RegisterUndo();
		for (var x = 0; x < jetcad.Stack.length; x++)
		{
      for (var y = 0; y < jetcad.Stack.length; y++)
		  {
        if (jetcad.Stack[x].type == "line" && jetcad.Stack[y].type == "line" && x != y && jetcad.metaIsSelected(x) !== true)
        {
          //Lines that are the same orientation
          if (jetcad.Stack[x].origin[0].toFixed(4) == jetcad.Stack[y].origin[0].toFixed(4) &&
              jetcad.Stack[x].origin[1].toFixed(4) == jetcad.Stack[y].origin[1].toFixed(4) &&
              jetcad.Stack[x].end[0].toFixed(4) == jetcad.Stack[y].end[0].toFixed(4) &&
              jetcad.Stack[x].end[1].toFixed(4) == jetcad.Stack[y].end[1].toFixed(4) ||

              jetcad.Stack[x].origin[0].toFixed(4) == jetcad.Stack[y].end[0].toFixed(4) &&
              jetcad.Stack[x].origin[1].toFixed(4) == jetcad.Stack[y].end[1].toFixed(4) &&
              jetcad.Stack[x].end[0].toFixed(4) == jetcad.Stack[y].origin[0].toFixed(4) &&
              jetcad.Stack[x].end[1].toFixed(4) == jetcad.Stack[y].origin[1].toFixed(4)
             )
          {
            //console.log("Found Duplicate Line!");
            jetcad.metaSelected(y, true);

          }
        }
        if (jetcad.Stack[x].type == "arc" && jetcad.Stack[y].type == "arc" && x != y && jetcad.metaIsSelected(x) !== true)
        {
          //Lines that are the same orientation
          if (jetcad.Stack[x].origin[0].toFixed(4) == jetcad.Stack[y].origin[0].toFixed(4) &&
              jetcad.Stack[x].origin[1].toFixed(4) == jetcad.Stack[y].origin[1].toFixed(4) &&
              jetcad.Stack[x].radius.toFixed(4) == jetcad.Stack[y].radius.toFixed(4) &&
              jetcad.Stack[x].startAngle.toFixed(4) == jetcad.Stack[y].startAngle.toFixed(4) &&
              jetcad.Stack[x].endAngle.toFixed(4) == jetcad.Stack[y].endAngle.toFixed(4)
             )
          {
            //console.log("Found Duplicate Arc!");
            jetcad.metaSelected(y, true);

          }

        }
        if (jetcad.Stack[x].type == "circle" && jetcad.Stack[y].type == "circle" && x != y && jetcad.metaIsSelected(x) !== true)
        {
          //Lines that are the same orientation
          if (jetcad.Stack[x].origin[0].toFixed(4) == jetcad.Stack[y].origin[0].toFixed(4) &&
              jetcad.Stack[x].origin[1].toFixed(4) == jetcad.Stack[y].origin[1].toFixed(4) &&
              jetcad.Stack[x].radius.toFixed(4) == jetcad.Stack[y].radius.toFixed(4)
             )
          {
            console.log("Found Duplicate Circle!");
            jetcad.metaSelected(y, true);

          }

        }
      }
		}
    jetcad_tools.DeleteSelected();
		jetcad_tools.Escape();
  }
	DrawTextAlongPath()
	{
		jetcad.DisableHotkeys = true;
		var markup = "<b>Font:</b><select id='font_selection'></select><br>";
		markup += "<b>Style:</b><select id='style_selection'></select><br>";
		markup += "<b>Size:</b><input value='10' id='font_size' type='number'><br>";
		markup += "<b>Text:</b><input id='tool_text' type='text'><br>";
		markup += "<input id='tool_ok' type='button' value='OK'><br>";
		$("#tool-settings").show();
		$("#tool-settings").html(markup);

		var font_selection = document.getElementById("font_selection");
		var text_input = document.getElementById("tool_text");
		var style_selection = document.getElementById("style_selection");
		var font_size = document.getElementById("font_size");
		var ok_button = document.getElementById("tool_ok");
		jetcad.ToolPoints[0] = {x: 0, y: 0}; //Text offset
		jetcad.ToolPoints[1] = jetcad.Mouse;
		jetcad.Callbacks.onMouse = function(){
			if (jetcad.isMouseDown)
			{
				var x_inc = jetcad.Mouse.x - jetcad.ToolPoints[1].x;
				var y_inc = jetcad.Mouse.y - jetcad.ToolPoints[1].y;
				if (x_inc < 20 && y_inc < 20)
				{
					jetcad.ToolPoints[0].x += x_inc;
					jetcad.ToolPoints[0].y += y_inc;
					//console.log(jetcad.ToolPoints[0]);
				}
			}
			jetcad.ToolPoints[1] = jetcad.Mouse;
			if (jetcad.ToolPoints[2] !== undefined)
			{
				jetcad.StaticStack = jetcad.StaticRender(jetcad.ShiftStack(jetcad.ToolPoints[2], jetcad.ToolPoints[0]));
				jetcad.render(true);
			}
		};
		ok_button.onclick = function(){
			jetcad.RegisterUndo();
			var text_stack = jetcad.ShiftStack(jetcad.ToolPoints[2], jetcad.ToolPoints[0]);
			for (var i = 0; i < text_stack.length; i++)
			{
				jetcad.Stack.push(text_stack[i]);
			}
			jetcad_tools.Escape();
		};
		var option;
		for (var family in fonts)
		{
			option = document.createElement('option');
			option.setAttribute('value', family);
			option.appendChild(document.createTextNode(family));
			font_selection.appendChild(option);
		}
		font_selection.onchange = function(){
			style_selection.innerHTML = '';
			var option;
			for (var style in fonts[font_selection.value])
			{
				option = document.createElement('option');
				option.setAttribute('value', style);
				option.appendChild(document.createTextNode(style));
				style_selection.appendChild(option);
			}
			text_input.onchange();
		};
		font_size.onchange = function()
		{
			text_input.onchange();
		};
		style_selection.onchange = function()
		{
			text_input.onchange();
		};
		text_input.onchange = function(){
			opentype.load(fonts[font_selection.value][style_selection.value], function(err, font) {
					if (err) {
							 notification.push('Font could not be loaded: ' + err);
					} else {

							var textModel = new jetcad.MakerJS.models.Text(font, text_input.value, Number(font_size.value));
							var dxf = jetcad.MakerJS.exporter.toDXF(textModel);
							jetcad.ToolPoints[2] = jetcad.ImportDXF(dxf, true);
							jetcad.Callbacks.onMouse();
							//console.log(text);
					}
			});
		};

		font_selection.onchange(); //Populate font style on first load

	}
	DrawText()
	{
		jetcad.DisableHotkeys = true;
		var markup = "<b>Font:</b><select id='font_selection'></select><br>";
		markup += "<b>Style:</b><select id='style_selection'></select><br>";
		markup += "<b>Size:</b><input value='10' id='font_size' type='number'><br>";
		markup += "<b>Text:</b><input id='tool_text' type='text'><br>";
		markup += "<input id='tool_ok' type='button' value='OK'><br>";
		$("#tool-settings").show();
		$("#tool-settings").html(markup);

		var font_selection = document.getElementById("font_selection");
		var text_input = document.getElementById("tool_text");
		var style_selection = document.getElementById("style_selection");
		var font_size = document.getElementById("font_size");
		var ok_button = document.getElementById("tool_ok");
		jetcad.ToolPoints[0] = {x: 0, y: 0}; //Text offset
		jetcad.ToolPoints[1] = jetcad.Mouse;
		jetcad.Callbacks.onMouse = function(){
			if (jetcad.isMouseDown)
			{
				var x_inc = jetcad.Mouse.x - jetcad.ToolPoints[1].x;
				var y_inc = jetcad.Mouse.y - jetcad.ToolPoints[1].y;
				if (x_inc < 20 && y_inc < 20)
				{
					jetcad.ToolPoints[0].x += x_inc;
					jetcad.ToolPoints[0].y += y_inc;
					//console.log(jetcad.ToolPoints[0]);
				}
			}
			jetcad.ToolPoints[1] = jetcad.Mouse;
			if (jetcad.ToolPoints[2] !== undefined)
			{
				jetcad.StaticStack = jetcad.StaticRender(jetcad.ShiftStack(jetcad.ToolPoints[2], jetcad.ToolPoints[0]));
				jetcad.render(true);
			}
		};
		ok_button.onclick = function(){
			jetcad.RegisterUndo();
			var text_stack = jetcad.ShiftStack(jetcad.ToolPoints[2], jetcad.ToolPoints[0]);
			for (var i = 0; i < text_stack.length; i++)
			{
				jetcad.Stack.push(text_stack[i]);
			}
			jetcad_tools.Escape();
		};
		var option;
		for (var family in fonts)
		{
			option = document.createElement('option');
			option.setAttribute('value', family);
			option.appendChild(document.createTextNode(family));
			font_selection.appendChild(option);
		}
		font_selection.onchange = function(){
			style_selection.innerHTML = '';
			var option;
			for (var style in fonts[font_selection.value])
			{
				option = document.createElement('option');
				option.setAttribute('value', style);
				option.appendChild(document.createTextNode(style));
				style_selection.appendChild(option);
			}
			text_input.onchange();
		};
		font_size.onchange = function()
		{
			text_input.onchange();
		};
		style_selection.onchange = function()
		{
			text_input.onchange();
		};
		text_input.onchange = function(){
			opentype.load(fonts[font_selection.value][style_selection.value], function(err, font) {
					if (err) {
							 notification.push('Font could not be loaded: ' + err);
					} else {

							var textModel = new jetcad.MakerJS.models.Text(font, text_input.value, Number(font_size.value));
							var dxf = jetcad.MakerJS.exporter.toDXF(textModel);
							jetcad.ToolPoints[2] = jetcad.ImportDXF(dxf, true);
							jetcad.Callbacks.onMouse();
							//console.log(text);
					}
			});
		};

		font_selection.onchange(); //Populate font style on first load

	}
	Layer()
	{
		var selected = [];
		for (var x = 0; x < jetcad.Stack.length; x++)
		{
			if (jetcad.metaIsSelected(x))
			{
				selected.push(x);
			}
		}
		jetcad.ToolPoints[0] = selected;
		jetcad.DisableHotkeys = true;
		jetcad.input_box("layer", "0", "", { x: jetcad.ClientMouse.x + 20, y: jetcad.ClientMouse.y + 20 });
		jetcad.Callbacks.onMouse = function(){
					var layer = $("#layer").val();
					jetcad.clear_input_boxes();
					jetcad.input_box("layer", layer, "", { x: jetcad.ClientMouse.x + 20, y: jetcad.ClientMouse.y + 20 });
					jetcad.DisableHotkeys = true;
		};
		jetcad.Callbacks.onTab = function(){
					$("#layer").focus();
					$("#layer").select();
		};
		jetcad.Callbacks.onEnter = function(){
			jetcad.RegisterUndo();
			var layer = $("#layer").val();
			console.log("Setting layer to " + layer);
			var selected = jetcad.ToolPoints[0];
			console.log(selected);
			for (var i = 0; i < selected.length; i++)
			{
				var index = selected[i];
				jetcad.Stack[index].meta.Layer = layer;
			}
			jetcad_tools.Escape();
		};

	}
	Color()
	{
		var selected = [];
		for (var x = 0; x < jetcad.Stack.length; x++)
		{
			if (jetcad.metaIsSelected(x))
			{
				selected.push(x);
			}
		}
		jetcad.ToolPoints[0] = selected;
		jetcad.DisableHotkeys = true;
		jetcad.input_box("color", "black", "", { x: jetcad.ClientMouse.x + 20, y: jetcad.ClientMouse.y + 20 });
		jetcad.Callbacks.onMouse = function(){
					var color = $("#color").val();
					jetcad.clear_input_boxes();
					jetcad.input_box("color", color, "", { x: jetcad.ClientMouse.x + 20, y: jetcad.ClientMouse.y + 20 });
					jetcad.DisableHotkeys = true;
		};
		jetcad.Callbacks.onTab = function(){
					$("#color").focus();
					$("#color").select();
		};
		jetcad.Callbacks.onEnter = function(){
			jetcad.RegisterUndo();
			var color = $("#color").val();
			console.log("Setting color to " + color);
			var selected = jetcad.ToolPoints[0];
			console.log(selected);
			for (var i = 0; i < selected.length; i++)
			{
				var index = selected[i];
				jetcad.Stack[index].meta.Color = color;
			}
			jetcad_tools.Escape();
		};

	}
	Mirror()
	{
		var selected = [];
		for (var x = 0; x < jetcad.Stack.length; x++)
		{
			if (jetcad.metaIsSelected(x))
			{
				selected.push(x);
			}
		}
		jetcad.SetStatusText("Click line to mirror about!");
		var FirstClick = function(){
			jetcad.RegisterUndo();
			for (var x = 0; x < jetcad.Stack.length; x++)
			{
				if (jetcad.metaIsMouseOver(x))
				{
					var mirror_line = jetcad.Stack[x];
					if (mirror_line.type != "line")
					{
						notification.push("Mirror must be line!");
						jetcad_tools.Escape();
						break;
					}
					for (var i = 0; i < selected.length; i++)
					{
						var index = selected[i];
						if (jetcad.Stack[index].type == "line")
						{
							var new_origin = jetcad.mirror_point_about_line({x: jetcad.Stack[index].origin[0], y: jetcad.Stack[index].origin[1]}, {x: mirror_line.origin[0], y: mirror_line.origin[1]}, {x: mirror_line.end[0], y: mirror_line.end[1]});
							var new_end = jetcad.mirror_point_about_line({x: jetcad.Stack[index].end[0], y: jetcad.Stack[index].end[1]}, {x: mirror_line.origin[0], y: mirror_line.origin[1]}, {x: mirror_line.end[0], y: mirror_line.end[1]});
							jetcad.Stack.push({ type: 'line', origin: [new_origin.x, new_origin.y], end: [new_end.x, new_end.y]});
						}
						else if (jetcad.Stack[index].type == "arc")
						{
							var start_line = jetcad.get_line_at_angle({x: jetcad.Stack[index].origin[0], y: jetcad.Stack[index].origin[1]}, jetcad.Stack[index].startAngle, jetcad.Stack[index].radius);
							var end_line = jetcad.get_line_at_angle({x: jetcad.Stack[index].origin[0], y: jetcad.Stack[index].origin[1]}, jetcad.Stack[index].endAngle, jetcad.Stack[index].radius);

							var new_origin = jetcad.mirror_point_about_line({x: jetcad.Stack[index].origin[0], y: jetcad.Stack[index].origin[1]}, {x: mirror_line.origin[0], y: mirror_line.origin[1]}, {x: mirror_line.end[0], y: mirror_line.end[1]});
							var new_startpoint = jetcad.mirror_point_about_line({x: start_line.end[0], y: start_line.end[1]}, {x: mirror_line.origin[0], y: mirror_line.origin[1]}, {x: mirror_line.end[0], y: mirror_line.end[1]});
							var new_endpoint = jetcad.mirror_point_about_line({x: end_line.end[0], y: end_line.end[1]}, {x: mirror_line.origin[0], y: mirror_line.origin[1]}, {x: mirror_line.end[0], y: mirror_line.end[1]});
							var new_startAngle = jetcad.get_line_angle(new_origin, new_startpoint);
							var new_endAngle = jetcad.get_line_angle(new_origin, new_endpoint);
							jetcad.Stack.push({ type: 'arc', origin: [new_origin.x, new_origin.y], radius: jetcad.Stack[index].radius, startAngle: new_endAngle, endAngle: new_startAngle});
						}
						else if (jetcad.Stack[index].type == "circle")
						{
							var new_origin = jetcad.mirror_point_about_line({x: jetcad.Stack[index].origin[0], y: jetcad.Stack[index].origin[1]}, {x: mirror_line.origin[0], y: mirror_line.origin[1]}, {x: mirror_line.end[0], y: mirror_line.end[1]});
							jetcad.Stack.push({ type: 'circle', origin: [new_origin.x, new_origin.y], radius: jetcad.Stack[index].radius});
						}
						else
						{
							notification.push("Unhandled entity! " + jetcad.Stack[index].type);
						}
					}
					jetcad_tools.Escape();
					break;
				}
			}
		};
		jetcad.NextClick = [ FirstClick ];
	}
	Scale()
	{
		var selected = [];
		for (var x = 0; x < jetcad.Stack.length; x++)
		{
			if (jetcad.metaIsSelected(x))
			{
				selected.push(x);
			}
		}
		jetcad.input_box("scale", 1, "factor", { x: jetcad.ClientMouse.x + 20, y: jetcad.ClientMouse.y + 20 });
		jetcad.Callbacks.onMouse = function(){
					var scale = jetcad.get_input_box_value("scale");
					jetcad.clear_input_boxes();
					jetcad.input_box("scale", scale, "scale factor", { x: jetcad.ClientMouse.x + 20, y: jetcad.ClientMouse.y + 20 });
			};
		jetcad.Callbacks.onTab = function(){
					$("#scale").focus();
					$("#scale").select();
		};
		jetcad.Callbacks.onEnter = function(){
					jetcad.RegisterUndo();
					//console.log("Enter!");
					var scale = jetcad.get_input_box_value("scale");
					jetcad.clear_input_boxes();

					for (var i = 0; i < selected.length; i++)
					{
						var index = selected[i];
						if (jetcad.Stack[index].type == "line")
						{
							jetcad.Stack[index].origin[0] *= scale;
							jetcad.Stack[index].origin[1] *= scale;

							jetcad.Stack[index].end[0] *= scale;
							jetcad.Stack[index].end[1] *= scale;
						}
						else if (jetcad.Stack[index].type == "circle")
						{
							jetcad.Stack[index].origin[0] *= scale;
							jetcad.Stack[index].origin[1] *= scale;

							jetcad.Stack[index].radius *= scale;
						}
						else if (jetcad.Stack[index].type == "arc")
						{
							jetcad.Stack[index].origin[0] *= scale;
							jetcad.Stack[index].origin[1] *= scale;

							jetcad.Stack[index].radius *= scale;
						}
						else
						{
							notification.push("Unhandled Entity " + jetcad.Stack[index]);
						}
					}

					jetcad_tools.Escape();
			};

	}
	Circular_Pattern()
	{
		var selected = [];
		for (var x = 0; x < jetcad.Stack.length; x++)
		{
			if (jetcad.metaIsSelected(x))
			{
				selected.push(x);
			}
		}
		jetcad.pushLive(function(m){
      var e = {
         type: 'circle',
         origin: [jetcad.SnappedMouse.x, jetcad.SnappedMouse.y],
         radius: jetcad.View.MouseOverRadius / jetcad.View.zoom
       };

       return e;
    });
		jetcad.SetStatusText("Click Point to rotate around!");
		var FirstClick = function(){
			jetcad.ToolPoints[0] = jetcad.SnappedMouse;
			jetcad.SetStatusText("Input variables and press enter!");
			jetcad.clear_input_boxes();
			jetcad.input_box("angle", 10, "deg (Step Angle)", { x: jetcad.ClientMouse.x + 20, y: jetcad.ClientMouse.y + 20 });
			jetcad.input_box("steps", 1, "(Steps)", { x: jetcad.ClientMouse.x + 20, y: jetcad.ClientMouse.y + 50 });

			jetcad.Callbacks.onMouse = function(){
					var angle = jetcad.get_input_box_value("angle");
					var steps = jetcad.get_input_box_value("steps");
					jetcad.clear_input_boxes();
					jetcad.input_box("angle", angle, "deg (Step Angle)", { x: jetcad.ClientMouse.x + 20, y: jetcad.ClientMouse.y + 20 });
					jetcad.input_box("steps", steps, "(Steps)", { x: jetcad.ClientMouse.x + 20, y: jetcad.ClientMouse.y + 50 });
			};
			jetcad.Callbacks.onEnter = function(){
					jetcad.RegisterUndo();
					var angle = jetcad.get_input_box_value("angle");
					var steps = jetcad.get_input_box_value("steps");
					jetcad.clear_input_boxes();

					for (var i = 0; i < selected.length; i++)
					{
						var index = selected[i];
						if (jetcad.Stack[index].type == "line")
						{
							for (var x = 1; x <= steps; x++)
							{
								var new_origin = jetcad.rotate_point({ x: jetcad.Stack[index].origin[0], y: jetcad.Stack[index].origin[1] }, jetcad.ToolPoints[0], angle * x);
								var new_end = jetcad.rotate_point({ x: jetcad.Stack[index].end[0], y: jetcad.Stack[index].end[1] }, jetcad.ToolPoints[0], angle * x);
								var e = {
									 type: 'line',
									 origin: [ new_origin.x, new_origin.y ],
									 end: [ new_end.x, new_end.y ]
								 };
								jetcad.Stack.push(e);
							}
						}
						if (jetcad.Stack[index].type == "circle")
						{
							for (var x = 1; x <= steps; x++)
							{
								console.log("Step: " + x);
								var new_origin = jetcad.rotate_point({ x: jetcad.Stack[index].origin[0], y: jetcad.Stack[index].origin[1] }, jetcad.ToolPoints[0], angle * x);
								var e = {
									 type: 'circle',
									 origin: [ new_origin.x, new_origin.y ],
									 radius: jetcad.Stack[index].radius
								 };
								jetcad.Stack.push(e);
							}
						}
						if (jetcad.Stack[index].type == "arc")
						{
							for (var x = 1; x <= steps; x++)
							{
								var new_origin = jetcad.rotate_point({ x: jetcad.Stack[index].origin[0], y: jetcad.Stack[index].origin[1] }, jetcad.ToolPoints[0], angle * x);
								var new_startAngle = jetcad.Stack[index].startAngle - (angle * x);
								var new_endAngle = jetcad.Stack[index].endAngle - (angle * x);
								var e = {
									 type: 'arc',
									 origin: [ new_origin.x, new_origin.y ],
									 radius: jetcad.Stack[index].radius,
									 startAngle: new_startAngle,
									 endAngle: new_endAngle,
								 };
								jetcad.Stack.push(e);
							}
						}
					}

					jetcad_tools.Escape();
			};
			jetcad.Callbacks.onTab = function(){
				$("#angle").focus();
				$("#angle").select();
				jetcad.Callbacks.onTab = function(){
					$("#steps").focus();
					$("#steps").select();
				};
			};
		};
		jetcad.NextClick = [ FirstClick ];
	}
	Rotate()
	{
		var selected = [];
		for (var x = 0; x < jetcad.Stack.length; x++)
		{
			if (jetcad.metaIsSelected(x))
			{
				selected.push(x);
			}
		}
		jetcad.SetStatusText("Click point to rotate around!");
		jetcad.pushLive(function(m){
      var e = {
         type: 'circle',
         origin: [jetcad.SnappedMouse.x, jetcad.SnappedMouse.y],
         radius: jetcad.View.MouseOverRadius / jetcad.View.zoom
       };

       return e;
    });
		var FirstClick = function(){
			jetcad.SetStatusText("Click reference point!");
			jetcad.ToolPoints[0] = jetcad.SnappedMouse;
		};
		var SecondClick = function(){
			jetcad.RegisterUndo();
			jetcad.ToolPoints[1] = jetcad.SnappedMouse;
			jetcad.ToolPoints[3] = jetcad.get_line_angle(jetcad.ToolPoints[0], jetcad.SnappedMouse);
			jetcad.ToolPoints[4] = jetcad.get_line_angle(jetcad.ToolPoints[0], jetcad.SnappedMouse);

			jetcad.ToolPoints[5] = jetcad.get_line_angle(jetcad.ToolPoints[1], jetcad.SnappedMouse);
			jetcad.Callbacks.onMouse = function()
			{
				if (jetcad.distance(jetcad.ToolPoints[0], jetcad.SnappedMouse) > 0.5)
				{
					jetcad.ToolPoints[4] = jetcad.get_line_angle(jetcad.ToolPoints[0], jetcad.SnappedMouse) + jetcad.ToolPoints[5] + -180;
					var diff_angle = jetcad.ToolPoints[3] - jetcad.ToolPoints[4];
					jetcad.ToolPoints[3] = jetcad.ToolPoints[4];
					for (var i = 0; i < selected.length; i++)
					{
						var index = selected[i];
						if (jetcad.Stack[index].type == "line")
						{
							var new_origin = jetcad.rotate_point({ x: jetcad.Stack[index].origin[0], y: jetcad.Stack[index].origin[1] }, jetcad.ToolPoints[0], diff_angle);
							var new_end = jetcad.rotate_point({ x: jetcad.Stack[index].end[0], y: jetcad.Stack[index].end[1] }, jetcad.ToolPoints[0], diff_angle);
							jetcad.Stack[index].origin = [ new_origin.x, new_origin.y ];
							jetcad.Stack[index].end = [ new_end.x, new_end.y ];
						}
						if (jetcad.Stack[index].type == "circle")
						{
							var new_origin = jetcad.rotate_point({ x: jetcad.Stack[index].origin[0], y: jetcad.Stack[index].origin[1] }, jetcad.ToolPoints[0], diff_angle);
							jetcad.Stack[index].origin = [ new_origin.x, new_origin.y ];
						}
						if (jetcad.Stack[index].type == "arc")
						{
							var new_origin = jetcad.rotate_point({ x: jetcad.Stack[index].origin[0], y: jetcad.Stack[index].origin[1] }, jetcad.ToolPoints[0], diff_angle);
							jetcad.Stack[index].origin = [ new_origin.x, new_origin.y ];

							jetcad.Stack[index].startAngle -= diff_angle;
							jetcad.Stack[index].endAngle -= diff_angle;
						}
					}
				}
			}
		};
		var ThreeClick = function(){
			jetcad_tools.Escape();
		};
		jetcad.NextClick = [ FirstClick, SecondClick, ThreeClick ];

	}
	SearchToolBox()
	{
		$("#draw-search").show();
		$("#draw-search").focus();
		jetcad.HandlersActive = false;
	}
	SaveDrawing()
	{
		jetcad.SaveDrawing();
	}
	Space()
	{
		//console.log("Space!");
		//console.log(jetcad.SnappedMouse);
		jetcad.Callbacks.DoubleClick(); //Select chain
	}
	ToggleConstructionStyle()
	{
		//console.log("Toggle Construction");
		jetcad.RegisterUndo();
		for (var x = 0; x < jetcad.Stack.length; x++)
		{
			if (jetcad.metaIsSelected(x))
			{
				if (jetcad.metaGetStyle(x) == "solid")
				{
					//console.log("Setting to dashed!");
					jetcad.metaSetStyle(x, "dashed");
				}
				else
				{
					//console.log("Setting to solid!");
					jetcad.metaSetStyle(x, "solid");
				}
			}
		}
		jetcad_tools.Escape();
	}
	CopySelected()
	{
		jetcad.CopiedIndex = [];
		for (var x = 0; x < jetcad.Stack.length; x++)
		{
			if (jetcad.metaIsSelected(x))
			{
				jetcad.CopiedIndex.push(x);
			}
		}
	}
	PasteSelected()
	{
		jetcad.RegisterUndo();
		for (var x = 0; x < jetcad.CopiedIndex.length; x++)
		{
			var new_entity = JSON.parse(JSON.stringify(jetcad.Stack[jetcad.CopiedIndex[x]]));
			jetcad.Stack.push(new_entity);
		}
		jetcad.ToolPoints[0] = jetcad.SnappedMouse;
			jetcad.Callbacks.onMouse = function(){

				if (jetcad.distance(jetcad.ToolPoints[0], jetcad.SnappedMouse) < 10)
				{
					var x_inc = jetcad.SnappedMouse.x - jetcad.ToolPoints[0].x;
					var y_inc = jetcad.SnappedMouse.y - jetcad.ToolPoints[0].y;
					for (var i = 0; i < jetcad.CopiedIndex.length; i++)
					{
						var index = jetcad.CopiedIndex[i];
						if (jetcad.Stack[index].type == "line")
						{
							jetcad.Stack[index].origin[0] += x_inc;
							jetcad.Stack[index].origin[1] += y_inc;
							jetcad.Stack[index].end[0] += x_inc;
							jetcad.Stack[index].end[1] += y_inc;
						}
						if (jetcad.Stack[index].type == "arc" || jetcad.Stack[index].type == "circle")
						{
							jetcad.Stack[index].origin[0] += x_inc;
							jetcad.Stack[index].origin[1] += y_inc;
						}
					}
				}
				jetcad.ToolPoints[0] = jetcad.SnappedMouse;
			};
		var FirstClick = function(){
			jetcad_tools.Escape();
		};
		jetcad.NextClick = [ FirstClick ];
	}
	Move()
	{
		jetcad.pushLive(function(m){
      var e = {
         type: 'circle',
         origin: [jetcad.SnappedMouse.x, jetcad.SnappedMouse.y],
         radius: jetcad.View.MouseOverRadius / jetcad.View.zoom
       };

       return e;
    });
		var FirstClick = function(){
      jetcad.RegisterUndo();
			var SelectedIndexes = [];
			for (var x = 0; x < jetcad.Stack.length; x++)
			{
				if (jetcad.metaIsSelected(x))
				{
					SelectedIndexes.push(x);
				}
			}
			jetcad.ToolPoints[0] = jetcad.SnappedMouse;
			jetcad.Callbacks.onMouse = function(){

				if (jetcad.distance(jetcad.ToolPoints[0], jetcad.SnappedMouse) < 10)
				{

					var x_inc = jetcad.SnappedMouse.x - jetcad.ToolPoints[0].x;
					var y_inc = jetcad.SnappedMouse.y - jetcad.ToolPoints[0].y;
					for (var i = 0; i < SelectedIndexes.length; i++)
					{
						var index = SelectedIndexes[i];
						if (jetcad.Stack[index].type == "line")
						{
							jetcad.Stack[index].origin[0] += x_inc;
							jetcad.Stack[index].origin[1] += y_inc;
							jetcad.Stack[index].end[0] += x_inc;
							jetcad.Stack[index].end[1] += y_inc;
						}
						if (jetcad.Stack[index].type == "arc" || jetcad.Stack[index].type == "circle")
						{
							jetcad.Stack[index].origin[0] += x_inc;
							jetcad.Stack[index].origin[1] += y_inc;
						}
					}
				}
				jetcad.ToolPoints[0] = jetcad.SnappedMouse;

			};
		};
		var SecondClick = function(){
			jetcad_tools.Escape();
		};
		jetcad.NextClick = [ FirstClick, SecondClick ];

	}
	DrawRectangle()
	{
		jetcad.pushLive(function(m){
      var e = {
         type: 'circle',
         origin: [jetcad.SnappedMouse.x, jetcad.SnappedMouse.y],
         radius: jetcad.View.MouseOverRadius / jetcad.View.zoom
       };

       return e;
    });
		var FirstClick = function()
		{
			jetcad.ToolPoints[0] = jetcad.SnappedMouse;
				jetcad.pushLive(function(m){
          var e;
           e = {
             type: 'line',
             origin: [jetcad.ToolPoints[0].x, jetcad.ToolPoints[0].y],
             end: [ jetcad.ToolPoints[0].x, jetcad.SnappedMouse.y]
           };
					return e;
				});
				jetcad.pushLive(function(m){
          var e;
           e = {
             type: 'line',
             origin: [jetcad.ToolPoints[0].x, jetcad.ToolPoints[0].y],
             end: [ jetcad.SnappedMouse.x, jetcad.ToolPoints[0].y]
           };
					return e;
				});
				jetcad.pushLive(function(m){
          var e;
           e = {
             type: 'line',
             origin: [jetcad.SnappedMouse.x, jetcad.SnappedMouse.y],
             end: [jetcad.ToolPoints[0].x + (jetcad.SnappedMouse.x - jetcad.ToolPoints[0].x), jetcad.ToolPoints[0].y]
           };
					return e;
				});
				jetcad.pushLive(function(m){
          var e;
           e = {
             type: 'line',
             origin: [jetcad.SnappedMouse.x, jetcad.SnappedMouse.y],
             end: [jetcad.ToolPoints[0].x, jetcad.ToolPoints[0].y + (jetcad.SnappedMouse.y - jetcad.ToolPoints[0].y)]
           };
					jetcad.clear_input_boxes();
					jetcad.input_box("Width", Number(jetcad.SnappedMouse.x - jetcad.ToolPoints[0].x).toFixed(4), "in (Width)", { x: jetcad.ClientMouse.x + 20, y: jetcad.ClientMouse.y + 20 });
					jetcad.input_box("Height", Number(jetcad.SnappedMouse.y - jetcad.ToolPoints[0].y).toFixed(4), "in (Height)", { x: jetcad.ClientMouse.x + 20, y: jetcad.ClientMouse.y + 50 });
					jetcad.Callbacks.onTab =function(){
						$("#Width").focus();
						$("#Width").select();
						jetcad.Callbacks.onTab = function(){
							 $("#Height").focus();
							 $("#Height").select();
							jetcad.Callbacks.onTab = null;
						};
					};
					return e;
				});

		}
		var SecondClick = function(){
			jetcad.RegisterUndo();
			jetcad.ToolPoints[1] = jetcad.SnappedMouse;

			//Right Line
      var right = {
        type: 'line',
        origin: [jetcad.ToolPoints[0].x, jetcad.ToolPoints[0].y],
        end: [ jetcad.ToolPoints[0].x, jetcad.ToolPoints[1].y]
      };
			jetcad.Stack.push(jetcad.breakLink(right));

			//Bottom Line
			var bottom = {
        type: 'line',
        origin: [jetcad.ToolPoints[0].x, jetcad.ToolPoints[0].y],
        end: [ jetcad.ToolPoints[1].x, jetcad.ToolPoints[0].y]
      };
			jetcad.Stack.push(jetcad.breakLink(bottom));

			//Top Line
			var top = {
        type: 'line',
        origin: [jetcad.ToolPoints[1].x, jetcad.ToolPoints[1].y],
        end: right.end
      };
			jetcad.Stack.push(jetcad.breakLink(top));

			//Left Line
			var left = {
        type: 'line',
        origin: [jetcad.ToolPoints[1].x, jetcad.ToolPoints[1].y],
        end: bottom.end
      };
			jetcad.Stack.push(jetcad.breakLink(left));
			jetcad_tools.Escape();

		};
		jetcad.NextClick = [ FirstClick, SecondClick ];

		jetcad.Callbacks.onEnter =function()
		{
			jetcad.RegisterUndo();
			var width = jetcad.get_input_box_value("Width");
			var height = jetcad.get_input_box_value("Height");
			console.log("Width: " + width);
			console.log("Height: " + height);
      jetcad.SnappedMouse = { x: jetcad.ToolPoints[0].x + width, y: jetcad.ToolPoints[0].y + height };
      (SecondClick)();
      jetcad_tools.Escape();
    };
	}
  DrawLine()
  {

    jetcad.pushLive(function(m){

      var e = {
         type: 'circle',
         origin: [jetcad.SnappedMouse.x, jetcad.SnappedMouse.y],
         radius: jetcad.View.MouseOverRadius / jetcad.View.zoom
       };

       return e;
    });

    jetcad.NextClick = [function(){

        jetcad.ToolPoints[0] = jetcad.SnappedMouse;

        jetcad.pushLive(function(m){
          var e;
          jetcad.clear_input_boxes();
          if (jetcad.ModifierKeys.Shift)
          {
            e = {
              type: 'line',
              origin: [jetcad.ToolPoints[0].x, jetcad.ToolPoints[0].y],
              end: [jetcad.SnappedMouse.x, jetcad.SnappedMouse.y]
            };
            var angle = jetcad.get_line_angle({x: e.origin[0], y:e.origin[1] }, {x: e.end[0], y: e.end[1] });
            //console.log(angle);
            if (angle < 133 && angle > 45 || angle < 335 && angle > 225)
            {
              //Veritcal
              e = {
                type: 'line',
                origin: [jetcad.ToolPoints[0].x, jetcad.ToolPoints[0].y],
                end: [jetcad.ToolPoints[0].x, jetcad.SnappedMouse.y]
              };
            }
            else
            {
              //Horizontal
              e = {
                type: 'line',
                origin: [jetcad.ToolPoints[0].x, jetcad.ToolPoints[0].y],
                end: [jetcad.SnappedMouse.x, jetcad.ToolPoints[0].y]
              };
            }

          }
          else
          {
             e = {
              type: 'line',
              origin: [jetcad.ToolPoints[0].x, jetcad.ToolPoints[0].y],
              end: [jetcad.SnappedMouse.x, jetcad.SnappedMouse.y]
             };
          }
          jetcad.input_box("length", jetcad.distance(jetcad.ToolPoints[0], jetcad.SnappedMouse).toFixed(4), "in", { x: jetcad.ClientMouse.x + 20, y: jetcad.ClientMouse.y + 20 });
          jetcad.input_box("angle", jetcad.get_line_angle(jetcad.ToolPoints[0], jetcad.SnappedMouse).toFixed(3), "deg", { x: jetcad.ClientMouse.x + 20, y: jetcad.ClientMouse.y + 50 });
          jetcad.Callbacks.onTab =function(){
            $("#length").focus();
            $("#length").select();
            jetcad.Callbacks.onTab = function(){
               $("#angle").focus();
               $("#angle").select();
              jetcad.Callbacks.onTab = null;
            };
          };
          jetcad.Callbacks.onEnter =function(){
            jetcad.RegisterUndo();
            var end_point = jetcad.polarToCartesian(jetcad.ToolPoints[0].x, jetcad.ToolPoints[0].y, jetcad.get_input_box_value("length"), jetcad.get_input_box_value("angle"));

            jetcad.ToolPoints[1] = end_point;

            var line = {
              type: 'line',
              origin: [jetcad.ToolPoints[0].x, jetcad.ToolPoints[0].y],
              end: [jetcad.ToolPoints[1].x, jetcad.ToolPoints[1].y]
            };
            jetcad.Stack.push(line);

            jetcad_tools.Escape();
          };
          return e;
        });



    }, function(){
       jetcad.RegisterUndo();
       jetcad.ToolPoints[1] = { x: jetcad.LiveStack[1].end[0], y: jetcad.LiveStack[1].end[1]};

        var line = {
          type: 'line',
          origin: [jetcad.ToolPoints[0].x, jetcad.ToolPoints[0].y] ,
          end: [jetcad.ToolPoints[1].x, jetcad.ToolPoints[1].y]
        };
        jetcad.clear_input_boxes();
        jetcad.clearLive();
		    jetcad.Stack.push(line);
        jetcad.render(true);
    }];
  }
  DrawCircle()
  {
    jetcad.pushLive(function(m){
      var e = {
         type: 'circle',
         origin: [jetcad.SnappedMouse.x, jetcad.SnappedMouse.y],
         radius: jetcad.View.MouseOverRadius / jetcad.View.zoom
       };
       return e;
    });
    jetcad.NextClick = [function(){

        jetcad.ToolPoints[0] = jetcad.SnappedMouse;

        jetcad.pushLive(function(m){
          var e;
          jetcad.clear_input_boxes();

          jetcad.input_box("diameter", (jetcad.distance(jetcad.ToolPoints[0], jetcad.SnappedMouse) * 2).toFixed(4), "in", { x: jetcad.ClientMouse.x + 20, y: jetcad.ClientMouse.y + 20 });
          jetcad.Callbacks.onTab =function(){
            $("#diameter").focus();
            $("#diameter").select();
             jetcad.Callbacks.onTab = null;
          };
          e = {
              type: 'circle',
              origin: [jetcad.ToolPoints[0].x, jetcad.ToolPoints[0].y],
              radius: jetcad.distance(jetcad.ToolPoints[0], jetcad.SnappedMouse)
            };
          jetcad.Callbacks.onEnter =function(){
            jetcad.RegisterUndo();
            var diameter = jetcad.get_input_box_value("diameter");

            jetcad.ToolPoints[1] = { x: jetcad.ToolPoints[0].x + (diameter / 2), y: jetcad.ToolPoints[0].y };

            var circle = {
              type: 'circle',
              origin: [jetcad.ToolPoints[0].x, jetcad.ToolPoints[0].y],
              radius: diameter / 2
            };
            jetcad.Stack.push(circle);

            jetcad_tools.Escape();
          };
          return e;
        });
        }, function(){
						jetcad.RegisterUndo();
            var diameter = jetcad.distance(jetcad.ToolPoints[0], jetcad.SnappedMouse)  * 2;

            jetcad.ToolPoints[1] = { x: jetcad.SnappedMouse.x + (diameter / 2), y: jetcad.SnappedMouse.y };

            var circle = {
              type: 'circle',
              origin: [jetcad.ToolPoints[0].x, jetcad.ToolPoints[0].y],
              radius: diameter / 2
            };
            jetcad.Stack.push(circle);

            jetcad_tools.Escape();
      }];
  }
	Trim()
	{
		jetcad.Callbacks.onMouse = function(){
			var found_mouseover = false;
			for (var x = 0; x < jetcad.Stack.length; x++)
      {
         if (jetcad.metaIsMouseOver(x))
         {
					 found_mouseover = true;
					 	var Intersections = jetcad.get_all_intersections(jetcad.Stack[x]);
					 	if (jetcad.Stack[x].type =='circle-angle-take')
						{
							jetcad.StaticStack = "";
							var intersection_angles = [];
							for (var i = 0; i < Intersections.length; i++)
							{
								for (var key in Intersections[i])
								{
									if (key.indexOf("path") != -1)
									{
										for (var a = 0; a < Intersections[i][key][a]; a++)
										{
											intersection_angles.push(Intersections[i][key][a]);
										}
										//console.log("path: " + key + " i=" + i);
										//console.log(intersection_angles);
									}
								}
							}
							intersection_angles = intersection_angles.sort(function(a,b) { return a - b;});
							//ttconsole.log(intersection_angles);
							var drag_line_angle = jetcad.get_line_angle({ x: jetcad.Stack[x].origin[0], y: jetcad.Stack[x].origin[1] }, jetcad.Mouse);
							//console.log("Drag line angle: " + drag_line_angle);
							var is_inbetween = false;
							var arc_segments = [];
							jetcad.StaticStack = "";
							for (var i = 1; i < intersection_angles.length; i++)
							{
								var line_one = jetcad.get_line_at_angle({x: jetcad.Stack[x].origin[0], y: jetcad.Stack[x].origin[1]}, intersection_angles[i -1], jetcad.Stack[x].radius);
								var line_two = jetcad.get_line_at_angle({x: jetcad.Stack[x].origin[0], y: jetcad.Stack[x].origin[1]}, intersection_angles[i], jetcad.Stack[x].radius)
								jetcad.StaticStack += jetcad.svgLine({x: line_one.origin[0], y: line_one.origin[1]}, {x: line_one.end[0], y: line_one.end[1]}, "blue");
								jetcad.StaticStack += jetcad.svgLine({x: line_two.origin[0], y: line_two.origin[1]}, {x: line_two.end[0], y: line_two.end[1]}, "blue");
								if (drag_line_angle > intersection_angles[i -1] && drag_line_angle < intersection_angles[i] )
								{
									jetcad.StaticStack = jetcad.svgArc({x: jetcad.Stack[x].origin[0], y: jetcad.Stack[x].origin[1]}, jetcad.Stack[x].radius, intersection_angles[i -1], intersection_angles[i], "red");
									arc_segments.push({startAngle: intersection_angles[i -1], endAngle: intersection_angles[i], trimed: true});
								}
								else
								{
									arc_segments.push({startAngle: intersection_angles[i -1], endAngle: intersection_angles[i], trimed: false});
								}
							}
							console.log(arc_segments);
							jetcad.ToolPoints[0] = arc_segments;
							jetcad.ToolPoints[1] = x;
							jetcad.ToolPoints[2] = jetcad.Stack[x];
							jetcad.NextClick = [ function(){
										jetcad.RegisterUndo();
										jetcad.metaSelected(jetcad.ToolPoints[1], true);
										jetcad_tools.DeleteSelected();
										for (var i = 0; i < jetcad.ToolPoints[0].length; i++)
										{
											if (jetcad.ToolPoints[0][i].trimed == false)
											{

												jetcad.Stack.push({
													type: 'arc',
													origin: jetcad.ToolPoints[2].origin,
													radius: jetcad.ToolPoints[2].radius,
													startAngle: jetcad.ToolPoints[0][i].startAngle,
													endAngle: jetcad.ToolPoints[0][i].endAngle,
												});
											}
										}
										jetcad.Stack.push({
													type: 'arc',
													origin: jetcad.ToolPoints[2].origin,
													radius: jetcad.ToolPoints[2].radius,
													startAngle: jetcad.ToolPoints[0][jetcad.ToolPoints[0].length - 1].endAngle,
													endAngle: jetcad.ToolPoints[0][0].startAngle,
												});
										jetcad.StaticStack = "";
										jetcad.render(true);
									}];
						}
					 	if (jetcad.Stack[x].type =='circle')
						{
							jetcad.StaticStack = "";
							var intersection_points = [];
							var intersection_paths = [];
							//console.log(Intersections);
							for (var i = 0; i < Intersections.length; i++)
							{
								for (var key in Intersections[i])
								{
									if (key.indexOf("path") != -1)
									{
										//We are a path angle
										intersection_paths.push(Intersections[i][key]);
									}
									else
									{
										//We are an point
										for (var a = 0; a < Intersections[i][key].length; a++)
										{
											var point = {x: Intersections[i][key][a][0], y: Intersections[i][key][a][1]};
											var angle = jetcad.get_line_angle({x: jetcad.Stack[x].origin[0], y: jetcad.Stack[x].origin[1]}, point);
											var int = { angle: angle, point: point };
											intersection_points.push(int);
										}
									}
								}
							}
							intersection_points = intersection_points.sort(function(a,b) { return a.angle - b.angle;});
							//console.log(intersection_points);
							var found_intersected_segment = false;
							/*if (intersection_points.length > 2)
							{
								for (var i = 0; i < intersection_points.length; i++)
								{
									//jetcad.StaticStack += jetcad.svgCircle(intersection_points[i].point, jetcad.View.MouseOverRadius / jetcad.View.zoom , "red");
									//Find using chord technique
									var cord, drag_line, color;
									drag_line = { type: 'line', origin: jetcad.Stack[x].origin, end: [jetcad.Mouse.x, jetcad.Mouse.y] };
									color = "red";
									if (i > 0)
									{
										cord = { type: 'line', origin: [intersection_points[i-1].point.x, intersection_points[i-1].point.y], end: [intersection_points[i].point.x, intersection_points[i].point.y] };
										//jetcad.StaticStack += jetcad.svgLine(intersection_points[i-1].point, intersection_points[i].point, color);
									}
									else
									{
										cord = { type: 'line', origin: [intersection_points[intersection_points.length-1].point.x, intersection_points[intersection_points.length-1].point.y], end: [intersection_points[i].point.x, intersection_points[i].point.y] };
										//jetcad.StaticStack += jetcad.svgLine(intersection_points[intersection_points.length-1].point, intersection_points[i].point, color);
									}
									if (jetcad.get_intersection(cord, drag_line) && jetcad.distance(jetcad.midpoint({x: cord.origin[0],y: cord.origin[1]}, {x: cord.end[0],y: cord.end[1]}), {x: jetcad.Stack[x].origin[0], y: jetcad.Stack[x].origin[1]}) > (jetcad.Stack[x].radius / 3))
									{
										color = "red";
										var start_angle = jetcad.get_line_angle({x: jetcad.Stack[x].origin[0], y: jetcad.Stack[x].origin[1]}, {x: cord.origin[0], y: cord.origin[1]});
										var end_angle = jetcad.get_line_angle({x: jetcad.Stack[x].origin[0], y: jetcad.Stack[x].origin[1]}, {x: cord.end[0], y: cord.end[1]});
										jetcad.StaticStack += jetcad.svgArc({x: jetcad.Stack[x].origin[0], y: jetcad.Stack[x].origin[1]}, jetcad.Stack[x].radius, start_angle, end_angle, color);
										found_intersected_segment = true;
									}
								}
							}*/
							if (found_intersected_segment == false && intersection_points.length > 0)
							{
								//console.log(intersection_paths);
								var segments_to_leave = [];
								for (var i = 0; i < intersection_points.length; i++)
								{
									var segment, drag_line, color, start_angle, end_angle;
									drag_line = { type: 'line', origin: jetcad.Stack[x].origin, end: [jetcad.Mouse.x, jetcad.Mouse.y] };
									if (i > 0)
									{
										start_angle = jetcad.get_line_angle({x: jetcad.Stack[x].origin[0], y: jetcad.Stack[x].origin[1]}, intersection_points[i-1].point);
										end_angle = jetcad.get_line_angle({x: jetcad.Stack[x].origin[0], y: jetcad.Stack[x].origin[1]}, intersection_points[i].point);
										segment = { type: 'arc', origin: jetcad.Stack[x].origin, radius: jetcad.Stack[x].radius, startAngle: start_angle, endAngle: end_angle };
										//jetcad.StaticStack += jetcad.svgLine(intersection_points[i-1].point, intersection_points[i].point, color);
									}
									else
									{
										start_angle = jetcad.get_line_angle({x: jetcad.Stack[x].origin[0], y: jetcad.Stack[x].origin[1]}, intersection_points[intersection_points.length-1].point);
										end_angle = jetcad.get_line_angle({x: jetcad.Stack[x].origin[0], y: jetcad.Stack[x].origin[1]}, intersection_points[i].point);
										segment = { type: 'arc', origin: jetcad.Stack[x].origin, radius: jetcad.Stack[x].radius, startAngle: start_angle, endAngle: end_angle };
									}
									if (jetcad.get_intersection(segment, drag_line))
									{
										jetcad.StaticStack += jetcad.svgArc({x: segment.origin[0], y: segment.origin[1]}, segment.radius, segment.startAngle, segment.endAngle, "red");
									}
									else
									{
										segments_to_leave.push(segment);
									}
								}
								jetcad.ToolPoints[1] = x;
								jetcad.ToolPoints[0] = segments_to_leave;
								var FirstClick = function(){
									jetcad.RegisterUndo();
										jetcad.metaSelected(jetcad.ToolPoints[1], true);
										jetcad_tools.DeleteSelected();
										for (var i = 0; i < jetcad.ToolPoints[0].length; i++)
										{
											jetcad.Stack.push({
													type: 'arc',
													origin: jetcad.ToolPoints[0][i].origin,
													radius: jetcad.ToolPoints[0][i].radius,
													startAngle: jetcad.ToolPoints[0][i].startAngle,
													endAngle: jetcad.ToolPoints[0][i].endAngle,
												});
										}
								};
								jetcad.NextClick = [ FirstClick ];
							}

						}
					 	if (jetcad.Stack[x].type == "arc")
						{
							//console.log("Trying to trim arc!");
							//console.log(Intersections);
							var intersection_angles = [];
							//jetcad.StaticStack += jetcad.svgArc({x: jetcad.Stack[x].origin[0], y: jetcad.Stack[x].origin[1]}, jetcad.Stack[x].radius, startAngle, endAngle, "red");
							for (var i = 0; i < Intersections.length; i++)
							{
								if (Intersections[i].path2Angles != undefined)
								{
									if (Intersections[i].path2Angles.length > 0) intersection_angles.push(Intersections[i].path2Angles[0]);
								}
							}
							intersection_angles = intersection_angles.sort(function(a,b) { return a - b;});
							//console.log(intersection_angles);
							var drag_line_angle = jetcad.get_line_angle({ x: jetcad.Stack[x].origin[0], y: jetcad.Stack[x].origin[1] }, jetcad.Mouse);
							//console.log("Drag line angle: " + drag_line_angle);
							var is_inbetween = false;
							var arc_segments = [];
							for (var i = 1; i < intersection_angles.length; i++)
							{
								if (drag_line_angle > intersection_angles[i -1] && drag_line_angle < intersection_angles[i] )
								{
									jetcad.StaticStack = jetcad.svgArc({x: jetcad.Stack[x].origin[0], y: jetcad.Stack[x].origin[1]}, jetcad.Stack[x].radius, intersection_angles[i -1], intersection_angles[i], "red");
									arc_segments.push({startAngle: intersection_angles[i -1], endAngle: intersection_angles[i], trimed: true});
								}
								else
								{
									arc_segments.push({startAngle: intersection_angles[i -1], endAngle: intersection_angles[i], trimed: false});
								}
							}
							console.log(arc_segments);
							jetcad.ToolPoints[0] = arc_segments;
							jetcad.ToolPoints[1] = x;
							jetcad.ToolPoints[2] = jetcad.Stack[x];
							jetcad.NextClick = [ function(){
										jetcad.RegisterUndo();
										jetcad.metaSelected(jetcad.ToolPoints[1], true);
										jetcad_tools.DeleteSelected();
										for (var i = 0; i < jetcad.ToolPoints[0].length; i++)
										{
											if (jetcad.ToolPoints[0][i].trimed == false)
											{

												jetcad.Stack.push({
													type: 'arc',
													origin: jetcad.ToolPoints[2].origin,
													radius: jetcad.ToolPoints[2].radius,
													startAngle: jetcad.ToolPoints[0][i].startAngle,
													endAngle: jetcad.ToolPoints[0][i].endAngle,
												});
											}
										}
										jetcad.StaticStack = "";
										jetcad.render(true);
									}];
						}
					 	if (jetcad.Stack[x].type == "line")
						{
							jetcad.StaticStack = "";
							var drag_lines = [];
							for (var i = 0; i < Intersections.length; i++)
							{
								var point = Intersections[i].intersectionPoints[0];
								//console.log(point);
								var e = {
									type: 'line',
									origin: [jetcad.Mouse.x, jetcad.Mouse.y],
									end: point
								};
								drag_lines.push(e);
							}
							var delete_between = [];
							for (var i = 0; i < drag_lines.length; i++)
							{
								//Get number of intersctions of drag_line[i], connect the endpoints of the drag lines that only have two intersection points
								if (jetcad.get_all_intersections(drag_lines[i]).length == 2)
								{
									delete_between.push(drag_lines[i].end);
								}
							}
							//console.log(delete_between);
							if (delete_between.length == 2) //We are an intersection between two lines
							{
								jetcad.StaticStack = jetcad.svgLine({x: delete_between[0][0], y: delete_between[0][1]}, {x: delete_between[1][0], y: delete_between[1][1]}, "red");
								var distance_between_zero_and_origin = jetcad.distance({x: delete_between[0][0], y: delete_between[0][1]}, {x: jetcad.Stack[x].origin[0], y: jetcad.Stack[x].origin[1]});
								var distance_between_zero_and_end = jetcad.distance({x: delete_between[0][0], y: delete_between[0][1]}, {x: jetcad.Stack[x].end[0], y: jetcad.Stack[x].end[1]});
								var distance_between_one_and_origin = jetcad.distance({x: delete_between[1][0], y: delete_between[1][1]}, {x: jetcad.Stack[x].origin[0], y: jetcad.Stack[x].origin[1]});
								var distance_between_one_and_end = jetcad.distance({x: delete_between[1][0], y: delete_between[1][1]}, {x: jetcad.Stack[x].end[0], y: jetcad.Stack[x].end[1]});

								var line_one;
								var line_two;
								//Draw origin to whichever intersection point is closer
								if (distance_between_zero_and_origin < distance_between_one_and_origin)
								{
									//Zero is closer to origin
									line_one = {
										type: "line",
										origin: jetcad.Stack[x].origin,
										end: delete_between[0]
									};
								}
								else
								{
									//One is closer to origin
									line_one = {
										type: "line",
										origin: jetcad.Stack[x].origin,
										end: delete_between[1]
									};
								}
								//Draw endpoint to whichever intersection point is closer
								if (distance_between_zero_and_end < distance_between_one_and_end)
								{
									//Zero is closer to end
									line_two = {
										type: "line",
										end: jetcad.Stack[x].end,
										origin: delete_between[0]
									};
								}
								else
								{
									//One is closer to end
									line_two = {
										type: "line",
										end: jetcad.Stack[x].end,
										origin: delete_between[1]
									};
								}

								jetcad.ToolPoints[0] = line_one;
								jetcad.ToolPoints[1] = line_two;
								jetcad.ToolPoints[2] = x;
									jetcad.NextClick = [ function(){
										jetcad.RegisterUndo();
										jetcad.metaSelected(jetcad.ToolPoints[2], true);
										jetcad_tools.DeleteSelected();

										jetcad.Stack.push(jetcad.ToolPoints[0]);
										jetcad.Stack.push(jetcad.ToolPoints[1]);
										jetcad.StaticStack = "";
										jetcad.render(true);
									}];
							}
							if (delete_between.length == 1) //We are not an instersection between two lines
							{
								if (jetcad.distance(jetcad.Mouse, {x: jetcad.Stack[x].origin[0], y: jetcad.Stack[x].origin[1]}) < jetcad.distance(jetcad.Mouse, {x: jetcad.Stack[x].end[0], y: jetcad.Stack[x].end[1]}))
								{
									jetcad.ToolPoints[0] = x;
									jetcad.ToolPoints[1] = delete_between[0];
									jetcad.StaticStack = jetcad.svgLine({x: delete_between[0][0], y: delete_between[0][1]}, {x: jetcad.Stack[x].origin[0], y: jetcad.Stack[x].origin[1]}, "red");
									jetcad.NextClick = [ function(){
										jetcad.RegisterUndo();
										jetcad.Stack[jetcad.ToolPoints[0]].origin = jetcad.ToolPoints[1];
										jetcad.StaticStack = "";
										jetcad.render(true);
									}];
								}
								else
								{
									jetcad.ToolPoints[0] = x;
									jetcad.ToolPoints[1] = delete_between[0];
									jetcad.StaticStack = jetcad.svgLine({x: delete_between[0][0], y: delete_between[0][1]}, {x: jetcad.Stack[x].end[0], y: jetcad.Stack[x].end[1]}, "red");
									jetcad.NextClick = [ function(){
										jetcad.RegisterUndo();
										jetcad.Stack[jetcad.ToolPoints[0]].end = jetcad.ToolPoints[1];
										jetcad.StaticStack = "";
										jetcad.render(true);
									}];
								}
							}
						}
         }
      }
			if (found_mouseover == false)
			{
				jetcad.StaticStack = "";
			}
		};
	}
	Fillet()
	{

		jetcad.input_box("radius", 0.250, "in", { x: jetcad.ClientMouse.x + 20, y: jetcad.ClientMouse.y + 20 });
		jetcad.pushLive(function(m){
          var e = {};
					var rad = jetcad.get_input_box_value("radius");
					jetcad.clear_input_boxes();
          jetcad.input_box("radius", rad, "in", { x: jetcad.ClientMouse.x + 20, y: jetcad.ClientMouse.y + 20 });
          jetcad.Callbacks.onTab =function(){
            $("#radius").focus();
            $("#radius").select();
             jetcad.Callbacks.onTab = null;
          };
          jetcad.Callbacks.onEnter =function(){
						jetcad.RegisterUndo();
						var Selected = jetcad_tools.getSelected();
						console.log(Selected.length);
						var radius = jetcad.get_input_box_value("radius");
						for (var x = 1; x < Selected.length; x++)
						{
							var arc = jetcad.MakerJS.path.fillet(Selected[x-1], Selected[x], radius);
							console.log(arc);
							if (arc)
							{
								//console.log(arc);
								jetcad.Stack.push(arc);
							}
							else
							{
								notification.push("Can't create fillet (" + radius + " Inch)");
							}
						}
						//Lets see if firt and last have a fillet, ie we ware filleting a full rectangle
						if (Selected.length > 2)
						{
							var arc = jetcad.MakerJS.path.fillet(Selected[0], Selected[Selected.length - 1], radius);
							console.log(arc);
							if (arc)
							{
								//console.log(arc);
								jetcad.Stack.push(arc);
							}
						}
            jetcad_tools.Escape();
          };
          return e;
        });
				jetcad.render(true);
	}
	Chamfer()
	{

		jetcad.input_box("chamfer", 0.125, "in", { x: jetcad.ClientMouse.x + 20, y: jetcad.ClientMouse.y + 20 });
		jetcad.pushLive(function(m){
          var e = {};
					var rad = jetcad.get_input_box_value("chamfer");
					jetcad.clear_input_boxes();
          jetcad.input_box("chamfer", rad, "in", { x: jetcad.ClientMouse.x + 20, y: jetcad.ClientMouse.y + 20 });
          jetcad.Callbacks.onTab =function(){
            $("#chamfer").focus();
            $("#chamfer").select();
             jetcad.Callbacks.onTab = null;
          };
          jetcad.Callbacks.onEnter =function(){
						jetcad.RegisterUndo();
						var Selected = jetcad_tools.getSelected();
						console.log(Selected.length);
						var chamfer = jetcad.get_input_box_value("chamfer");
						for (var x = 1; x < Selected.length; x++)
						{
							var arc = jetcad.MakerJS.path.fillet(Selected[x-1], Selected[x], chamfer);
							console.log(arc);
							if (arc)
							{
								//console.log(arc);
								var start_line = jetcad.get_line_at_angle({ x: arc.origin[0], y: arc.origin[1] }, arc.startAngle, arc.radius);
								var end_line = jetcad.get_line_at_angle({ x: arc.origin[0], y: arc.origin[1] }, arc.endAngle, arc.radius);
								var chamfer_line = { type: "line", origin: start_line.end, end: end_line.end };
								jetcad.Stack.push(chamfer_line);
							}
							else
							{
								notification.push("Can't create chamfer (" + chamfer + " Inch)");
							}
						}
						//Lets see if firt and last have a fillet, ie we ware filleting a full rectangle
						if (Selected.length > 2)
						{
							var arc = jetcad.MakerJS.path.fillet(Selected[0], Selected[Selected.length - 1], chamfer);
							console.log(arc);
							if (arc)
							{
								var start_line = jetcad.get_line_at_angle({ x: arc.origin[0], y: arc.origin[1] }, arc.startAngle, arc.radius);
								var end_line = jetcad.get_line_at_angle({ x: arc.origin[0], y: arc.origin[1] }, arc.endAngle, arc.radius);
								var chamfer_line = { type: "line", origin: start_line.end, end: end_line.end };
								jetcad.Stack.push(chamfer_line);
							}
						}
            jetcad_tools.Escape();
          };
          return e;
        });
				jetcad.render(true);
	}
  DeleteSelected()
  {
		jetcad.RegisterUndo();
    var new_stack = [];
    for (var x = 0; x < jetcad.Stack.length; x++)
		{
      if (!jetcad.metaIsSelected(x))
      {
        new_stack.push(jetcad.Stack[x]);
      }
    }
    jetcad.Stack = new_stack;
    jetcad.render(true);
  }
  Escape()
  {
		//console.log("Escape!");
		$("#tool-settings").hide();
		jetcad.HandlersActive = true;
		jetcad.DisableHotkeys = false;
    jetcad.NextClick = [];
    jetcad.ToolPoints = [];
    jetcad.clearLive();
    jetcad.clear_input_boxes();
    jetcad.Callbacks.onEnter = null;
		jetcad.Callbacks.onMouse = null;
		jetcad.Callbacks.onTab = null;
    jetcad.StaticStack = "";
		$("#draw-search").hide();

    for (var x = 0; x < jetcad.Stack.length; x++)
		{
      jetcad.metaSelected(x, false);
    }
    jetcad.render(true);
    jetcad.SetStatusText("");
  }
	Undo()
	{
		jetcad.CommitUndo();
		jetcad_tools.Escape();
	}
  SelectAll()
  {
    for (var x = 0; x < jetcad.Stack.length; x++)
		{
			jetcad.metaSelected(x, true);
    }
    jetcad.render(true);
  }
	Offset()
	{
		jetcad.input_box("offset", 0.250, "in", { x: jetcad.ClientMouse.x + 20, y: jetcad.ClientMouse.y + 20 });
          jetcad.Callbacks.onTab =function(){
            $("#offset").focus();
            $("#offset").select();
             jetcad.Callbacks.onTab = null;
          };
		jetcad.Callbacks.onEnter =function(){
						jetcad.RegisterUndo();
						var Selected = jetcad_tools.getSelected(); //Get Selected in order
            var distance = jetcad.get_input_box_value("offset");
						jetcad.Stack = jetcad.Stack.concat(jetcad.offset_selected(distance));

						/*if (arc)
						{
							console.log(arc);
							jetcad.Stack.push(arc);
						}*/
            jetcad_tools.Escape();
          };
	}
	//Functions that are required by other drawing functions
	getSelected()
	{
		var UnsortedSelected = [];

		for (var x = 0; x < jetcad.Stack.length; x++)
		{
      if (jetcad.metaIsSelected(x))
      {
        UnsortedSelected.push({ entity: jetcad.Stack[x], time: jetcad.Stack[x].meta.SelectedTimeStamp});
      }
    }
		//return JSON.parse(JSON.stringify(Selected));
		UnsortedSelected.sort(function(a, b) {
    	return a.time - b.time;
		});
		var Selected = [];
		for (var x = 0; x < UnsortedSelected.length; x++)
		{
			Selected.push(UnsortedSelected[x].entity);
		}
		return Selected;
	}
}
