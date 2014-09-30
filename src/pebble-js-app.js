var nextLine = 0;
var charCount = 1024;
var lineCount = 16;

var last_id = 0;

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

function sendLineSucceeded(e) {
    console.log("Successfully delivered message with transactionId=" + e.data.transactionId);
}

function sendLineFailed(e) {
    console.log("Unable to deliver message with transactionId=" + e.data.transactionId + " Error is: " + e.error.message);
}

function sendLine(line_index) {
    var send_data =  { "lineIndex": line_index, "lineText": lines[line_index] };
    var transactionId = Pebble.sendAppMessage(send_data, sendLineSucceeded, sendLineFailed);
    last_id = transactionId;
}

function initializer(e) {
    // all initialization to be done in here
    // todo: load lines from file
    console.log("JavaScript menutest1 app ready and running; Lines: " + lines.length);
}

function responder(e) {
    console.log("menutest1 responder called");
    // if (e.payload.command !== null) {
    //     console.log("Received message with command: " + e.payload.command);
    //     // todo: switch on message type, which can be a request for data, or a change of status
    //     if (nextLine < 0) {
    //         console.log("Sending sizes chars=" + charCount + "; lines=" + lineCount);
    //         Pebble.sendAppMessage({"setSpace": charCount, "setLines": lineCount});
    //         nextLine = 0;
    //     } else if (nextLine < lines.length) {
    //         console.log("sending next line which is line " + nextLine);
    //         sendLine(nextLine);
    //         nextLine += 1;
    //     } else {
    //         console.log("marking end of file");
    //         Pebble.sendAppMessage({"allDone": 1});
    //     }
    // } else {
    //     console.log("Received message without command");
    //     /// appMessageQueue.clear(); // where is this defined?
    //     Pebble.sendAppMessage({"huh": 1});
    // }
}

Pebble.addEventListener("ready", initializer);
Pebble.addEventListener("appmessage", responder);
