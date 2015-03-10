//var jsonObj = {0:"Add some tasks from the configuration page!", 1: 0};

/*var transactionId = Pebble.addEventListener("ready", function(e) {
                        console.log("Done getting Ready");
                         Pebble.sendAppMessage(jsonObj,
                                              function(e) {
                                                console.log('Successfully delivered message with transactionId='+ e.data.transactionId);
                                               },
                                                function(e) {
                                                  console.log('Unable to deliver message with transactionId=' + e.data.transactionId+ ' Error is: ');
                                                });
});*/

Pebble.addEventListener('ready',
  function(e) {  
    console.log("IN READY JAVASCRIPT SHIIIII");
    Pebble.sendAppMessage({ 0:'This is 21 characters' , 1:543, 2:"Next test", 3:770, 4:'Dishes', 5:333,
                          });
  });

                                               

Pebble.addEventListener("appmessage",
                       function(e){
                          console.log("Got a message");
                       });

Pebble.addEventListener("showConfiguration",
                       function(e){
                         Pebble.openURL("http://gurtajkhatra.github.io/TimeToDoConfig.html");
                       });

Pebble.addEventListener('webviewclosed',
                       function(e){
                         var tempJson = JSON.parse(decodeURIComponent(e.response));
                         console.log(tempJson);
                         
                         
                       });
