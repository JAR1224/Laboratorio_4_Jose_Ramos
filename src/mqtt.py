import paho.mqtt.client as mqtt
import time
import json

mytopic = "v1/devices/me/telemetry"

is_connected = False

# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
    global is_connected
    print("Connected with result code "+str(rc))
    if rc == 0:
        is_connected = True

# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    print(msg.topic+" "+str(msg.payload))

def on_publish(client,userdata,result):    
    print("result: ",result)

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.on_publish = on_publish

client.username_pw_set("gpf7toozp33wqtqar7re")

client.connect("iot.eie.ucr.ac.cr")

state = "on"
client.loop_start()

while( not is_connected ):
    print("Waiting for MQTT connection ...")
while(1):

	#client.subscribe(mytopic)
	y = json.dumps(state)
	ret = client.publish(mytopic,y)
	print("ret: ",ret)
	state = "off" if state=="on" else state

	time.sleep(4)
client.loop_stop()
client.disconnect()