Pebble.addEventListener("ready",
			function(e) {
			    // all initialization to be done in here
			    console.log("JavaScript app ready and running!");
			}
		       );

Pebble.addEventListener("appmessage",
			function(e) {
			    console.log("Received message: " + e.payload);
			}
		       );

"appKeys": {
  "setSizes":  0,
  "dataLine": 1
}

Pebble.sendAppMessage({"setSizes": "4 64" });
Pebble.sendAppMessage({"dataLine": "Hello"});
Pebble.sendAppMessage({"dataLine": "world"});
