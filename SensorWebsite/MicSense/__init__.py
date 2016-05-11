import os

from flask import Flask

app = Flask(__name__)

# Create app
app.config.from_pyfile('config.py', silent=True)
app.config.from_pyfile(os.path.join(app.instance_path, 'config.py'))

import MicSense.models
import MicSense.views