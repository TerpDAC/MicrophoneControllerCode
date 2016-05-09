import datetime
import random
from flask import Flask, request, render_template

app = Flask(__name__)

curdata = []


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

    if len(curdata) == 0:
        return ",".join(["low"] * 7)
    else:
        e = curdata[-1]
        total = e[1] + e[2] + e[3]
        #if ((e[3] * 100) / total) >= 47:
        if (total - e[3]) / total >= 50:
            stat = "low"
        else:
            total_no_low = total - e[3]
            if ((e[1] * 100) / total_no_low) >= 25:
                stat = "high"
            else:
                stat = "med"
    return ",".join(["low", stat] + (["low"]*5))

@app.route('/floorstatus-dbg')
def floorstatusdbg():
    if len(curdata) == 0:
        return "No data!"
    else:
        e = curdata[-1]
        total = e[1] + e[2] + e[3]

        total_no_low = total - e[3]

        low_percent = ((e[3] * 100) / total)
        med_percent = ((e[2] * 100) / total)
        high_percent = ((e[1] * 100) / total)
        high_percent_in_nonlow = ((e[1] * 100) / total_no_low)
        med_percent_in_nonlow = ((e[2] * 100) / total_no_low)

        return "Low percent: %.2f%% | Med percent: %.2f%% | High percent: %.2f%% | Med percent (within med/high): %.2f%% | High percent (within med/high): %.2f%%" % (low_percent, med_percent, high_percent, med_percent_in_nonlow, high_percent_in_nonlow)

@app.route('/rawdata')
def rawdata():
    output = "Date,High,Med,Low\n"

    for e in curdata:
        print(e)
        print(datetime.datetime.fromtimestamp(e[0]).strftime("%Y/%m/%d %H:%M:%S"))
        buf = []
        buf.append(datetime.datetime.fromtimestamp(e[0]).strftime("%Y/%m/%d %H:%M:%S"))
        buf.append(str(e[1]))
        buf.append(str(e[2]))
        buf.append(str(e[3]))
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

    new_data = [timestamp, high, med, low]
    curdata.append(list(new_data))

    return "OK"

if __name__ == '__main__':
    app.run()
