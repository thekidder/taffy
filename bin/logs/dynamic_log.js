var divs;


function DynamicLog()
{
	var log = document.getElementById('log');
	divs = log.getElementsByTagName('div');
	for(i=0;i<divs.length;i++)
	{
		if(divs[i].className=="container")
		{
			var header = divs[i].getElementsByTagName('h3')[0];
			var link = document.createElement('a');
			var img = document.createElement('img');
			img.src = 'minus.gif';
			link.appendChild(img);
			link.href="#";
			eval('f = function() {CollapseAll('+i+');return false;}');
			link.onclick = f;
			divs[i].insertBefore(link, header);
		}
		if(divs[i].className=='critical' || divs[i].className=='error' || divs[i].className=='warning' || divs[i].className=='info' || divs[i].className=='debug1' || divs[i].className=='debug2')
		{
			divs[i].onclick = function(){Collapse(this);return false;};
		}
	}
	var controls = document.createElement('div');
	controls.id = 'controls';
	var header = document.createElement('h2');
	header.appendChild(document.createTextNode('Visibility controls'));
	controls.appendChild(header);
	var box = document.createElement('input');
	var text = document.createElement('span');
	text.className = 'critical';
	text.appendChild(document.createTextNode('Critical'));
	box.type = 'checkbox';
	box.checked = "checked";
	box.id = 'critical_control';
	box.onclick = function(){HideClass('critical');};
	controls.appendChild(box);
	controls.appendChild(text);
	
	box = document.createElement('input');
	text = document.createElement('span');
	text.className = 'error';
	text.appendChild(document.createTextNode('Error'));
	box.type = 'checkbox';
	box.checked = "checked";
	box.id = 'error_control';
	box.onclick = function(){HideClass('error');};
	controls.appendChild(box);
	controls.appendChild(text);
	
	box = document.createElement('input');
	text = document.createElement('span');
	text.className = 'warning';
	text.appendChild(document.createTextNode('Warning'));
	box.type = 'checkbox';
	box.checked = "checked";
	box.id = 'warning_control';
	box.onclick = function(){HideClass('warning');};
	controls.appendChild(box);
	controls.appendChild(text);
		
	box = document.createElement('input');
	text = document.createElement('span');
	text.className = 'info';
	text.appendChild(document.createTextNode('Info'));
	box.type = 'checkbox';
	box.checked = "checked";
	box.id = 'info_control';
	box.onclick = function(){HideClass('info');};
	controls.appendChild(box);
	controls.appendChild(text);
		
	box = document.createElement('input');
	text = document.createElement('span');
	text.className = 'debug1';
	text.appendChild(document.createTextNode('Debug 1'));
	box.type = 'checkbox';
	box.checked = "checked";
	box.id = 'debug1_control';
	box.onclick = function(){HideClass('debug1');};
	controls.appendChild(box);
	controls.appendChild(text);
		
	box = document.createElement('input');
	text = document.createElement('span');
	text.className = 'debug2';
	text.appendChild(document.createTextNode('Debug 2'));
	box.type = 'checkbox';
	box.checked = "checked";
	box.id = 'debug2_control';
	box.onclick = function(){HideClass('debug2');};
	controls.appendChild(box);
	controls.appendChild(text);
		
	log.parentNode.insertBefore(controls, log);
}

function CollapseAll(elementid)
{
	var link = divs[elementid].getElementsByTagName('a')[0];
	link.onclick = function() {ZeroAll(elementid);return false;};
	link.firstChild.src = 'zero.gif';
	var messages = divs[elementid].getElementsByTagName('div');
	for(i=0;i<messages.length;i++)
	{
		if(messages[i].className=='critical' || messages[i].className=='error' || messages[i].className=='warning' || messages[i].className=='info' || messages[i].className=='debug1' || messages[i].className=='debug2')
		{
			//messages[i].style.display = 'none';
			Collapse(messages[i]);
		}
	}
}

function ZeroAll(elementid)
{
	var link = divs[elementid].getElementsByTagName('a')[0];
	link.onclick = function() {ExpandAll(elementid);return false;};
	link.firstChild.src = 'plus.gif';
	var messages = divs[elementid].getElementsByTagName('div');
	for(i=0;i<messages.length;i++)
	{
		if(messages[i].className=='critical' || messages[i].className=='error' || messages[i].className=='warning' || messages[i].className=='info' || messages[i].className=='debug1' || messages[i].className=='debug2')
		{
			messages[i].style.display = 'none';
		}
	}
}

function ExpandAll(elementid)
{
	var link = divs[elementid].getElementsByTagName('a')[0];
	link.onclick = function() {CollapseAll(elementid);return false;};
	link.firstChild.src = 'minus.gif';
	/*var messages = divs[elementid].getElementsByTagName('div');
	for(i=0;i<messages.length;i++)
	{
		messages[i].style.display = 'block';
	}*/
	var messages = divs[elementid].getElementsByTagName('div');
	for(i=0;i<messages.length;i++)
	{
		if(messages[i].className=='critical' || messages[i].className=='error' || messages[i].className=='warning' || messages[i].className=='info' || messages[i].className=='debug1' || messages[i].className=='debug2')
		{
			if(document.getElementById(messages[i].className+'_control').checked == true)
				Expand(messages[i]);
		}
	}
}

function Collapse(element)
{
	element.onclick = function() {Expand(this);};
	var message = element.getElementsByTagName('div')[0];
	message.style.display = 'none';
}

function Expand(element)
{
	element.style.display = 'block';
	var message = element.getElementsByTagName('div')[0];
	message.style.display = 'block';
	element.onclick = function() {Collapse(this);};
}

function HideClass(class)
{
	document.getElementById(class+'_control').onclick = function() {ShowClass(class);};
	for(i=0;i<divs.length;i++)
	{
		if(divs[i].className == class)
		{
			divs[i].style.display = "none";
		}
	}
}

function ShowClass(class)
{
	document.getElementById(class+'_control').onclick = function() {HideClass(class);};
	for(i=0;i<divs.length;i++)
	{
		if(divs[i].className == class)
		{
			divs[i].style.display = "block";
		}
	}
}