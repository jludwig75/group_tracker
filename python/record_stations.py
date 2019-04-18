import sys
import time
import json
import os
import getlocs
import datetime


MAX_POINT_INTERVAL_SEC = 10


GPX_FILE_START = '''<?xml version="1.0" encoding="UTF-8"?>
<gpx version="1.1" creator="Garmin Connect"
  xsi:schemaLocation="http://www.topografix.com/GPX/1/1 http://www.topografix.com/GPX/1/1/gpx.xsd http://www.garmin.com/xmlschemas/GpxExtensions/v3 http://www.garmin.com/xmlschemas/GpxExtensionsv3.xsd http://www.garmin.com/xmlschemas/TrackPointExtension/v1 http://www.garmin.com/xmlschemas/TrackPointExtensionv1.xsd"
  xmlns="http://www.topografix.com/GPX/1/1"
  xmlns:gpxtpx="http://www.garmin.com/xmlschemas/TrackPointExtension/v1"
  xmlns:gpxx="http://www.garmin.com/xmlschemas/GpxExtensions/v3" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance">
'''


GPX_FILE_METADATA = '''  <metadata>
    <link href="connect.garmin.com">
      <text>Garmin Connect</text>
    </link>
    <time>%s</time>
  </metadata>
'''

GPX_TRACK_START = '''  <trk>
    <name>Untitled</name>
    <trkseg>
'''

GPX_TRACK_POINT = '''      <trkpt lon="%.6f" lat="%.6f">
        <time>%s</time>
      </trkpt>
'''

GPX_TRACK_END = '''    </trkseg>
  </trk>
'''

GPX_FILE_END = '''</gpx>
'''

def gpx_timestamp(time):
    utc_time = datetime.datetime(2000, 1, 1).utcfromtimestamp(time)
    return utc_time.strftime('%Y-%m-%dT%H:%M:%S.%f')[:-3] + 'Z'


class GpxFileRecorder:
    def __init__(self, station_id):
        self._station_id = station_id
        self._file_name = 'station_%u_track.gpx' % self._station_id
        self._last_latitude = None
        self._last_longitude = None
        self._last_time = 0

    def record_location(self, location):
        if location['timestamp'] - self._last_time > MAX_POINT_INTERVAL_SEC or location['latitude'] != self._last_latitude or location['longitude'] != self._last_longitude:
            self.write_location(location)
            self._last_time = location['timestamp']
            self._last_latitude = location['latitude']
            self._last_longitude = location['longitude']
    
    def write_gpx_file_start(self, f, timestamp):
        f.write(GPX_FILE_START)
        f.write(GPX_FILE_METADATA % gpx_timestamp(timestamp))
        f.write(GPX_TRACK_START)

    def write_location(self, location):
        time_string = gpx_timestamp(location['timestamp'])
        trkpt = GPX_TRACK_POINT % (location['longitude'], location['latitude'], time_string)
        write_file_start = not os.path.isfile(self._file_name)
        with open(self._file_name, 'a') as f:
            if write_file_start:
                self.write_gpx_file_start(f, location['timestamp'])
            f.write(trkpt)


class LocationTracker:
    def __init__(self):
        self._gpx_recorders = {}
    
    def record_location(self, location):
        if not location['station_id'] in self._gpx_recorders:
            self._gpx_recorders[location['station_id']] = GpxFileRecorder(location['station_id'])
        self._gpx_recorders[location['station_id']].record_location(location)


tracker = LocationTracker()

while True:
    locs = getlocs.get_locations(sys.argv[1])
    for loc in locs:
        tracker.record_location(loc)
    time.sleep(1)