from flask.ext.sqlalchemy import SQLAlchemy

from MicSense.db import db, initDatabase

class Data(db.Model):
    #stores the mac address, no colons
    id_address = db.Column(db.String(12))

    id = db.Column(db.Integer(), primary_key=True)
    timestamp = db.Column(db.DateTime())
    high = db.Column(db.Integer())
    med = db.Column(db.Integer())
    low = db.Column(db.Integer())

    #name = db.Column(db.String(80), unique=True)
    #description = db.Column(db.String(255))

class Hardware(db.Model):
    floor_num = Column(Integer())
    loaction = Column(String(100))
    macAddress = Column(String(12), ForeignKey('data.id_address'))
    isolated_data = relationship('Data')
    
initDatabase()
