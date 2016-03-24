import datetime
from flask import Flask, request, render_template

app = Flask(__name__)

curdata = []

@app.route('/')
def hello():
    return render_template('data.html')

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
