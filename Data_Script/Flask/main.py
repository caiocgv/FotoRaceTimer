from flask import Flask, render_template, request, send_file
import yaml
from Athlete_Class import racer
from time_class import Time

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
    """
    Uploads a file and processes its contents to update the `Athletes` list.
    Returns:
        A rendered template of the `index.html` page with the updated `Athletes` list.
    Raises:
        ValueError: If the file name is invalid.
        ValueError: If the file type is invalid.
    """
    
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

        for i in range(len(data)):            
            if ":" in data[i]:
                if file.filename.rsplit('.',1)[0].lower() in ['largada','chegada']:
                    for racers in Athletes:
                        if str(racers.number) == data[i+1]:
                            if file.filename.rsplit('.',1)[0].lower() == 'largada':
                                racers.start = Time(data[i])
                            elif file.filename.rsplit('.',1)[0].lower() == 'chegada':
                                racers.finish = Time(data[i])
                            else:
                                pass
                            if racers.start != None and racers.finish != None:
                                racers.calculate_time()
                            break
                else:
                    return render_template('error.html', error='Invalid file name')
    else:
        return render_template('error.html', error='Invalid file type')

    return render_template('index.html', Athletes=Athletes)

if __name__ == '__main__':
    app.run(debug=True) 
    