from flask import Flask, request, render_template

from MicSense import app
from MicSense.models import Data, Sensor
from MicSense.db import db

import datetime
from pytz import timezone

DEFAULT_MID_THRESH = 100
DEFAULT_HIGH_THRESH = 200

@app.route('/<path:path>')
def static_proxy(path):
    # send_static_file will guess the correct MIME type
    return app.send_static_file(path)

@app.route('/')
def hello():
    return render_template('data.html')

@app.route('/floor')
def floor():
    return render_template('floor.html')

@app.route('/floorstatus')
def floorstatus():
    '''Retrieves current data for every floor and evaluates the floor's status. If the low
    samples make up 50% of the data the floor is deemed "low". If the low samples are less
    than 50% and the high samples make up more than 25% of the total samples the floor is
    deemed "high". Otherwise the floor is deemed "med".'''
    #return ",".join(["low", ["low", "med", "high"][random.randrange(0,3)]] + (["low"]*5))
    
    #arrays that will store the information for each floor
    #index+1 corresponds to the floor where the data was collected
    total = [0]*7
    high = [0]*7
    med = [0]*7
    low = [0]*7
    stat = ['']*7

    while (floorNum < 8): 
        if Data.query.count() == 0:
            return ",".join(["low"] * 7)
        else:
            for a in Sensor.query.filter_by(floor_num = floorNum):
                # Negative sign indicates reverse order (most recent)
                e = a.data.query.order_by('-timestamp').first()
                print("floorstatus: " + str(e.timestamp))
                #stores the total, high, med, and low values for each floor
                total[a.floor_num - 1] += e.high + e.med + e.low
                high[a.floor_num - 1] += e.high
                med[a.floor_num - 1] += e.med
                low[a.floor_num - 1] += e.low
        floor += 1
        
    for x in range(0,7):
        #if ((e[3] * 100) / total) >= 47:
        if (low[x] * 100) / total[x] >= 50 or total[x] == 0:
            stat[x] = "low"
        else:
            total_no_low = total[x] - low[x]
            if ((high[x] * 100) / total_no_low) >= 25:
                stat[x] = "high"
            else:
                stat[x] = "med"
    if not floorNum or floorNum > 7 or floorNum < 1:
        return ",".join(stat)
    else:
        return stat(floorNum)

@app.route('/floorstatus-dbg')
def floorstatusdbg():
    if Data.query.count() == 0:
        return "No data!"
    else:
        # Negative sign indicates reverse order (most recent)
        e = Data.query.order_by('-timestamp').first()
        print("floorstatusdbg: " + str(e.timestamp))
        total = e.high + e.med + e.low

        # high, med, low
        total_no_low = total - e.low

        low_percent = ((e.low * 100) / total)
        med_percent = ((e.med * 100) / total)
        high_percent = ((e.high * 100) / total)
        high_percent_in_nonlow = ((e.high * 100) / total_no_low) if total_no_low != 0 else 0
        med_percent_in_nonlow = ((e.med * 100) / total_no_low) if total_no_low != 0 else 0

        new_ts = e.timestamp.replace(tzinfo=timezone('UTC')).astimezone(timezone('US/Eastern'))

        return "Latest timestamp: %s | Low percent: %.2f%% | Med percent: %.2f%% | High percent: %.2f%% | Med percent (within med/high): %.2f%% | High percent (within med/high): %.2f%%" % (new_ts.strftime("%Y/%m/%d %H:%M:%S"), low_percent, med_percent, high_percent, med_percent_in_nonlow, high_percent_in_nonlow)

@app.route('/rawdata')
def rawdata():

    address = request.args.get('id', None)

    if address:
        current_time = datetime.datetime.utcnow()
        one_hour_ago = current_time - datetime.timedelta(hours=1)
        
        sensor = Sensor.query.filter_by(mac_address = address).first_or_404()
        output = 'Floor: '+ str(sensor.floor_num) + ', Location: ' + sensor.location + '\n'
        output += 'Date,High,Med,Low\n'
        for e in Sensor.data.query.filter_by(sensor.data.timestamp >= one_hour_ago).all():
            new_ts = e.timestamp.replace(tzinfo=timezone('UTC')).astimezone(timezone('US/Eastern'))
            print('Object: ' + str(e))
            print(new_ts.strftime("%Y/%m/%d %H:%M:%S"))
            buf = []
            buf.append(new_ts.strftime("%Y/%m/%d %H:%M:%S"))
            buf.append(str(e.high))
            buf.append(str(e.med))
            buf.append(str(e.low))
            output += ','.join(buf) + '\n'
    else:
        output = ''
        for i in range(1,8):
            for sensor in Sensor.query.filter_by(floor_num=i).all():
                output += 'Floor: ' + str(i) + ', Location: ' + sensor.location + 'Mac Address: ' + sensor.mac_address + '\n'
                for e in sensor.data.query.filter_by(sensor.data.timestamp >= one_hour_ago).all():
                    new_ts = e.timestamp.replace(tzinfo=timezone('UTC')).astimezone(timezone('US/Eastern'))
                    print('Object: ' + str(e))
                    print(new_ts.strftime("%Y/%m/%d %H:%M:%S"))
                    buf = []
                    buf.append(new_ts.strftime("%Y/%m/%d %H:%M:%S"))
                    buf.append(str(e.high))
                    buf.append(str(e.med))
                    buf.append(str(e.low))
                    output += ','.join(buf) + '\n'
    return output
        #for e in curdata:
