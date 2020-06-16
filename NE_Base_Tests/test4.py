import os
import time
import uuid
import logging
import asyncio
import platform
from binascii import hexlify
from bleak import BleakClient
from bleak import _logger as logger

# Definitions 
DEVICE_ADDRESS 		= "CC:0F:BD:D1:01:92"
CHARACTERISTIC_UUID = ("f3641401-00b0-4240-ba50-05ca45bf8abc")
DEFAULT_RESULT 		= 255
result 				= DEFAULT_RESULT
recv_value 			= None
client 				= None
loop 				= None

# Buffer Details
stream_buf 		= []
target_length 	= 0

# commands
read_sensor_data_command 		= 0
erase_sensor_data_command		= 1
read_sensor_data_desc_command	= 2
erase_sensor_data_desc_command  = 3
start_saving_command			= 4
stop_saving_command				= 5
real_time_command				= 6
read_flash_page_command			= 7
fill_sensor_data				= 8

def notify_callback(handle, value):
    global result
    global target_length
    global recv_value
    #print(len(value))
    print("Received data %s " % hexlify(value))
	
    if target_length > 0:
        global stream_buf
        stream_buf.append(value)
        print(target_length)
        target_length = target_length - 1
        return

    recv_value = value
    result = value[0]
	
async def dev_write(arr = [], wait=0.5):
    global result
    global client
	
    b_arr = bytearray(arr)
    result = DEFAULT_RESULT
	
    try:
        await client.write_gatt_char(CHARACTERISTIC_UUID, b_arr)

    except Exception as e:
        print(e)
        return
	
    await asyncio.sleep(wait, loop=loop)	# enough time to receive notification
	
async def test_read_desc(sensor):
	global result
	global recv_value
	
	print("- Testing Read-Description, Sensor: {}".format(sensor))
	pkt = list()
	pkt.append(read_sensor_data_desc_command)
	pkt.append(sensor)
	
	await dev_write(pkt)
	if(result != 0):
		print("Error")
		return
	
	print("blk_start: {}, blk_end: {}, cur_page: {}, cur_block: {}, filled: {}".format(recv_value[1],
			recv_value[2], recv_value[3], recv_value[4], recv_value[5]))
	return recv_value

async def test_erase_desc(sensor):
	global result
	global recv_value
	
	print("- Testing Erase-Description, Sensor: {}".format(sensor))
	pkt = list()
	pkt.append(erase_sensor_data_desc_command)
	pkt.append(sensor)
	
	await dev_write(pkt)
	if(result != 0):
		print("Error")
		return
	
	print("Erased")
	return

async def test_start_saving():
	global result
	
	print("- Testing Start_Saving")
	pkt = list()
	pkt.append(start_saving_command)
	
	await dev_write(pkt)
	if(result != 0):
		print("Error")
		return
	
	print("Saving Started")
	return

async def test_stop_saving():
	global result
	
	print("- Testing Stop_Saving")
	pkt = list()
	pkt.append(stop_saving_command)
	
	await dev_write(pkt)
	if(result != 0):
		print("Error")
		return
	
	print("Saving Stopped")
	return

async def test_read_flash_page(page, block):
	global result
	global recv_value
	global stream_buf
	global target_length
	
	print("- Testing Read Flash Page, page: {}, block: {}".format(page, block))
	pkt = list()
	pkt.append(read_flash_page_command)
	pkt.append(page)
	pkt.append(block)
	
	stream_buf = []
	target_length = 3
	await dev_write(pkt)
	
	start_time = int(time.time())
	while(target_length > 0):
		if( int(time.time()) > start_time + 3):
			target_length = 0
			stream_buf = []
			print("Error: Timeout")
			return
	
	for arr in stream_buf:
		print(hexlify(arr))
	stream_buf = []
	
	print("Complete")
	return

async def test_fill_data(sensor):
	global result
	global recv_value
	
	print("- Testing Fill Sensor Data, Sensor: {}".format(sensor))
	pkt = list()
	pkt.append(fill_sensor_data)
	pkt.append(sensor)
	
	await dev_write(pkt, 3)
	if(result != 0):
		print("Error")
		return
	
	print("Erased")
	return

async def test_read_data(sensor):
	global result
	global recv_value
	global stream_buf
	global target_length
	
	print("- Testing Read Sensor Data")
	sdesc = await test_read_desc(sensor)
	npages = (sdesc[1] - sdesc[4])*256 + sdesc[3]
	print("Num Pages: {}".format(npages))
	if npages<1:
		return
	
	pkt = list()
	pkt.append(read_sensor_data_command)
	pkt.append(sensor)
	
	stream_buf = []
	target_length = npages*2+1
	await dev_write(pkt)
	
	start_time = int(time.time())
	while(target_length > 0):
		if( int(time.time()) > start_time + 15):
			target_length = 0
			stream_buf = []
			print("Error: Timeout")
			return
	
	for arr in stream_buf:
		print(hexlify(arr))
	stream_buf = []
	
	print("Complete")
	return

async def run(address, loop, debug=False):
    global client
	
    async with BleakClient(address, loop=loop) as client:
        x = await client.is_connected()
        print("Connected: {0}".format(x))

        await client.start_notify(CHARACTERISTIC_UUID, notify_callback)
		
        await test_fill_data(0)
        #await asyncio.sleep(1.0, loop=loop)
        await test_read_data(0)
        #await test_read_flash_page(1,2)
        #await test_read_desc(0)
		
        await asyncio.sleep(1.0, loop=loop)
        await client.stop_notify(CHARACTERISTIC_UUID)


if __name__ == "__main__":
    print("Starting Test ...")
    address = ( DEVICE_ADDRESS )
    loop = asyncio.get_event_loop()
    loop.run_until_complete(run(address, loop, False))
	
	
	
