from flask import Flask, render_template, request, send_file
import yaml
from Athlete_Class import racer

app = Flask(__name__)
Athletes = []
try:
    with open('athletes.yaml', 'r') as file:
        existing_data = yaml.load(file, Loader=yaml.FullLoader)

    for data in existing_data:
        Athletes.append(racer(data))
except:
    pass

@app.route('/', methods=['GET', 'POST'])
def main():
    global Athletes
    if request.method == 'POST':
        Athletes.append(racer(request.form.getlist('info'))) if request.form.getlist('info')[1] != "" else None
        return render_template('index.html', Athletes=Athletes)
    else:
        return render_template('index.html', Athletes=Athletes)

@app.route('/clear')
def clear():
    global Athletes
    Athletes = []
    save()
    return render_template('index.html', Athletes=Athletes)

@app.route('/save')
def save():
    global Athletes
    athlete_data = [athlete.to_dict() for athlete in Athletes]
    with open('athletes.yaml', 'w') as file:
        yaml.dump(athlete_data, file)
    return render_template('index.html', Athletes=Athletes)

@app.route('/export') 
def download(): 
    save()
    return send_file('athletes.yaml', as_attachment=True)

@app.route('/import', methods=['POST'])
def upload():
    global Athletes
    file = request.files.get('file')
    
    if file.filename.rsplit('.',1)[1].lower() == 'yaml':
        file.save('athletes.yaml')
        with open('athletes.yaml', 'r') as file:
            existing_data = yaml.load(file, Loader=yaml.FullLoader)
        Athletes = [racer(data) for data in existing_data]

    elif file.filename.rsplit('.',1)[1].lower() == 'txt':
        string = file.read().decode('utf-8')
        data = string.split('\r')
        print(data)

    return render_template('index.html', Athletes=Athletes)

if __name__ == '__main__':
    app.run(debug=True) 
    