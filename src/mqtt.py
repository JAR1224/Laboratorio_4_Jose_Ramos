import serial
import time
import paho.mqtt.client as mqtt
import json

dictionary ={ 
  "x": 0, 
  "y": 0, 
  "z": 0,
  "bateria" : 0
}

mytopic = "v1/devices/me/telemetry"

is_connected = False
upload = False
tmp = 0
counter=0

def on_connect(client, userdata, flags, rc):
    global is_connected
    print("Connected with result code "+str(rc))
    if rc == 0:
        is_connected = True

def on_publish(client,userdata,result):
    global upload    
    upload=True

client = mqtt.Client()
client.on_connect = on_connect
client.on_publish = on_publish

client.username_pw_set("19me1xr4k0tmae3tgbz4")

client.connect("iot.eie.ucr.ac.cr")

client.loop_start()

while( not is_connected ):
    print("Waiting for MQTT connection ...")

serialPort = serial.Serial(
    port="COM6", baudrate=115200, bytesize=8, timeout=2, stopbits=serial.STOPBITS_ONE
)
print("Serial Begin")

serialString = ""
while 1:
    if serialPort.in_waiting > 0:

        serialString = serialPort.readline()
  
        try:
            #print(serialString.decode("Ascii"))
            #f.write(serialString.decode("Ascii"))
            data = serialString.decode("Ascii").split(',')
            dictionary['x'] = data[0]
            dictionary['y'] = data[1]
            dictionary['z'] = data[2]
            dictionary["bateria"] = float(float(data[3])/100)
            print(dictionary)
            json_data = json.dumps(dictionary)
            client.publish(mytopic,json_data)
        except:
            pass