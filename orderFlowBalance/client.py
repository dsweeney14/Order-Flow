import websocket
import time
import json
import hmac 
import hashlib
import base64
import os 
import win32pipe, win32file

# API URL
WS_URL = 'wss://ws-direct.exchange.coinbase.com'

# Pipe settings
PIPE_NAME = 'coinbase_pipe'
PIPE_BUFFER_SIZE = 512

# Function to create a named pipe
def create_named_pipe(pipe_name):
    return win32pipe.CreateNamedPipe(
        r'\\.\pipe\\' + pipe_name,
        win32pipe.PIPE_ACCESS_DUPLEX,
        win32pipe.PIPE_TYPE_MESSAGE | win32pipe.PIPE_READMODE_MESSAGE | win32pipe.PIPE_WAIT,
        1, PIPE_BUFFER_SIZE, PIPE_BUFFER_SIZE,
        0,
        None
    )

# Create the data streaming pipe
pipe_handle = create_named_pipe(PIPE_NAME)

# Function to send a message through the pipe
def send_message_through_pipe(pipe_handle, message):
    try:
        win32file.WriteFile(pipe_handle, message.encode())
    except Exception as e:
        print("Failed to send message through pipe:", e)


# Get API keys and assign to variables
with open("config.txt", "r", encoding="utf-8") as file:
        for line in file:
            if "=" in line:
                key, value = line.split("=", 1)
                key = key.strip()
                value = value.strip()

                if key == "API_KEY":
                    API_KEY=value

                elif key == "API_SECRET":
                    API_SECRET=value
                
                elif key == "PASSPHRASE":
                    API_PASSPHRASE=value

# Function to create a timestamp
def create_timestamp():
    return str(time.time())

# Function to create HMAC signature
def create_signature(timestamp, secret, method, request_path, body):
    message = str(timestamp) + method + request_path + body
    hmac_key = base64.b64decode(secret)
    signature = hmac.new(hmac_key, message.encode('utf-8'), hashlib.sha256)
    return base64.b64encode(signature.digest()).decode('utf-8')

# Function to authenticate and subscribe
def on_open(ws):
    timestamp = create_timestamp()
    signature = create_signature(timestamp, API_SECRET, 'GET', '/users/self/verify', '')

    # Create the subscription message with the required auth headers
    subscribe_message = {
        "type": "subscribe",
        "product_ids": [
            "BTC-USD"
        ],
        "channels": [
            "level3"
        ],
        "signature": signature,
        "key": API_KEY,
        "passphrase": API_PASSPHRASE,
        "timestamp": timestamp
    }

    ws.send(json.dumps(subscribe_message))

# Define WebSocket callbacks
def on_message(ws, message): 
    send_message_through_pipe(pipe_handle, message)
    

def on_error(ws, error):
    print("Error:")
    print(error)

def on_close(ws, close_status_code, close_msg):
    print("### closed ###")

# Connect to WebSocket
def connect_to_websocket():
    websocket.enableTrace(False)
    headers = {
        'User-Agent': 'My-User-Agent-String'
    }
    ws = websocket.WebSocketApp(WS_URL,
                                on_open=on_open,
                                on_message=on_message,
                                on_error=on_error,
                                on_close=on_close,
                                header=headers)

    ws.run_forever()

if __name__ == "__main__":
    print("Connecting to Coinbase Pro WebSocket Feed...")
    win32pipe.ConnectNamedPipe(pipe_handle, None)
    print("Client connected to pipe.")
    connect_to_websocket()
    
