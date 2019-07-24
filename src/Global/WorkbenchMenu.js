function appendWorkbenchMenu(menu)
{
  const {remote} = require('electron');
  const {Menu, MenuItem} = remote;
  menu.append(new MenuItem ({
    label: 'Workbench',
		submenu: [
			{ label: 'CAD',
			click: function() {
				require('electron').remote.getCurrentWindow().loadFile('../layout/CAD/CAD.html');
			}},
		]
 }));
}
