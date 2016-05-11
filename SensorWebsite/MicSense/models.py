from flask.ext.sqlalchemy import SQLAlchemy

from MicSense.db import db, initDatabase

class Data(db.Model):
    id = db.Column(db.Integer(), primary_key=True)
    timestamp = db.Column(db.DateTime())
    high = db.Column(db.Integer())
    med = db.Column(db.Integer())
    low = db.Column(db.Integer())

    #name = db.Column(db.String(80), unique=True)
    #description = db.Column(db.String(255))

initDatabase()