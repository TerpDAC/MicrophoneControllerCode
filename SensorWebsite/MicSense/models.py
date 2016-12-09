from flask.ext.sqlalchemy import SQLAlchemy

from MicSense.db import db, initDatabase

class Data(db.Model):

    id = db.Column(db.Integer(), primary_key=True)
    timestamp = db.Column(db.DateTime())
    high = db.Column(db.Integer())
    med = db.Column(db.Integer())
    low = db.Column(db.Integer())
    #Foreign key referencing Sensor
    mac_address = db.Column(db.String(12), db.ForeignKey('sensor.mac_address'))

    #name = db.Column(db.String(80), unique=True)
    #description = db.Column(db.String(255))

class Sensor(db.Model):

    id = db.Column(db.Integer(), primary_key=True)    
    floor_num = db.Column(db.Integer())
    location = db.Column(db.String(100))
    mac_address = db.Column(db.String(12), unique=True)
    #initializes a one to many relationship with data
    data = db.relationship('Data', backref='sensor', lazy='dynamic')
    
initDatabase()
