var next_line = -1;
var char_count = 1024;
var line_count = 16;

var lines = [
    "a haon",
    "a do",
    "a tri",
    "a ceathair",
    "a cuig",
    "a se",
    "a seacht",
    "a hocht",
    "a naoi",
    "deich",
    "aon deag",
    "do dheag"
];

function sendLineSucceeded(e)
{
    console.log("Successfully delivered message with transactionId=" + e.data.transactionId);
}

function sendLineFailed(e)
{
    console.log("Unable to deliver message with transactionId=" + e.data.transactionId + " Error is: " + e.error.message);
}

function sendLine(line_index)
{
    var send_data =  { "lineIndex": line_index, "lineText": lines[line_index] }
    var transactionId = Pebble.sendAppMessage(send_data, sendLineSucceeded, sendLineFailed);
}

function initializer(e) 
{
    // all initialization to be done in here
    // todo: load lines from file
    console.log("JavaScript app ready and running!");
}

function responder(e) {
    console.log("Received message: " + e.payload);
    // todo: switch on message type, which can be a request for data, or a change of status
    if (next_line < 0) {
        Pebble.sendAppMessage({"setSpace": char_count, "setLines": line_count})
	next_line = 0;
    } else {
	if (lines[next_line]) {
	    sendLine(next_line++);
	}
    }
}

Pebble.addEventListener("ready", initializer);
Pebble.addEventListener("appmessage", responder);