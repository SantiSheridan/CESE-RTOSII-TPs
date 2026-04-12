import serial
import threading
import time

PORT = 'COM6'
BAUDRATE = 115200
tx_msg = [
    b'{type:1,id:2,priority:2}',
    b'{type:1,id:17,priority:17}',
    b'{type:1,id:100,priority:100}',
    b'{type:1,id:101,priority:100}',
    b'{type:1,id:102,priority:100}',
    b'{type:1,id:103,priority:100}',
    b'{type:1,id:104,priority:100}',
]

ser = serial.Serial(PORT, BAUDRATE, timeout=1)

def read_thread():
    buffer = ""
    while True:
        data = ser.read(ser.in_waiting or 1)
        if data:
            buffer += data.decode('utf-8', errors='ignore')
            while '\n' in buffer or '}' in buffer:
                if '}' in buffer:
                    end = buffer.index('}') + 1
                    msg = buffer[:end].strip()
                    print(f"[RX] {msg}")
                    buffer = buffer[end:]
                else:
                    break

t = threading.Thread(target=read_thread, daemon=True)
t.start()
time.sleep(1)  # esperar que el thread de lectura arranque

tx_msg = b''.join(tx_msg)
print(f"[TX] {tx_msg.decode()}")
ser.write(tx_msg)
time.sleep(5)

ser.close()