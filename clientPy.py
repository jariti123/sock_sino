import socket
import sys
import chardet

def send_command(command):
    s = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
    s.connect(("127.0.0.1", 8080))
    s.sendall(command.encode())
    response = s.recv(1024)
    try:
        print("Server response:",response.replace('\x00', '').decode('utf-8'))
    except UnicodeDecodeError:
        print("Server response:", response.decode('latin1').strip())

if len(sys.argv) < 3 and sys.argv[1] not in ["--save","--searchall"]:
    print("usage:./client --command [options]\n")
    print("example:./client --search --name test1\n\n")
    sys.exit(1)

if sys.argv[1] == "--add" and sys.argv[2] == "--name" and len(sys.argv) == 12:
    command = "ADD {} {} {} {} {}".format(sys.argv[3],sys.argv[5],sys.argv[7],sys.argv[9], sys.argv[11])
elif sys.argv[1] == "--del" and sys.argv[2] == "--name" and len(sys.argv) == 4:
    command = "DEL {}".format(sys.argv[3])
elif sys.argv[1] == "--search" and sys.argv[2] == "--name" and len(sys.argv) == 4:
    command = "SEARCH {}".format(sys.argv[3])
elif sys.argv[1] == "--save" and len(sys.argv) == 2:
    command = "SAVE"
else:
    print("Invalid Command")
    sys.exit(1)

send_command(command)