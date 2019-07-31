const electron = require('electron');
var $ = require('jquery');
var fs = require('fs');
let DxfParser = require('dxf-parser');
let DxfWriter = require('dxf-writer');
const Workbench = "CAD";
var CurrentFile = null;

var motion = new MotionPlanner();

var render = new ProfileRender();

function ParseDXF(data, part_name)
{
  var parser = new DxfParser();
  try {
      var imported_stack = [];
      var dxf = parser.parseSync(data);
      for (var i = 0; i < dxf.entities.length; i++)
      {
        if (dxf.entities[i].type == "CIRCLE")
        {
          imported_stack.push({ type: "circle", origin: [dxf.entities[i].center.x, dxf.entities[i].center.y], radius: dxf.entities[i].radius, meta: render.copy_obj(render._defaultMeta)});
        }
        else if (dxf.entities[i].type == "LINE")
        {
          imported_stack.push({ type: "line", origin: [dxf.entities[i].vertices[0].x, dxf.entities[i].vertices[0].y], end: [dxf.entities[i].vertices[1].x, dxf.entities[i].vertices[1].y], meta: render.copy_obj(render._defaultMeta) });
        }
        else if (dxf.entities[i].type == "ARC")
        {
          imported_stack.push({ type: "arc", origin: [dxf.entities[i].center.x, dxf.entities[i].center.y], startAngle: render.to_degrees(dxf.entities[i].startAngle), endAngle: render.to_degrees(dxf.entities[i].endAngle), radius: dxf.entities[i].radius, meta: render.copy_obj(render._defaultMeta)});
        }
      }
      var part = render.newPart(part_name);
      part.entities = imported_stack;
      render.Stack.push(part);
  }catch(err) {
      return console.error(err.stack);
  }
}
function zoom(zoomFactor)
{
  camera.fov *= zoomFactor;
  camera.updateProjectionMatrix();
}
function ImportDrawing()
{
	const {dialog} = electron.remote;
	dialog.showOpenDialog({
        properties: ['openFile', 'multiSelections'],
				filters: [
								{ name: 'DXF', extensions: ['dxf'] },
								{ name: 'SVG', extensions: ['svg'] },
							]
    }, function (files) {
        if (files !== undefined) {
        	files.forEach(function(item, index, arr){
						if (item.includes(".dxf"))
						{
							console.log("Opening DXF File: " + item);
							fs.readFile(item, 'utf-8', (err, data) => {
								if(err){
										alert("An error ocurred reading the file :" + err.message);
										return;
								}
                var path = require("path");
                var filepath = item;
                var name = path.parse(filepath).name;
								ParseDXF(data, name);
							});
						}
					});
        }
    });
}
function CreateMenu()
{
	const {remote} = require('electron');
  const {Menu, MenuItem} = remote;

  const menu = new Menu();
  menu.append(new MenuItem ({
    label: 'File',
		submenu: [
			{ label: 'New',
			click: function() {
				NewDrawing();
			}},
			{ label: 'Open',
			click: function() {
				OpenDrawing();
			}},
			{ label: 'Import',
			click: function() {
				ImportDrawing();
			}},
			{ label: 'Save',
			click: function() {
				SaveDrawing();
			}},
			{ label: 'Save As',
			click: function() {
				SaveDrawingAs();
			}},
			{ label: 'Debug',
			click: function() {
				require('electron').remote.getCurrentWindow().toggleDevTools();
			}},
			{ label: 'Reload',
			click: function() {
				require('electron').remote.getCurrentWindow().reload();
			}}
		]
 }));
 appendWorkbenchMenu(menu);
	Menu.setApplicationMenu(menu);
}
function animate()
{
  //render.controls.update();
  motion.motion_tick();
  requestAnimationFrame ( animate );
  render.renderer.render (render.scene, render.camera);
}
function test()
{
  motion.push_target({x: 12.1554, y: 1.2514, f: 75});
  motion.push_target({x: 5.6991, y: 3.8114, f: 30});
  motion.push_target({x: 0, y: 0, f: 50});
  motion.plan_moves_on_stack_and_run();
}
function main()
{
	CreateMenu();
  render._renderHeight = window.innerHeight - 125;
	render.init();
  Terminal_Init();
  animate();

  motion.Callbacks.Step_X = function(dir){
    render.Stack[0].offset[0] = motion.get_current_position().x;
    render.Stack[0].offset[1] = motion.get_current_position().y;
    render.Stack[0].updateRender = true;
  };
  motion.Callbacks.Step_Y = function(dir){
    render.Stack[0].offset[0] = motion.get_current_position().x;
    render.Stack[0].offset[1] = motion.get_current_position().y;
    render.Stack[0].updateRender = true;
  };


  //Autoload a file, just to make initial development cycle faster
  var item = "test/dxf/test.dxf";
  fs.readFile(item, 'utf-8', (err, data) => {
    if(err){
        alert("An error ocurred reading the file :" + err.message);
        return;
    }
    var path = require("path");
    var filepath = item;
    var name = path.parse(filepath).name;
    ParseDXF(data, name);
  });
}
$( document ).ready(function() {
    main();
});
