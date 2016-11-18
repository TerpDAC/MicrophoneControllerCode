from flask import Flask, request, render_template

from MicSense import app
from MicSense.models import Data
from MicSense.db import db

import datetime
from pytz import timezone

# This is really cool!
# YAAAAAASS

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

    if Data.query.count() == 0:
        return ",".join(["low"] * 7)
    else:
        # Negative sign indicates reverse order (most recent)
        e = Data.query.order_by('-timestamp').first() #curdata[-1]
        print("floorstatus: " + str(e.timestamp))
        # high, med, low
        total = e.high + e.med + e.low
        #if ((e[3] * 100) / total) >= 47:
        if (e.low * 100) / total >= 50:
            stat = "low"
        else:
            total_no_low = total - e.low
            if ((e.high * 100) / total_no_low) >= 25:
                stat = "high"
            else:
                stat = "med"
    return ",".join(["low", stat] + (["low"]*5))

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
    output = "Date,High,Med,Low\n"

    current_time = datetime.datetime.utcnow()
    one_hour_ago = current_time - datetime.timedelta(hours=1)

    #for e in curdata:
    for e in Data.query.filter(Data.timestamp >= one_hour_ago).all():
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

    if not d:
        return "ERROR"

    data = d.split(",")

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
