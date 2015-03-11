var jsonObj = {0:"Shsj",1:1484, 2:"GARGE", 3:433, 4:"dhishe", 5:32};


Pebble.addEventListener('ready',
  function(e) {  
    //Pebble.sendAppMessage({ 0:'This is 21 characters' , 1:10, 2:"Next test", 3:770, 4:'Dishes', 5:333,});
      Pebble.sendAppMessage(jsonObj);
  });

                                               

Pebble.addEventListener("appmessage",
                       function(e){
                          console.log("Got a message");
                       });

Pebble.addEventListener("showConfiguration",
                       function(e){
                         Pebble.openURL("http://gurtajkhatra.github.io/TimeToDo%20configuration%20Page/TimeToDoConfig.html");
                       });


Pebble.addEventListener('webviewclosed',
  function(e) {
      var oldJson = jsonObj;
      var configuration = JSON.parse(decodeURIComponent(e.response));
      console.log("e.response: " + e.response);
      console.log("After decoding: " + decodeURIComponent(e.response));
      console.log("After JSON.parse " + configuration);
      configuration = JSON.parse(configuration); 
      jsonObj = '{';
      for(var i in configuration.tasks) {
          console.log(configuration.tasks[i].name);
          console.log(configuration.tasks[i].time);
          jsonObj = jsonObj + '"' + (2*i) + '"' + ':"' + configuration.tasks[i].name + '",';
          jsonObj = jsonObj + '"' + (2*i+1) + '"' + ':' + configuration.tasks[i].time + ',';      
      }
      
      jsonObj = jsonObj.substring(0,jsonObj.length-1) + '}';
      console.log("OLD JSON: " + oldJson);
      console.log("NEW JSON: " + jsonObj);
      jsonObj = JSON.parse(jsonObj);
      
      Pebble.sendAppMessage(jsonObj);
      
  }
);

