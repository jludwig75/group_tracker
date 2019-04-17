import sys
import httplib
import json
import time

def get_locations(server):
    result = []
    conn = httplib.HTTPConnection(server)
    conn.request("GET", "/locs")
    response = conn.getresponse()
    if response.status == 200 and response.reason == 'OK':
        result = json.loads(response.read())
    conn.close()
    return result

while True:
    locs = get_locations(sys.argv[1])
    print locs
    time.sleep(1)