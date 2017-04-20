import traceback
# Run a test server.

try:
    print("Hello")
    from MicSense import app
    app.run(host='localhost', port=8080, debug=True)
    input("Press ENTER: ")
except:
    traceback.print_exc()
    input("Press ENTER: ")
