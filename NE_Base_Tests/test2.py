import os
import Adafruit_BluefruitLE
from binascii import hexlify
import time
import uuid

# Definitions 
device_address = 'cc:0f:bd:d1:01:92'
char_id = "f3641401-00b0-4240-ba50-05ca45bf8abc"
SERVICE_UUID = uuid.UUID('f3641400-00b0-4240-ba50-05ca45bf8abc')
CHAR_UUID    = uuid.UUID('f3641401-00b0-4240-ba50-05ca45bf8abc')
device_name = "NE_B"
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

# Get the BLE provider for the current platform.
ble = Adafruit_BluefruitLE.get_provider()
device = None
mchar  = None
recv_value = []

# Buffer Details
stream_buf = []
is_streaming = False

def notify_callback(value):
	global result
	global recv_value
	global is_streaming
	print(len(value))
	print("Received data %s " % hexlify(value))
	
	'''if is_streaming:
		global stream_buf
		stream_buf.append(value)
		return
	recv_value = value
	result = value[0]'''
	
def dev_write(arr = []):
	global device
	global result
	global mchar
	
	b_arr = bytearray(arr)
	result = DEFAULT_RESULT
	
	try:
		mchar.char_write(char_id, b_arr)
	except Exception as e:
		print(e)
		return
	
	time.sleep(0.5)	# enough time to receive notification
	
def test_read_desc(sensor):
	global result
	global recv_value
	
	print("Testing Read-Description, Sensor: {}".format(sensor))
	pkt = list()
	pkt.append(read_sensor_data_desc_command)
	pkt.append(sensor)
	
	dev_write(pkt)
	if(result != 0):
		print("Error")
		return
	
	print("blk_start: {}, blk_end: {}, cur_page: {}, cur_block: {}, filled: {}".format(recv_value[1],
			recv_value[2], recv_value[3], recv_value[4], recv_value[5]))
	return

def test_erase_desc(sensor):
	global result
	global recv_value
	
	print("Testing Erase-Description, Sensor: {}".format(sensor))
	pkt = list()
	pkt.append(erase_sensor_data_desc_command)
	pkt.append(sensor)
	
	dev_write(pkt)
	if(result != 0):
		print("Error")
		return
	
	print("Erased")
	return

def test_start_saving():
	global result
	
	print("Testing Start_Saving")
	pkt = list()
	pkt.append(start_saving_command)
	
	dev_write(pkt)
	if(result != 0):
		print("Error")
		return
	
	print("Saving Started")
	return

def test_stop_saving():
	global result
	
	print("Testing Stop_Saving")
	pkt = list()
	pkt.append(stop_saving_command)
	
	dev_write(pkt)
	if(result != 0):
		print("Error")
		return
	
	print("Saving Stopped")
	return

def test_flash_page_read(page, block):
	global result
	global recv_value
	global stream_buf
	global is_streaming
	
	print("Testing Read Flash Page, page: {}, block: {}".format(page, block))
	pkt = list()
	pkt.append(read_flash_page_command)
	pkt.append(page)
	pkt.append(block)
	
	stream_buf = []
	is_streaming = True
	dev_write(pkt)
	
	'''start_time = int(time.time())
	while(len(stream_buf) < 3):
		if( int(time.time()) > start_time + 3):
			is_streaming = False
			print("Timeout")
			return
	
	is_streaming = False 
	for arr in stream_buf:
		print(hexlify(arr))
	stream_buf = []'''
	print("Complete")
	return


def main():
	global device
	global ble
	global mchar
	global SERVICE_UUID
	ble.clear_cached_data()
    # Get the first available BLE network adapter and make sure it's powered on.
	adapter = ble.get_default_adapter()
	adapter.power_on()
	print('Using adapter: {0}'.format(adapter.name))

	device = ble.find_devices()
	print(len(device))
	for d in device:
		print(d.id)
	if device is None:
		raise RuntimeError('Failed to find device!')
		
	print('Connecting to device...')
	device.connect()
	
	try:
		print('Discovering services...')
		device.discover([SERVICE_UUID], [CHAR_UUID])
		mservice = device.find_service(SERVICE_UUID)
		mchar	 = mservice.find_characteristic(CHAR_UUID)
		
		print('Subscribing to characteristic changes...')
		mchar.start_notify(notify_callback)
		
		test_flash_page_read(1, 1)
		time.sleep(10)
		
	except Exception as e:
		print("Error Occured")
		print(e)
		
	finally:
		# Make sure device is disconnected on exit.
		device.disconnect()


# Initialize the BLE system.  MUST be called before other BLE calls!
ble.initialize()

ble.run_mainloop_with(main)

print("Done")




