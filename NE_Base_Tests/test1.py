import os
import pygatt
from binascii import hexlify
import time

# Definitions 
device_address = 'cc:0f:bd:d1:01:92'
char_id = "f3641401-00b0-4240-ba50-05ca45bf8abc"
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

# Initializations
adapter = pygatt.backends.GATTToolBackend(search_window_size=168)
device = None
recv_value = []

# Buffer Details
stream_buf = []
is_streaming = False

def notify_callback(handle, value):
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
	
	b_arr = bytearray(arr)
	result = DEFAULT_RESULT
	
	try:
		device.char_write(char_id, b_arr)
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


if __name__ == "__main__":
	print("Starting test ...")
	try:
		adapter.start()
		device = adapter.connect(device_address, address_type=pygatt.BLEAddressType.random)
		rmtu = device.exchange_mtu(230)
		print("MTU: {}".format(rmtu))
		device.subscribe(char_id, callback=notify_callback)
	except Exception as e:
		print("Error ")
		exit()
		
	#device.exchange_mtu(140)
	print("Initialization success")

	test_flash_page_read(1, 1)
	
	adapter.stop()
	