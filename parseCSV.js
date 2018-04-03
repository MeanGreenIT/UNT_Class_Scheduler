/*
* Author: Jesse Culver
* Built on top of the code provided by MounirMesselmeni https://github.com/MounirMesselmeni/html-fileapi
*/

function handleFiles(files)
{
	//check for API support
	if(window.FileReader)
	{
		//Start processing data
		getAsText(files[0]);
	}
	else
	{
		alert('You are using an older web browser that is not supported, please use something newer.');
	}
}

//This reads the file into memory
function getAsText(fileToRead)
{
	var reader = new FileReader();
	//Read file into memory
	reader.readAsText(fileToRead);
	//Error handling
	reader.onload = loadHandler;
	reader.onerror = errorHandler;
}

//On load set what we just read in as csv then send it off to processing
function loadHandler(event)
{
	var csv = event.target.result;
	processData(csv);
}

//Process the csv file that was just passed in
function processData(csv)
{
	//Split the csv file first by returns and newlines
	var allTextLines = csv.split(/\r\n|\n/);
	//lines is where we will be storing each line of the csv file
	var lines = [];
	//This not only splits by line but also by , for nested arrays
	while (allTextLines.length) {
        lines.push(allTextLines.shift().split(','));
    }
    //Splice blank lines out as well as clean up input to remove non alphanumerics from certain columns
    for(i = 0; i < lines.length; i++)
    {
      //blank line cleanup
    	if(lines[i][0] == '')
    	{
    		console.log(i+" is a blank line, removing it.");
    		lines.splice(i, 1);
    	}
      //alphanumeric cleanup
    	if(lines[i].length > 15)
    	{
    		lines[i][11] = lines[i][11] + " " + lines[i][12];
    		lines[i][11] = lines[i][11].replace(/['"]+/g, '');
    		lines[i][12] = lines[i][13];
    		lines[i][13] = lines[i][14];
    		lines[i][14] = lines[i][15];
    		lines[i].pop();
    	}
    }
	console.log(lines);
}

//Handle errors here
function errorHandler(evt)
{
	if(evt.target.error.name == "NotReadableError")
	{
		alert("Canno't read file!");
	}
}
