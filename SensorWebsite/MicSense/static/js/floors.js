function updateFloorStatus() {
    $.get("floorstatus-dbg", function(dbgStatusString) {
        $('#dbgstatus').html(dbgStatusString);
    });

    $.get("floorstatus", function(statusString) {
        floorStatuses = statusString.split(",");
        var i = 0;
        for (i = 0; i < floorStatuses.length; i++) {
            if (floorStatuses[i] != curFloorStatus[i]) {
                volChange[i] = 1;
            }
        }
        
        for (i = 0; i < volChange.length; i++) {
            if (volChange[i] == 1) {
                newFloorStatus = floorStatuses[i];
            
                $(floorArr[i]).removeClass("waves-green");
                $(floorArr[i]).removeClass("waves-yellow");
                $(floorArr[i]).removeClass("waves-red");
                
                switch (newFloorStatus) {
                    case 'low':
                        curClasses = $(floorArr[i]).prop('class');

                        $(floorArr[i]).addClass("waves-green");
                        Waves.ripple(floorArr[i]);

                        if (curClasses.search("high") != -1) {
                            $(floorArr[i]).switchClass("high", "low", 1000, "easeInOutQuad");
                        } else {
                            $(floorArr[i]).switchClass("med", "low", 1000, "easeInOutQuad");
                        }

                        $(floorArr[i] + ' div:nth-child(2)').html('<i class="fa fa-volume-off"></i>');
                        break;

                    case 'med':
                        curClasses = $(floorArr[i]).prop('class');

                        $(floorArr[i]).addClass("waves-yellow");
                        Waves.ripple(floorArr[i]);

                        if (curClasses.search("high") != -1) {
                            $(floorArr[i]).switchClass("high", "med", 1000, "easeInOutQuad");
                        } else {
                            $(floorArr[i]).switchClass("low", "med", 1000, "easeInOutQuad");
                        }

                        $(floorArr[i] + ' div:nth-child(2)').html('<i class="fa fa-volume-down"></i>');
                        break;

                    case 'high':
                        curClasses = $(floorArr[i]).prop('class');

                        $(floorArr[i]).addClass("waves-red");
                        Waves.ripple(floorArr[i]);

                        if (curClasses.search("med") != -1) {
                            $(floorArr[i]).switchClass("med", "high", 1000, "easeInOutQuad");
                        } else {
                            $(floorArr[i]).switchClass("low", "high", 1000, "easeInOutQuad");
                        }

                        $(floorArr[i] + ' div:nth-child(2)').html('<i class="fa fa-volume-up"></i>');
                    
                }
                curFloorStatus[i] = newFloorStatus;
                volChange[i] = 0;
            }
        }
    
    });
}

function autoRefreshFloorStatus() {
    updateFloorStatus();
}
//takes place of #floor2 variable in code (floor generalized to sensor)
var floorArr = ['#floor1', '#floor2', '#floor3', '#floor4', '#floor5', '#floor6', '#floor7'];
//create arr for current status for each sensor, all initialized to low
var curFloorStatus = ['low', 'low', 'low', 'low', 'low', 'low', 'low'];
var volChange = [0,0,0,0,0,0,0]; //0 if floor stayed the same, 1 if change
var i = 0;
for(i = 0; i < floorArr.length; i++) {
    Waves.attach(floorArr[i], ['waves-block']);
}

setInterval(autoRefreshFloorStatus, 5000);
autoRefreshFloorStatus();
