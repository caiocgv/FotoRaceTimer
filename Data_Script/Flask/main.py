from flask import Flask, render_template, request, send_file
import yaml
from Athlete_Class import racer
from time_class import Time

app = Flask(__name__)

@app.route('/', methods=['GET', 'POST'])
def main():
    global Athletes, categories, flag
    categories = []

    if request.method == 'POST':
        Athletes.append(racer(request.form.getlist('info'))) if request.form.getlist('info')[1] != "" else None
        return render_template('index.html', Athletes=Athletes)
    
    else:
        Athletes = []
        try:
            with open('athletes.yaml', 'r') as file:
                existing_data = yaml.load(file, Loader=yaml.FullLoader)

            for data in existing_data:
                Athletes.append(racer(data))

            if isinstance(Athletes[0].stage, list):
                flag = 'true'
            
            with open('categories.yaml', 'r') as file:
                existing_data = yaml.load(file, Loader=yaml.FullLoader)
            
            for category in existing_data['categories']:
                categories.append(category)
        
        except:
            pass

        return render_template('index.html', Athletes=Athletes, categories=categories, flag=flag)

@app.route('/clear')
def clear():
    global Athletes
    Athletes = []
    save()
    return render_template('index.html', Athletes=Athletes, categories=categories)

@app.route('/save')
def save():
    global Athletes
    athlete_data = [athlete.to_dict() for athlete in Athletes]
    with open('athletes.yaml', 'w') as file:
        yaml.dump(athlete_data, file)
    return render_template('index.html', Athletes=Athletes, categories=categories)

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

    
    global Athletes, flag
    max_stage = 0

    file = request.files.get('file')
    flag = request.form.get('flag')

    if flag == 'true':
        # multi stage race, some variables should be declared as lists to store all data
        for racers in Athletes:
            racers.stage = [] if isinstance(racers.stage, list) == False else racers.stage
            racers.start = [] if isinstance(racers.start, list) == False else racers.start
            racers.finish = [] if isinstance(racers.finish, list) == False else racers.finish
            racers.time = [] if isinstance(racers.time, list) == False else racers.time
    
    if file.filename.rsplit('.',1)[1].lower() == 'yaml':
        file.save('athletes.yaml')
        with open('athletes.yaml', 'r') as file:
            existing_data = yaml.load(file, Loader=yaml.FullLoader)
        Athletes = [racer(data) for data in existing_data]
        return render_template('index.html', Athletes=Athletes)
    elif file.filename.rsplit('.',1)[1].lower() == 'txt':
        string = file.read().decode('utf-8')
        data = string.split('\r')

        for i in range(len(data)):            
            if ":" in data[i]:
                if file.filename.rsplit('.',1)[0].lower() in ['largada','chegada']:

                    if len(Athletes) == 0: # check if there are athletes registered
                        return render_template('error.html', error='No athletes registered')
                    
                    for racers in Athletes:
                        if flag != 'true': 
                            # single stage race, look for the matching number, get the time, break the loop
                            if str(racers.number) == str(data[i+1]):
                                if file.filename.rsplit('.',1)[0].lower() == 'largada':
                                    racers.start = Time(data[i])
        
                                elif file.filename.rsplit('.',1)[0].lower() == 'chegada':
                                    racers.finish = Time(data[i])

                                racers.calculate_time()
                                break

                        else:   # multi stage race, look for all the matching numbers where the last digit is always the stage number
                            if str(racers.number) == str(data[i+1])[:-1]:
                                if data[i+1][-1:] not in racers.stage:  # check if the stage is already in the list
                                    racers.stage.append(data[i+1][-1:]) # if not, add it with the start and finish time

                                    if file.filename.rsplit('.',1)[0].lower() == 'largada':
                                        racers.start.append(Time(data[i]))
                                        racers.finish.append(Time(None))

                                    elif file.filename.rsplit('.',1)[0].lower() == 'chegada':
                                        racers.start.append(Time(None))
                                        racers.finish.append(Time(data[i]))

                                else: # if the stage is already in the list, update the start or finish time
                                    if file.filename.rsplit('.',1)[0].lower() == 'largada':
                                        racers.start[racers.stage.index(data[i+1][-1:])] = Time(data[i])    # find the index of the stage and update the start time

                                    elif file.filename.rsplit('.',1)[0].lower() == 'chegada':
                                        racers.finish[racers.stage.index(data[i+1][-1:])] = Time(data[i])   # find the index of the stage and update the finish time                                
                                
                                racers.calculate_time()  # calculate the time

                                # check if racers.time has at least one valid time
                                if len(racers.time) > 0:
                                    racers.totTime = [Time(None) for time in racers.time]

                                    for i in range(len(racers.time)):
                                        if i == 0:
                                            racers.totTime[i] = racers.time[i]
                                        else:
                                            racers.totTime[i] = racers.totTime[i-1].add(racers.time[i]) # calculate the total time

                    # check the max number of stages
                    if flag == 'true':                                
                        for i in range(len(Athletes)):
                            if len(Athletes[i].stage) > len(Athletes[max_stage].stage):
                                max_stage = i
                                    
                        for stage in Athletes[max_stage].stage:
                            for racers in Athletes:
                                if stage not in racers.stage:
                                    racers.stage.append(stage)
                                    racers.start.append(Time(None))
                                    racers.finish.append(Time(None))
                                    racers.time.append(Time('00:10:00:000'))

                                # check if racers.time has at least one valid time and fill the totTime list
                                if len(racers.time) > 0:
                                    racers.totTime = [Time(None) for time in racers.time]

                                for i in range(len(racers.time)): # calculate the total time
                                    if i == 0:
                                        racers.totTime[i] = racers.time[i]
                                    else:
                                        racers.totTime[i] = racers.totTime[i-1].add(racers.time[i]) # calculate the total time

                else:
                    return render_template('error.html', error='Invalid file name')
    else:
        return render_template('error.html', error='Invalid file type')

    return render_template('index.html', Athletes=Athletes, flag=flag, categories=categories)


@app.route('/results', methods=['GET', 'POST']) 
def results(): 
    global Athletes
    if request.method == 'POST':
        filters = request.form.get('category')
        stage   = request.form.get('stage')

    else:
        if flag == 'true':
            Athletes.sort(key=lambda x: x.totTime[-1].compare())
        else:
            Athletes.sort(key=lambda x: x.time.compare())
        filters = 'all'
        stage   = 'all'

    return render_template('results.html', Athletes=Athletes, categories=categories, filters=filters, stage=stage) 

if __name__ == '__main__':
    app.run(debug=True) 
    