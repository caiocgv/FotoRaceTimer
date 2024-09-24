from flask import Flask, render_template, request
from Athlete_Class import racer

app = Flask(__name__)
Athletes = []

@app.route('/', methods=['GET', 'POST'])
def main():
    global Athletes
    if request.method == 'POST':
        Athletes.append(racer(request.form.getlist('info')))
        return render_template('index.html', Athletes=Athletes)
    else:
        return render_template('index.html', Athletes=Athletes)

@app.route('/clear')
def clear():
    global Athletes
    Athletes = []
    return render_template('index.html', Athletes=Athletes)

if __name__ == '__main__':
    app.run(debug=True) 
    