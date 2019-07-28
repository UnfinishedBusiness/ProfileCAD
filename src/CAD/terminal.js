var Terminal;
var on_tab_key = null;
var getline_active = false;
var getline_callback = null;
var getline_unfocus_on_finish = null;
const red = "\u001b[31m";
const green = "\u001b[32m"
const blue = "\u001b[34m";
const white = "\u001b[37m";
const reset = "\u001b[0m";
const PS1 = "\r" + red + "ProfileCAD" + blue + "$ " + reset;
const left_code = "\u001b[{n}C";
const right_code = "\u001b[{n}D";

var terminal_commands = [
	{ description: "Clear Terminal Screen", cmd: "clear", run: function(args){ Terminal.clear(); ret(); } },
	{ description: "Close Terminal", cmd: "exit", run: function(args){ Terminal_UnFocus(); ret();} },
	{ description: "Evaluate math expression", cmd: "eval", run: function(args){ eval_command(args); } },
	{ description: "List Parts in Model", cmd: "list_parts", run: function(args){ list_parts(args); } },
	{ description: "Set View", cmd: "view", run: function(args){ view(args); } },
	{ description: "Display this menu", cmd: "help", run: function(args){ help(); } },
	{ description: "History", cmd: "history", run: function(args){ history(); } },
	{ description: "Draw a line", hotkey: "l", cmd: "line", run: function(args){ DrawLine(); } },
	{ description: "Draw a circle", hotkey: "c", cmd: "circle", run: function(args){ DrawCircle(); } },
	{ description: "Delete selected entites", hotkey: "delete", cmd: "delete", run: function(args){ DeleteSelected(); } },
	{ description: "Chain Select", hotkey: "space", cmd: "chain_select", run: function(args){ ChainSelect(); } },
	{ special: true, cmd: "control-c", run: function(args){ printf("Terminating!\n"); } },
	{ special: true, cmd: "tab-complete", run: function(args){ tab_complete(args); } },
	{ special: true, cmd: "up-arrowkey", run: function(args){ UpArrow(); } },
	{ special: true, cmd: "down-arrowkey", run: function(args){ DownArrow(); } },
];
var command_history = [];
var command_history_counter = 0;
var command_before_arrow = "";
function ret()
{
	Terminal.write(PS1);
}
function printf(stdout)
{
	Terminal.write(stdout);
}
function LeftArrow()
{

}
function RightArrow()
{

}
function UpArrow()
{
	console.log("Up arrow!");
	if (command_history_counter == 0) command_before_arrow = TerminalLineBuffer;
	command_history_counter++;
	if (command_history_counter > command_history.length) command_history_counter = command_history.length;
	//console.log("command_history_counter: " + command_history_counter);
	if (command_history[command_history.length - command_history_counter] != undefined)
	{
		printf("\r                                                                        \r");
		printf(PS1 + command_history[command_history.length - command_history_counter]);
		TerminalLineBuffer = command_history[command_history.length - command_history_counter];
	}
}
function DownArrow()
{
	console.log("Down arrow!");
	command_history_counter--;
	if (command_history_counter < 0) command_history_counter = 0;
	//console.log("command_history_counter: " + command_history_counter);
	if (command_history_counter == 0)
	{
		printf("\r                                                                        \r");
		TerminalLineBuffer = command_before_arrow;
		printf(PS1 + TerminalLineBuffer);
	}
	else
	{
		printf("\r                                                                        \r");
		printf(PS1 + command_history[command_history.length - command_history_counter]);
		TerminalLineBuffer = command_history[command_history.length - command_history_counter];
	}
}
function save_parameters()
{
	printf("Saving machine parameters!\n");
	MachineParameters_Save();
	ret();
}
function view(args)
{
	if (args[1] == "XY")
	{
		printf("Setting to XY View!\n");
		render.camera.rotation.set(0, 0, 0);
		render.camera.position.set(0, 0, 100);
	}
	ret();
}
function list_parts(args)
{
	for (var x = 0; x < render.Stack.length; x++)
	{
			printf(render.Stack[x].part_name + "\n\r");
	}
	ret();
}
function history()
{
	for (var x = 0; x < command_history.length; x++)
	{
			printf(command_history[x] + "\n\r");
	}
	ret();
}
function help()
{
	for (var x = 0; x < terminal_commands.length; x++)
	{
		if (terminal_commands[x].hotkey == undefined)
		{
			if (terminal_commands[x].description != undefined) printf(terminal_commands[x].cmd + " - " + terminal_commands[x].description + "\n\r");
		}
		else
		{
			if (terminal_commands[x].description != undefined) printf(terminal_commands[x].cmd + " (hotkey: " + terminal_commands[x].hotkey + ") - " + terminal_commands[x].description + "\n\r");
		}

	}
	ret();
}
function sleep(ms) {
  return new Promise(resolve => setTimeout(resolve, ms));
}
function getline(callback)
{
	getline_unfocus_on_finish = false;
	if (Terminal.isFocused == false) getline_unfocus_on_finish = true;
	Terminal_Focus(); //Makse sure we have focus so that key input is directed to TerminalLineBuffer;
	getline_active = true; //Make sure key input knows that enter is not to eval command, but to switch flag false
	getline_callback = callback;
}
function tab_complete(args)
{
	//console.log("Tab_Complete: " + TerminalLineBuffer);
	var search = TerminalLineBuffer;
	var tools = JSON.parse(JSON.stringify(terminal_commands));
	for (var i = 0; i < tools.length; i++)
	{
		var score = 0;
		for (var x = 0; x < tools[i].cmd.length; x++)
		{
			if (search[x] == tools[i].cmd[x] && tools[i].special == undefined) score += 1;
		}
		tools[i].score = score;
	}
	//console.log(tools);
	var winner = "";
	var bar = 0;
	for (var i = 0; i < tools.length; i++)
	{
		if (tools[i].score > bar)
		{
			bar = tools[i].score;
			winner = tools[i].cmd;
		}
	}
	printf(PS1 + winner);
	TerminalLineBuffer = winner;
}
function eval_command(args)
{
	args[0] = "";
	var eval_string = args.join(" ");
	printf(eval(eval_string) + "\n");
	ret();
}
function Terminal_Hotkey_Eval(key)
{
	for (var x = 0; x < terminal_commands.length; x++)
	{
		if (terminal_commands[x].hotkey == key)
		{
			printf(terminal_commands[x].cmd + "\n\r");
			Terminal_Eval(terminal_commands[x].cmd);
			return;
		}
	}
}
function Terminal_Eval(cmd_buffer)
{
	//console.log("Command buffer: " + cmd_buffer);
	if (cmd_buffer == "")
	{
		ret();
		return;
	}
	var args = cmd_buffer.split(' ');
	for (var x = 0; x < terminal_commands.length; x++)
	{
		if (terminal_commands[x].cmd == args[0])
		{
			terminal_commands[x].run(args);
			if (terminal_commands[x].special == undefined)
			{
				command_history.push(cmd_buffer);
				command_history_counter = 0;
			}
			return;
		}
	}
	printf("Command not found!\n");
	ret();
}
function Terminal_Init()
{
	Terminal.applyAddon(fullscreen);
	Terminal.applyAddon(webLinks);
  Terminal.applyAddon(fit);
	Terminal = new Terminal({
		allowTransparency: true,
		fontFamily: `'Fira Mono', monospace`,
    fontSize: 15,
	});
  Terminal.open(document.getElementById('terminal'));
	//Terminal.toggleFullScreen(true);
	$("#terminal").css({'opacity': 0.7}).css({'position': 'absolute'}).css({'width': "100%"}).css({'z-index': 100});
	Terminal.fit();
	Terminal.resize(120, 7);
  Terminal.write(PS1);
	TerminalLineBuffer = "";
	Terminal.on('key', (key, ev) => {
        console.log("Keycode: " + key.charCodeAt(0) + " Key: " + key);
				if (Terminal.isFocused== true)
				{
					if (key.charCodeAt(0) == 13)
					{
						Terminal.write('\n\r');
						if (getline_active == false)
						{
							Terminal_Eval(TerminalLineBuffer);
							TerminalLineBuffer = "";
						}
						else
						{
							getline_active = false;
							getline_callback(JSON.parse(JSON.stringify(TerminalLineBuffer)));
							if (getline_unfocus_on_finish == true) Terminal_UnFocus();
							TerminalLineBuffer = "";
						}
					}
					else if (key.charCodeAt(0) == 27) //escape
					{
						Terminal_UnFocus();
					}
					else if (key.charCodeAt(0) == 127)
					{
						if (TerminalLineBuffer.length > 0)
						{
								Terminal.write('\b \b');
								TerminalLineBuffer = TerminalLineBuffer.substring(0, TerminalLineBuffer.length - 1);
						}
					}
					else if (key.charCodeAt(0) == 3) //control-c
					{
						Terminal.write('\n\r');
						Terminal_Eval("control-c");
						TerminalLineBuffer = "";
					}
					else if (key.charCodeAt(0) == 9) //tab
					{
						if (on_tab_key != null)
						{
							if (on_tab_key() == true) on_tab_key = false;
							return;
						}
						else
						{
							Terminal_Eval("tab-complete");
						}
					}
					else if (key.charCodeAt(0) == 27 && key.includes("A")) //up arrow
					{
						Terminal_Eval("up-arrowkey");
					}
					else if (key.charCodeAt(0) == 27 && key.includes("B")) //down arrow
					{
						Terminal_Eval("down-arrowkey");
					}
					else if (key.charCodeAt(0) == 27 && key.includes("C")) //Left arrow
					{

					}
					else if (key.charCodeAt(0) == 27 && key.includes("D")) //Left arrow
					{

					}
					else
					{
						Terminal.write(key);
						TerminalLineBuffer += key;
					}
				}
				else
				{
					if (key == "s")
					{
						Terminal_Focus();
					}
					else if (key.charCodeAt(0) == 27) //escape
					{
						UnSelectAll();
						hideSnapPointer();
						render.cancelAllEntityCallbacks();
						printf("\r\nCanceled!\n\r");
						ret();
					}
					else if (key.charCodeAt(0) == 9)
					{
						if (on_tab_key != null)
						{
							if (on_tab_key() == true) on_tab_key = false;
							return;
						}
					}
					else if (key.charCodeAt(0) == 127)
					{
						Terminal_Hotkey_Eval("delete");
					}
					else if (key.charCodeAt(0) == 4)
					{
						Terminal_Hotkey_Eval("delete");
					}
					else if (key.charCodeAt(0) == 32)
					{
						Terminal_Hotkey_Eval("space");
					}
					else
					{
							Terminal_Hotkey_Eval(key);
					}
				}
	});
	Terminal.focus();
	Terminal_UnFocus();
}
function return_terminal_focus()
{
	if (Terminal.wasFocused == false) Terminal_UnFocus();
}
function save_terminal_focus()
{
	Terminal.wasFocused = false;
	if (Terminal.isFocused == true) Terminal.wasFocused = true;
}
function Terminal_Focus()
{
	Terminal.isFocused = true;
	Terminal.setOption('theme', {
	    background: '#000000'
	});
}
function Terminal_UnFocus()
{
	Terminal.isFocused = false;
	Terminal.setOption('theme', {
	    background: '#2a0a2e'
	});
}
