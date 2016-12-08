from flask import Flask, request, render_template

from MicSense import app
from MicSense.models import Data
from MicSense.db import db

import datetime
from pytz import timezone

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
    #return ",".join(["low", ["low", "med", "high"][random.randrange(0,3)]] + (["low"]*5))

    floorNum = request.args.get('floor', None)
    #index corresponds to floor, index 0 should not be used
    total = [0]*8
    high = [0]*8
    med = [0]*8
    low = [0]*8
    stat = ['']*8
    if Data.query.count() == 0:
        return ",".join(["low"] * 7)
    else:
        for a in Hardware.query.all():
            # Negative sign indicates reverse order (most recent)
            e = a.isolated_data.query.order_by('-timestamp').first()
            print("floorstatus: " + str(e.timestamp))
            #stores the total, high, med, and low values for each floor
            total[a.floor_num] += e.high + e.med + e.low
            high[a.floor_num] += e.high
            med[a.floor_num] += e.med
            low[a.floor_num] += e.low
    
    for x in range(1,8):
        #if ((e[3] * 100) / total) >= 47:
        if (low[x] * 100) / total[x] >= 50:
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

    if not address:
        output = "Mac Address,Floor,Date,High,Med,Low\n"

        current_time = datetime.datetime.utcnow()
        one_hour_ago = current_time - datetime.timedelta(hours=1)

        for a in Hardware.query.all():
            for e in a.isolated_data.query.filter(Hardware.isolated_data.timestamp >= one_hour_ago).all():
                if e.timestamp
                    new_ts = e.timestamp.replace(tzinfo=timezone('UTC')).astimezone(timezone('US/Eastern'))
                    print("OBJECT: " + str(e))
                    print(new_ts.strftime("%Y/%m/%d %H:%M:%S"))
                    buf = []
                    macAddress = (e.id_address)[0,2] + ':' + (e.id_address)[2,4] + ':' + (e.id_address)[4,6] + ':' + (e.id_address)[6,8] + ':' + (e.id_address)[8,10] + ':' + (e.id_address)[10,12]
                    buf.append(macAddress)
                    buf.append(a.floor)
                    buf.append(new_ts.strftime("%Y/%m/%d %H:%M:%S"))
                    buf.append(str(e.high))
                    buf.append(str(e.med))
                    buf.append(str(e.low))
                    output += ",".join(buf) + "\n"                
    else:
        if Hardware.query.filter(address):
            a = Hardware.query.filter(address).all()
            for e in a.isolated_data.query.filter(a.timestamp >= one_hour_ago).all():
                if e.timestamp
                    new_ts = e.timestamp.replace(tzinfo=timezone('UTC')).astimezone(timezone('US/Eastern'))
                    print("OBJECT: " + str(e))
                    print(new_ts.strftime("%Y/%m/%d %H:%M:%S"))
                    buf = []
                    macAddress = (address)[0,2] + ':' + (address)[2,4] + ':' + (address)[4,6] + ':' + (address)[6,8] + ':' + (address)[8,10] + ':' + (address)[10,12]
                    buf.append(macAddress)
                    buf.append(a.floor)
                    buf.append(new_ts.strftime("%Y/%m/%d %H:%M:%S"))
                    buf.append(str(e.high))
                    buf.append(str(e.med))
                    buf.append(str(e.low))
                    output += ",".join(buf) + "\n"

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
    d = request.args.get('d', None)
    #stores the macAddress as a string 
    id_address = request.args.get('id', None)

    if not d or not id_address:
        return "ERROR"

    data = d.split(",")
    id_address = id_address.replace(":", "")

    print("INPUT INCOMING")
    print(d)
    print(data)

    timestamp = int(data[0])
    high = int(data[1])
    med = int(data[2])
    low = int(data[3])

    #new_data = [timestamp, high, med, low]
    #curdata.append(list(new_data))

    new_data = Data(timestamp = datetime.datetime.fromtimestamp(timestamp).replace(tzinfo=timezone('UTC')), high = high, med = med, low = low)
    db.session.add(new_data)
    db.session.commit()

    return "OK"
