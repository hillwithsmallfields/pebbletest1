Pebble.addEventListener("ready",
			function(e) {
			    // all initialization to be done in here
			    // todo: load lines from file
			    console.log("JavaScript app ready and running!");
			}
		       );

Pebble.addEventListener("appmessage",
			function(e) {
			    console.log("Received message: " + e.payload);
			    // todo: switch on message type, which can be a request for data, or a chagne of status
			}
		       );

"appKeys": {
  "setSizes":  0,
  "dataLine": 1
}

Pebble.sendAppMessage({"setSizes": "4 64" });
Pebble.sendAppMessage({"dataLine": "Hello"});
Pebble.sendAppMessage({"dataLine": "world"});