##        for e in Data.query.filter(Data.timestamp >= one_hour_ago).all():
##            if e.timestamp:
##                new_ts = e.timestamp.replace(tzinfo=timezone('UTC')).astimezone(timezone('US/Eastern'))
##                print("OBJECT: " + str(e))
##                print(new_ts.strftime("%Y/%m/%d %H:%M:%S"))
##                buf = []
##                buf.append(new_ts.strftime("%Y/%m/%d %H:%M:%S"))
##                macAddress = (e.id_address)[0,2] + (e.id_address)[2,4] + (e.id_address)[4,6] + (e.id_address)[6,8] + (e.id_address)[8,10] + (e.id_address)[10,12]
##                buf.append(macAddress)
##                buf.append(str(e.high))
##                buf.append(str(e.med))
##                buf.append(str(e.low))
##                output += ",".join(buf) + "\n"


@app.route('/rawdataall')
def rawdataall():
    output = "Date,High,Med,Low\n"

    for e in Data.query.all():
        if e.timestamp:
            new_ts = e.timestamp.replace(tzinfo=timezone('UTC')).astimezone(timezone('US/Eastern'))
            print("OBJECT: " + str(e))
            print(new_ts.strftime("%Y/%m/%d %H:%M:%S"))
            buf = []
            buf.append(new_ts.strftime("%Y/%m/%d %H:%M:%S"))
            buf.append(str(e.high))
            buf.append(str(e.med))
            buf.append(str(e.low))
            output += ",".join(buf) + "\n"

    return output

@app.route('/datasubmit')
def dataSubmit():
    '''Requests the data and Mac Address as parameters and submits the given information
    into the database. If an entry has already been made with the Mac Address the data is
    placed into that object's database. Otherwise a new Sensor object is created to store
    future submissions.'''
    d = request.args.get('d', None)
    #stores the macAddress as a string 
    id_address = request.args.get('id', None)

    if not d or not id_address:
        return "ERROR"

    data = d.split(",")
    #id_address = id_address.replace(":", "")

    print("INPUT INCOMING")
    print(d)
    print(data)

    #retrieves the sensor trying to submit data
    sensor = Sensor.query.filter_by(mac_address = id_address)

    timestamp = int(data[0])
    high = int(data[1])
    med = int(data[2])
    low = int(data[3])

    #new_data = [timestamp, high, med, low]
    #curdata.append(list(new_data))

    new_data = Data(timestamp = datetime.datetime.fromtimestamp(timestamp).replace(tzinfo=timezone('UTC')), high = high, med = med, low = low)
    if not sensor:
        sensor = Sensor(floor_num = 2, location = 'Near the stairs',
            mac_address = id_address,
            mid_thresh = DEFAULT_MID_THRESH,
            high_thresh = DEFAULT_HIGH_THRESH)
        db.session.add(sensor)
    
    sensor.data.append(new_data)
    db.session.add(new_data)
    #db.session.add(sensor) do I need to add every time I change something?
    db.session.commit()

    return "SenseOK:%i:%i" % (sensor.mid_thresh, sensor.high_thresh)

@app.route('/calibrate')
def calibrate():
    d = request.args.get('d', None)
    #stores the macAddress as a string 
    id_address = request.args.get('id', None)

    if not d or not id_address:
        return "ERROR"

    data = d.split(",")
    #id_address = id_address.replace(":", "")

    #retrieves the sensor trying to submit data
    sensor = Sensor.query.filter_by(mac_address = id_address)

    if not sensor:
        sensor = Sensor(floor_num = 2, location = 'Near the stairs',
            mac_address = id_address,
            mid_thresh = DEFAULT_MID_THRESH,
            high_thresh = DEFAULT_HIGH_THRESH)
        db.session.add(sensor)
    
    db.session.commit()

    return "SenseOK:%i:%i" % (sensor.mid_thresh, sensor.high_thresh)
