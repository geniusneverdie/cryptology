import pymysql
from jinja2 import Environment, FileSystemLoader
from flask import Flask, render_template, request, redirect, url_for,session
import re
import os
import random

#rsa算法
# 求最大公约数
def gcd(a, b):
    if a < b:
        return gcd(b, a)
    elif a % b == 0:
        return b
    else:
        return gcd(b, a % b)


# 快速幂+取模
def power(a, b, c):
    ans = 1
    while b != 0:
        if b & 1:
            ans = (ans * a) % c
        b >>= 1
        a = (a * a) % c
    return ans


# 快速幂
def quick_power(a: int, b: int) -> int:
    ans = 1
    while b != 0:
        if b & 1:
            ans = ans * a
        b >>= 1
        a = a * a
    return ans


# 大素数检测
def Miller_Rabin(n):
    a = random.randint(2, n - 2)  # 随机第选取一个a∈[2,n-2]
    # print("随机选取的a=%lld\n"%a)
    s = 0  # s为d中的因子2的幂次数。
    d = n - 1
    while (d & 1) == 0:  # 将d中因子2全部提取出来。
        s += 1
        d >>= 1

    x = power(a, d, n)
    for i in range(s):  # 进行s次二次探测
        newX = power(x, 2, n)
        if newX == 1 and x != 1 and x != n - 1:
            return False  # 用二次定理的逆否命题，此时n确定为合数。
        x = newX

    if x != 1:  # 用费马小定理的逆否命题判断，此时x=a^(n-1) (mod n)，那么n确定为合数。
        return False

    return True  # 用费马小定理的逆命题判断。能经受住考验至此的数，大概率为素数。


# 卢卡斯-莱墨素性检验
def Lucas_Lehmer(num: int) -> bool:  # 快速检验pow(2,m)-1是不是素数
    if num == 2:
        return True
    if num % 2 == 0:
        return False
    s = 4
    Mersenne = pow(2, num) - 1  # pow(2, num)-1是梅森数
    for x in range(1, (num - 2) + 1):  # num-2是循环次数，+1表示右区间开
        s = ((s * s) - 2) % Mersenne
    if s == 0:
        return True
    else:
        return False

