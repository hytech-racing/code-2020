from flask import Flask 
app = Flask(__name__)

from flask import render_template

data = [
    {
        "displayName": "MODULE A TEMP",
        "id": "module-a-temp",
    },

    {
        "displayName": "MODULE B TEMP",
        "id": "module-b-temp",

    },
    {
        "displayName": "MODULE C TEMP",
        "id": "module-c-temp",
        "value": 3
    }

]

@app.route('/hello/')
def hello():
    return render_template('hello2.html', data=data)

if __name__ == '__main__':
    app.run(debug=True)
