"""
python3 + flask 
"""

from flask import Flask 
from flask import request, session

app = Flask(__name__)

@app.route('/login', methods=['POST', 'GET'])
def login():
    # 如果是一个post请求
    if request.method == 'POST':
        if request.form['username'] == 'admin' or request.form['password'] == '123':
            session['username'] = request.form['username']
            return 'Admin login successfully!'
        else:
            return 'No such user!'
    # 如果是一个get请求
    print ("--test-- :",session)
    if 'username' in session:
        #如果创建过session,也就是说sesson里面会username的key,那么表示这个已经登录过
        return 'Hello %s!' % session['username']
    # 如果没有登录
    else:
        title = request.args.get('title', 'Default')
        return '''
                <form action = "/login" method = "post">
                <p><input name="username"> </p>
                <p><input name="password"> </p>
                <p><button type="submit"> sign in </button> </p>
                </form>
                '''
 
app.secret_key = '123456'

if __name__ == '__main__':
    # 在浏览器里面输入 http://127.0.0.1:5555/login
    app.run(debug=True,port=5555)