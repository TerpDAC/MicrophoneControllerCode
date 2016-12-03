var curFloorStatus = "";

function updateFloorStatus() {
    $.get("floorstatus-dbg", function(dbgStatusString) {
        $('#dbgstatus').html(dbgStatusString);
    });

    $.get("floorstatus", function(statusString) {
        floorStatuses = statusString.split(",");
    
        for (int i = 0; i < floorStatuses.length; i++) {
            newFloorStatus = floorStatuses[i];
            
            if (newFloorStatus != curFloorStatus[i]) {
                $(sensorArr[i]).removeClass("waves-green");
                $(sensorArr[i]).removeClass("waves-yellow");
                $(sensorArr[i]).removeClass("waves-red");

                if (newFloorStatus == "low") {
                    curClasses = $(sensorArr[i]).prop('class');

                    $(sensorArr[i]).addClass("waves-green");
                    Waves.ripple(sensorArr[i]);

                    if (curClasses.search("high") != -1) {
                        $(sensorArr[i]).switchClass("high", "low", 1000, "easeInOutQuad");
                    } else {
                        $(sensorArr[i]).switchClass("med", "low", 1000, "easeInOutQuad");
                    }

                    $(sensorArr[i] + ' div:nth-child(2)').html('<i class="fa fa-volume-off"></i>');
                }

                if (newFloorStatus == "med") {
                    curClasses = $(sensorArr[i]).prop('class');

                    $(sensorArr[i]).addClass("waves-yellow");
                    Waves.ripple(sensorArr[i]);

                    if (curClasses.search("high") != -1) {
                        $(sensorArr[i]).switchClass("high", "med", 1000, "easeInOutQuad");
                    } else {
                        $(sensorArr[i]).switchClass("low", "med", 1000, "easeInOutQuad");
                    }

                    $(sensorArr[i] + ' div:nth-child(2)').html('<i class="fa fa-volume-down"></i>');
                }

                if (newFloorStatus == "high") {
                    curClasses = $(sensorArr[i]).prop('class');

                    $(sensorArr[i]).addClass("waves-red");
                    Waves.ripple(sensorArr[i]);

                    if (curClasses.search("med") != -1) {
                        $(sensorArr[i]).switchClass("med", "high", 1000, "easeInOutQuad");
                    } else {
                        $(sensorArr[i]).switchClass("low", "high", 1000, "easeInOutQuad");
                    }

                    $(sensorArr[i] + ' div:nth-child(2)').html('<i class="fa fa-volume-up"></i>');
                }

                curFloorStatus[i] = newFloorStatus;
            }
        }
    
    });
}

function autoRefreshFloorStatus() {
    updateFloorStatus();
}

var sensorArr = ['#sensor1', '#sensor2', '#sensor3', '#sensor4'];		//takes place of #floor2 variable in code (floor generalized to sensor)
var curFloorStatus = ['low', 'low', 'low', 'low'];				//create arr for current status for each sensor, all initialized to low
for(int i = 0; i < sensorArr.length; i++) {
    Waves.attach(sensorArr[i], ['waves-block']);
}

setInterval(autoRefreshFloorStatus, 5000);
