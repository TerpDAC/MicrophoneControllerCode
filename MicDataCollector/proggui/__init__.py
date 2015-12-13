import os
import sys

from proggui.ProgGUI import ProgressBar
from proggui.pguiproc import ProgressProcessSPPipe

# Attempt to hijack execution if a special environment variable is
# defined!

env_var = os.environ.get("PROGGUI_SUBPROCESS_RECEIVER")

if env_var:
    p = ProgressProcessSPPipe()
    sys.exit(p.run())
