from flask.ext.sqlalchemy import SQLAlchemy

from MicSense.db import db, initDatabase

class Data(db.Model):

    id = db.Column(db.Integer(), primary_key=True)
    #stores the mac address, no colons
    mac_address = db.Column(db.String(12))
    timestamp = db.Column(db.DateTime())
    high = db.Column(db.Integer())
    med = db.Column(db.Integer())
    low = db.Column(db.Integer())

    #name = db.Column(db.String(80), unique=True)
    #description = db.Column(db.String(255))

class Sensor(db.Model):

    id = db.Column(db.Integer(), primary_key=True)    
    floor_num = db.Column(db.Integer())
    location = db.Column(db.String(100))
    mac_address = db.Column(db.String(12), db.ForeignKey('data.mac_address'))
    
initDatabase()