# 扩展的欧几里得算法，ab=1 (mod m), 得到a在模m下的乘法逆元b
def Extended_Eulid(a: int, m: int) -> int:
    def extended_eulid(a: int, m: int):
        if a == 0:  # 边界条件
            return 1, 0, m
        else:
            x, y, gcd = extended_eulid(m % a, a)  # 递归
            x, y = y, (x - (m // a) * y)  # 递推关系，左端为上层
            return x, y, gcd  # 返回第一层的计算结果。
        # 最终返回的y值即为b在模a下的乘法逆元
        # 若y为复数，则y+a为相应的正数逆元

    n = extended_eulid(a, m)
    if n[1] < 0:
        return n[1] + m
    else:
        return n[1]

# 按照需要的bit来生成大素数
def Generate_prime(key_size: int) -> int:
    while True:
        num = random.randrange(quick_power(2, key_size - 1), quick_power(2, key_size))
        if Miller_Rabin(num):
            return num


# 生成公钥和私钥
def KeyGen(p: int, q: int):
    e = random.randint(1, (p - 1) * (q - 1))
    while gcd(e, (p - 1) * (q - 1)) != 1:
        e = random.randint(1, (p - 1) * (q - 1))
    n = p * q
    d = Extended_Eulid(e, (p - 1) * (q - 1))
    return n, e, d


def Encrypt(message: int, e: int, n: int) -> int:
    ciphertext = power(message, e, n)
    return ciphertext


def Decrypt(ciphertext: int, d: int, n: int) -> int:
    plaintext = power(ciphertext, d, n)
    return plaintext


# 连接数据库
connection = pymysql.connect(host='localhost', port=3306, user='root', password='4239692', db='rsa', charset='utf8')
# 创建游标对象
cursor = connection.cursor()

app = Flask(__name__)
app.secret_key = "mysecretkey"

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/vi', methods=['GET'])
def vi():
    return render_template('vi.html')

from Crypto.Cipher import AES
import base64
class FileAES:
    def __init__(self,key):
        self.key = key #将密钥转换为字符型数据
        self.mode = AES.MODE_ECB  #操作模式选择ECB

    def encrypt(self,text):
        """加密函数"""
        file_aes = AES.new(self.key,self.mode)  #创建AES加密对象
        text = text.encode('utf-8')  #明文必须编码成字节流数据，即数据类型为bytes
        while len(text) % 16 != 0:  # 对字节型数据进行长度判断
            text += b'\x00'  # 如果字节型数据长度不是16倍整数就进行补充
        en_text = file_aes.encrypt(text)  #明文进行加密，返回加密后的字节流数据
        return str(base64.b64encode(en_text),encoding='utf-8')  #将加密后得到的字节流数据进行base64编码并再转换为unicode类型

    def decrypt(self,text):
        """解密函数"""
        file_aes = AES.new(self.key,self.mode)
        text = bytes(text,encoding='utf-8')  #将密文转换为bytes，此时的密文还是由basen64编码过的
        text = base64.b64decode(text)   #对密文再进行base64解码
        de_text = file_aes.decrypt(text)  #密文进行解密，返回明文的bytes
        return str(de_text,encoding='utf-8').strip()  #将解密后得到的bytes型数据转换为str型，并去除末尾的填充

@app.route('/login', methods=['GET','POST'])
def login():
    # 获取表单数据
    if request.method == 'POST':
        iduser = request.form['iduser']
        password = request.form['password']
        sql = "SELECT * FROM user WHERE iduser='%s' AND password='%s'" % (iduser, password)
        cursor.execute(sql)
        result = cursor.fetchall()
        # 验证用户
        if len(result) > 0:
            session['iduser'] = iduser
            return redirect(url_for('me', user=iduser))
        else:
            message = "用户名或密码错误！"
            return render_template('login.html', message=message)
    # 显示登录页面
    return render_template('login.html')
# 重定向到“me”页面
@app.route('/me/<user>', methods=['GET'])
def me(user):
    with connection.cursor() as cursor:
        # 查询所有与当前用户相关的数据
        sql = "SELECT * FROM `receive`"
        cursor.execute(sql)
        receives = cursor.fetchall()
    return render_template('me.html', receives=receives)

@app.route('/users', methods=['GET'])
def users():
    with connection.cursor() as cursor:
        cursor.execute("SELECT * FROM user")
        users = cursor.fetchall()
    return render_template('users.html', users=users)

@app.route('/user/add', methods=['GET', 'POST'])
def add_user():
    if request.method == 'POST':
        iduser = request.form.get('iduser')
        phonenumber = request.form.get('phonenumber')
        # 验证身份证号的正则表达式
        id_pattern = re.compile(r'^\d{4}(\d|X)$')
        # 验证手机号的正则表达式
        #phone_pattern = re.compile(r'^1[3456789]\d{9}$')
        name = request.form.get('name')
        idca = request.form.get('idca')
        ta = request.form.get('ta')
        password = request.form.get('password')
        key_size = 8
        p = Generate_prime(key_size)
        q = Generate_prime(key_size)
        n, e1, d = KeyGen(p, q)
        pka = d
        e = e1;
        key = os.urandom(16)
        aes_test = FileAES(key)
        p1 = str(pka)
        pka1 = aes_test.encrypt(p1)
        skca = key[0]
        t1 = str(ta)
        ta1 = aes_test.encrypt(t1)
        i1 = str(idca)
        id1 = aes_test.encrypt(i1)
        if len(iduser)==4 :
            #bool(id_pattern.match(iduser))and bool(phone_pattern.match(phonenumber))
            with connection.cursor() as cursor:
                sql = "INSERT INTO user (iduser,phonenumber,name,idca,ta,pka,n,password,p,q,e) VALUES (%s,'%s','%s','%s','%s','%s','%s','%s','%s','%s','%s')" % (
                iduser, phonenumber, name, idca, ta, pka, n, password, p, q, e)
                cursor.execute("INSERT INTO ca (idca,pka1,skca,ta1,id1) VALUES (%s,%s,%s,%s,%s)",
                               (idca, pka1, skca, ta1, id1))
                print(sql)
                cursor.execute(sql)
                connection.commit()
            return redirect(url_for('users'))
    else:
        return render_template('add_user.html')


@app.route('/coach/edit/<int:iduser>', methods=['GET', 'POST'])
def edit_user(iduser):
    with connection.cursor() as cursor:
        cursor.execute("SELECT * FROM user WHERE iduser=%s", (iduser,))
        user = cursor.fetchone()
    if request.method == 'POST':
        x=iduser
        iduser = request.form('iduser')
        phonenumber = request.form('phonenumber')
        name = request.form('name')
        idca = request.form('idca')
        ta = request.form('ta')
        pka = request.form('pka')
        n = request.form('n')
        password = request.form('password')
        p = request.form('p')
        q = request.form('q')
        e = request.form('e')
        with connection.cursor() as cursor:
            cursor.execute("UPDATE user SET iduser=%s, phonenumber=%s, name=%s, idca=%s, ta=%s, pka=%s, n=%s, password=%s, p=%s, q=%s,e=%s"
                           " WHERE iduser=%s",
                           (iduser,phonenumber,name,idca,ta,pka,n,password,p,q,e,x))
            connection.commit()
        return redirect(url_for('users'))
    return render_template('edit_user.html', user=user)

@app.route('/user/delete/<int:iduser>')
def delete_user(iduser):
    with connection.cursor() as cursor:
            cursor.execute("START TRANSACTION")
            cursor.execute("DELETE FROM user WHERE iduser = %s", (iduser,))
            cursor.execute("COMMIT")
    return redirect(url_for('users'))

@app.route('/ca', methods=['GET'])
def ca():
    with connection.cursor() as cursor:
        cursor.execute("SELECT * FROM ca")
        cas = cursor.fetchall()
    return render_template('ca.html', cas=cas)

@app.route('/ca/add', methods=['GET', 'POST'])
def add_ca():
    if request.method == 'POST':
        idca=request.form.get('idca')
        pka1 = request.form.get('pka1')
        skca = request.form.get('skca')
        ta1 = request.form.get('ta1')
        id1 = request.form.get('id1')
        with connection.cursor() as cursor:
            cursor.execute("INSERT INTO ca (idca,pka1,skca,ta1,id1) VALUES (%s,%s,%s,%s,%s)",
                           (idca,pka1,skca,ta1,id1))
            connection.commit()
        return redirect(url_for('ca'))
    else:
        return render_template('add_ca.html')

@app.route('/ca/edit/<int:idca>', methods=['GET', 'POST'])
def edit_ca(idca):
    with connection.cursor() as cursor:
        cursor.execute("SELECT * FROM ca WHERE idca=%s", idca)
        ca = cursor.fetchone()
    if request.method == 'POST':
        x=idca
        idca = request.form.get('idca')
        pka1 = request.form.get('pka1')
        skca = request.form.get('skca')
        ta1 = request.form.get('ta1')
        id1 = request.form.get('id1')
        with connection.cursor() as cursor:
            cursor.execute("UPDATE `ca` SET idca=%s,pka1=%s, skca=%s, ta1=%s, id1=%s  WHERE idca=%s",
                           (idca,pka1,skca,ta1,id1, x))
            connection.commit()
        return redirect(url_for('ca'))
    return render_template('edit_ca.html', ca=ca)

@app.route('/ca/delete/<int:idca>', methods=['POST'])
def delete_ca(idca):
    with connection.cursor() as cursor:
        cursor.execute("DELETE FROM ca WHERE idca=%s", idca)
        connection.commit()
    return redirect(url_for('ca'))

@app.route('/manage', methods=['GET'])
def manage():
    with connection.cursor() as cursor:
        cursor.execute("SELECT * FROM manage")
        manages = cursor.fetchall()
    return render_template('manage.html', manages=manages)

@app.route('/manage/add', methods=['GET', 'POST'])
def add_manage():
    if request.method == 'POST':
        idmanager=request.form.get('idmanager')
        password = request.form.get('password')
        iduser = request.form.get('iduser')
        with connection.cursor() as cursor:
            cursor.execute("INSERT INTO manage (idmanager,password,iduser) VALUES (%s,%s,%s)",
                           (idmanager,password,iduser))
            connection.commit()
        return redirect(url_for('manage'))
    else:
        return render_template('add_manage.html')

@app.route('/manage/edit/<int:idmanager>', methods=['GET', 'POST'])
def edit_manage(idmanager):
    with connection.cursor() as cursor:
        cursor.execute("SELECT * FROM manage WHERE idmanager=%s", idmanager)
        manage = cursor.fetchone()
    if request.method == 'POST':
        x=idmanager
        idmanager = request.form.get('idmanager')
        password = request.form.get('password')
        iduser = request.form.get('iduser')
        with connection.cursor() as cursor:
            cursor.execute("UPDATE `manage` SET idmanager=%s, password=%s, iduser=%s WHERE idmanager=%s",
                           (idmanager,password,iduser, x))
            connection.commit()
        return redirect(url_for('manage'))
    return render_template('edit_manage.html', manage=manage)

@app.route('/manage/delete/<int:idmanager>', methods=['POST'])
def delete_manage(idmanager):
    with connection.cursor() as cursor:
        cursor.execute("DELETE FROM manage WHERE idmanager=%s", idmanager)
        connection.commit()
    return redirect(url_for('manage'))

@app.route('/send', methods=['GET'])
def send():
    with connection.cursor() as cursor:
        cursor.execute("SELECT * FROM send")
        sends = cursor.fetchall()
    return render_template('send.html', sends=sends)

@app.route('/send/add', methods=['GET', 'POST'])
def add_send():
    if request.method == 'POST':
        idsend=request.form.get('idsend')
        idreceive=request.form.get('idsend')
        ida1 = request.form.get('ida1')
        ta1 = request.form.get('ta1')
        message1 = request.form.get('message1')
        pka1 = request.form.get('pka1')
        iduser = request.form.get('iduser')
        MD5 = request.form.get('MD5')
        with connection.cursor() as cursor:
            cursor.execute("INSERT INTO send (idsend,ida1,ta1,message1,pka1,iduser,MD5) VALUES (%s,%s,%s,%s,%s,%s,%s)",
                           (idsend,ida1,ta1,message1,pka1,iduser,MD5))
            cursor.execute("INSERT INTO receive (idreceive,ida1,ta1,message1,pka1,iduser,MD5) VALUES (%s,%s,%s,%s,%s,%s,%s)",
                           (idreceive, ida1, ta1, message1, pka1, iduser, MD5))
            connection.commit()
        return redirect(url_for('send'))
    else:
        return render_template('add_send.html')

@app.route('/send/edit/<int:idsend>', methods=['GET', 'POST'])
def edit_send(idsend):
    with connection.cursor() as cursor:
        cursor.execute("SELECT * FROM send WHERE idsend=%s", idsend)
        send = cursor.fetchone()
    if request.method == 'POST':
        x=idsend
        idsend = request.form.get('idsend')
        idreceive = request.form.get('idsend')
        ida1 = request.form.get('ida1')
        ta1 = request.form.get('ta1')
        message1 = request.form.get('message1')
        pka1 = request.form.get('pka1')
        iduser = request.form.get('iduser')
        MD5 = request.form.get('MD5')
        with connection.cursor() as cursor:
            cursor.execute("UPDATE `send` SET idsend=%s,ida1=%s,ta1=%s,message1=%s,pka1=%s,iduser=%s,MD5=%s WHERE idsend=%s",
                           (idsend,ida1,ta1,message1,pka1,iduser,MD5, x))
            cursor.execute("UPDATE `receive` SET idreceive=%s,ida1=%s,ta1=%s,message1=%s,pka1=%s,iduser=%s,MD5=%s WHERE idreceive=%s",
                (idreceive, ida1, ta1, message1, pka1, iduser, MD5, x))
            connection.commit()
        return redirect(url_for('send'))
    return render_template('edit_send.html', send=send)

@app.route('/send/delete/<int:idsend>', methods=['POST'])
def delete_send(idsend):
    with connection.cursor() as cursor:
        cursor.execute("DELETE FROM send WHERE idsend=%s", idsend)
        connection.commit()
    return redirect(url_for('send'))

@app.route('/receive', methods=['GET'])
def receive():
    with connection.cursor() as cursor:
        cursor.execute("SELECT * FROM receive")
        receives = cursor.fetchall()
    return render_template('receive.html', receives=receives)

@app.route('/receive/add', methods=['GET', 'POST'])
def add_receive():
    if request.method == 'POST':
        idreceive=request.form.get('idreceive')
        ida1 = request.form.get('ida1')
        ta1 = request.form.get('ta1')
        message1 = request.form.get('message1')
        pka1 = request.form.get('pka1')
        iduser = request.form.get('iduser')
        MD5 = request.form.get('MD5')
        with connection.cursor() as cursor:
            cursor.execute("INSERT INTO receive (idreceive,ida1,ta1,message1,pka1,iduser,MD5) VALUES (%s,%s,%s,%s,%s,%s,%s)",
                           (idreceive,ida1,ta1,message1,pka1,iduser,MD5))
            connection.commit()
        return redirect(url_for('receive'))
    else:
        return render_template('add_receive.html')

@app.route('/receive/edit/<int:idreceive>', methods=['GET', 'POST'])
def edit_receive(idreceive):
    with connection.cursor() as cursor:
        cursor.execute("SELECT * FROM receive WHERE idreceive=%s", idreceive)
        receive = cursor.fetchone()
    if request.method == 'POST':
        x=idreceive
        idreceive = request.form.get('idreceive')
        ida1 = request.form.get('ida1')
        ta1 = request.form.get('ta1')
        message1 = request.form.get('message1')
        pka1 = request.form.get('pka1')
        iduser = request.form.get('iduser')
        MD5 = request.form.get('MD5')
        with connection.cursor() as cursor:
            cursor.execute("UPDATE `receive` SET idreceive=%s,ida1=%s,ta1=%s, message1=%s,pka1=%s,iduser=%s,MD5=%s WHERE idreceive=%s",
                           (idreceive,ta1,message1,ida1,pka1,iduser,MD5, x))
            connection.commit()
        return redirect(url_for('receive'))
    return render_template('edit_receive.html', receive=receive)

@app.route('/receive/delete/<int:idreceive>', methods=['POST'])
def delete_receive(idreceive):
    with connection.cursor() as cursor:
        cursor.execute("DELETE FROM receive WHERE idreceive=%s", idreceive)
        connection.commit()
    return redirect(url_for('receive'))


if __name__ == '__main__':
    app.run(debug=True)


# 关闭游标和数据库连接
cursor.close()
connection.close()



