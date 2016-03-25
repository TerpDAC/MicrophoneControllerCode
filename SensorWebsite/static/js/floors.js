var curFloorStatus = "";

function updateFloorStatus() {
    $.get("floorstatus", function(statusString) {
        floorStatuses = statusString.split(",");
        if (floorStatuses.length >= 2) {
            newFloorStatus = floorStatuses[1];
            
            if (newFloorStatus != curFloorStatus) {
                $('#floor2').removeClass("waves-green");
                $('#floor2').removeClass("waves-yellow");
                $('#floor2').removeClass("waves-red");
                
                if (newFloorStatus == "low") {
                    curClasses = $('#floor2').prop('class');
                    
                    $('#floor2').addClass("waves-green");
                    Waves.ripple('#floor2');
                    
                    if (curClasses.search("high") != -1) {
                        $('#floor2').switchClass("high", "low", 1000, "easeInOutQuad");
                    } else {
                        $('#floor2').switchClass("med", "low", 1000, "easeInOutQuad");
                    }
                    
                    $('#floor2 div:nth-child(2)').html('<i class="fa fa-volume-off"></i>');
                }
                
                if (newFloorStatus == "med") {
                    curClasses = $('#floor2').prop('class');
                    
                    $('#floor2').addClass("waves-yellow");
                    Waves.ripple('#floor2');
                    
                    if (curClasses.search("high") != -1) {
                        $('#floor2').switchClass("high", "med", 1000, "easeInOutQuad");
                    } else {
                        $('#floor2').switchClass("low", "med", 1000, "easeInOutQuad");
                    }
                    
                    $('#floor2 div:nth-child(2)').html('<i class="fa fa-volume-down"></i>');
                }
                
                if (newFloorStatus == "high") {
                    curClasses = $('#floor2').prop('class');
                    
                    $('#floor2').addClass("waves-red");
                    Waves.ripple('#floor2');
                    
                    if (curClasses.search("med") != -1) {
                        $('#floor2').switchClass("med", "high", 1000, "easeInOutQuad");
                    } else {
                        $('#floor2').switchClass("low", "high", 1000, "easeInOutQuad");
                    }
                    
                    $('#floor2 div:nth-child(2)').html('<i class="fa fa-volume-up"></i>');
                }
                
                curFloorStatus = newFloorStatus;
            }
        }
    });
}

function autoRefreshFloorStatus() {
    updateFloorStatus();
}

Waves.attach('#floor2', ['waves-block']);

setInterval(autoRefreshFloorStatus, 5000);
