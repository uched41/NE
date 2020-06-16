import os
from bluepy import btle
from binascii import hexlify
import time
import uuid

# Definitions 
device_address = "CC:0F:BD:D1:01:92"
char_id = "f3641401-00b0-4240-ba50-05ca45bf8abc"
SERVICE_UUID = uuid.UUID('f3641400-00b0-4240-ba50-05ca45bf8abc')
CHAR_UUID    = uuid.UUID('f3641401-00b0-4240-ba50-05ca45bf8abc')
DEFAULT_RESULT = 255
result = DEFAULT_RESULT

# commands
read_sensor_data_command 		= 0
erase_sensor_data_command		= 1
read_sensor_data_desc_command	= 2
erase_sensor_data_desc_command  = 3
start_saving_command			= 4
stop_saving_command				= 5
real_time_command				= 6
read_flash_page_command			= 7

	
class MyDelegate(btle.DefaultDelegate):
    def __init__(self):
        btle.DefaultDelegate.__init__(self)
        # ... initialise here

    def handleNotification(self, cHandle, data):
        print(len(data))
        print("Received data %s " % hexlify(data))


# Initialisation  -------

p = btle.Peripheral( device_address, "random" )
p.setDelegate( MyDelegate() )
p.setMTU(230)

setup_data = bytearray([read_flash_page_command, 1, 1])

# Setup to turn notifications on, e.g.
svc = p.getServiceByUUID( SERVICE_UUID )
ch = svc.getCharacteristics( CHAR_UUID )[0]

# Descriptor update to CCCD and enable notification 
p.writeCharacteristic(ch.getHandle()+1, b"\x01\x00", True)

ch.write( setup_data )

# Main loop --------

while True:
    if p.waitForNotifications(1.0):
        # handleNotification() was called
        continue

    print ("Waiting...")
    # Perhaps do something else here

	
	
	