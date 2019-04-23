import httplib
import json

def get_locations(server):
    result = []
    try:
        conn = httplib.HTTPConnection(server)
        conn.request("GET", "/locs")
        response = conn.getresponse()
        if response.status == 200 and response.reason == 'OK':
            result = json.loads(response.read())
        conn.close()
    except:
        pass
    return result
