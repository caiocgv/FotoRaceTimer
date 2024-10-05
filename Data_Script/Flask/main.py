from flask import Flask, render_template, request, send_file
import zipfile
import yaml
from Athlete_Class import racer
from time_class import Time
from pdf_generator import convert_html_to_pdf

app = Flask(__name__)

@app.route('/', methods=['GET', 'POST'])
def main():
    global Athletes, categories, flag

    if request.method == 'POST':
        Athletes.append(racer(request.form.getlist('info'))) if request.form.getlist('info')[1] != "" else None
        flag = 'false'
        save()
        return render_template('index.html', Athletes=Athletes, categories=categories, flag=flag)
    
    else:
        
        categories = []
        flag = 'false'
        Athletes = []
        try:
            with open('athletes.yaml', 'r') as file:
                existing_data = yaml.load(file, Loader=yaml.FullLoader)

            for data in existing_data:
                Athletes.append(racer(data))

            if isinstance(Athletes[0].stage, list):
                flag = 'true'
        
        except:
            pass        
            
        with open('categories.yaml', 'r') as file:
            existing_data = yaml.load(file, Loader=yaml.FullLoader)            
        for category in existing_data['categories']:
            categories.append(category)
        

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

    if len(Athletes)>0 and isinstance(Athletes[0].stage, list):
        flag = 'true'
    else:
        flag = 'false'
    
    return render_template('index.html', Athletes=Athletes, categories=categories, flag=flag)

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

    file = request.files.getlist('file')
    flag = request.form.get('flag')

    if flag == 'true':
        # multi stage race, some variables should be declared as lists to store all data
        for racers in Athletes:
            racers.stage = [] if isinstance(racers.stage, list) == False else racers.stage
            racers.start = [] if isinstance(racers.start, list) == False else racers.start
            racers.finish = [] if isinstance(racers.finish, list) == False else racers.finish
            racers.time = [] if isinstance(racers.time, list) == False else racers.time
    
    if file[0].filename.rsplit('.',1)[1].lower() == 'yaml':
        file[0].save('athletes.yaml')
        
        with open('athletes.yaml', 'r') as file:
            existing_data = yaml.load(file, Loader=yaml.FullLoader)
        Athletes = [racer(data) for data in existing_data]
        if isinstance(Athletes[0].stage, list):
            flag = 'true'

        return render_template('index.html', Athletes=Athletes, flag=flag, categories=categories)
    
    elif file[0].filename.rsplit('.',1)[1].lower() == 'txt':

        for files in file:
            string = files.read().decode('utf-8')
            data = string.split('\r')

            for i in range(len(data)):            
                if ":" in data[i]:
                    if files.filename.rsplit('.',1)[0].lower() in ['largada','chegada']:

                        if len(Athletes) == 0: # check if there are athletes registered
                            return render_template('error.html', error='No athletes registered')
                        
                        for racers in Athletes:
                            if flag != 'true': 
                                # single stage race, look for the matching number, get the time, break the loop
                                if str(racers.number) == str(data[i+1]):
                                    if files.filename.rsplit('.',1)[0].lower() == 'largada':
                                        racers.start = Time(data[i])
            
                                    elif files.filename.rsplit('.',1)[0].lower() == 'chegada':
                                        racers.finish = Time(data[i])

                                    racers.calculate_time()
                                    break

                            else:   # multi stage race, look for all the matching numbers where the last digit is always the stage number
                                if str(racers.number) == str(data[i+1])[:-1]:
                                    if data[i+1][-1:] not in racers.stage:  # check if the stage is already in the list
                                        racers.stage.append(data[i+1][-1:]) # if not, add it with the start and finish time

                                        if files.filename.rsplit('.',1)[0].lower() == 'largada':
                                            racers.start.append(Time(data[i]))
                                            racers.finish.append(Time(None))

                                        elif files.filename.rsplit('.',1)[0].lower() == 'chegada':
                                            racers.start.append(Time(None))
                                            racers.finish.append(Time(data[i]))

                                    else: # if the stage is already in the list, update the start or finish time
                                        if files.filename.rsplit('.',1)[0].lower() == 'largada':
                                            racers.start[racers.stage.index(data[i+1][-1:])] = Time(data[i])    # find the index of the stage and update the start time

                                        elif files.filename.rsplit('.',1)[0].lower() == 'chegada':
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
    save()
    return render_template('index.html', Athletes=Athletes, flag=flag, categories=categories)


@app.route('/results', methods=['GET', 'POST']) 
def results(): 
    global Athletes, flag, categories

    if request.method == 'POST':
        filters = request.form.get('category')
        special   = request.form.get('stage')

        if special != 'all':
            Athletes.sort(key=lambda x: x.time[x.stage.index(special)].compare())  
        else:
            if flag == 'true':
                Athletes.sort(key=lambda x: x.totTime[-1].compare())
            else:
                Athletes.sort(key=lambda x: x.time.compare())
                flag = 'false'
    else:
        if flag == 'true':
            Athletes.sort(key=lambda x: x.totTime[-1].compare())
        else:
            Athletes.sort(key=lambda x: x.time.compare())
            flag = 'false'
        filters = 'all'
        special = 'all'
    
    print (render_template('results.html', Athletes=Athletes, categories=categories, filters=filters, stage=special, flag=flag))
    return render_template('results.html', Athletes=Athletes, categories=categories, filters=filters, stage=special, flag=flag) 

@app.route('/export_pdf', methods=['GET', 'POST'])
def export_pdf():
    global Athletes, flag
    if request.method == 'POST':
        if flag == 'true':
            output = []
            for special in Athletes[0].stage:
                for filters in categories: # loop through all categories to export especific results for each category in each stage
                    Athletes.sort(key=lambda x: x.time[x.stage.index(special)].compare())
                    
                    # skip category if there are no athletes in it
                    size = len([athlete for athlete in Athletes if athlete.category == filters])
                    if size == 0:
                        continue
                    
                    output.append('Resultado Categoria - ' + filters + ' - Especial ' + special + '.pdf')
                    size = '58mm ' + str(size*6+30) + 'mm'
                    
                    
                    file_title = output[len(output)-1]
                    convert_html_to_pdf(render_template('export_results.html', Athletes=Athletes, filters=filters, stage=special, output=file_title[:-4], size=size), file_title)

            for special in Athletes[0].stage: # loop through all stages to export the general results for each stage
                Athletes.sort(key=lambda x: x.time[x.stage.index(special)].compare())
                    
                # skip category if there are no athletes in it
                size = len([athlete for athlete in Athletes])
                    
                output.append('Resultado Geral' + ' - Especial ' + special + '.pdf') # define filename and title
                size = '58mm ' + str(size*6+30) + 'mm' # define the size of the pdf
                
                file_title = output[len(output)-1]
                convert_html_to_pdf(render_template('export_results.html', Athletes=Athletes, filters='all', stage=special, output=file_title[:-4], size=size), file_title)

            for filters in categories: # loop through all categories to export the general results for each category
                Athletes.sort(key=lambda x: x.totTime[-1].compare())
                    
                # skip category if there are no athletes in it
                size = len([athlete for athlete in Athletes if athlete.category == filters])
                if size == 0:
                    continue
                    
                output.append('Resultado Categoria - ' + filters + ' - Geral.pdf') # define filename and title
                size = '58mm ' + str(size*6+30) + 'mm'

                file_title = output[len(output)-1]

                convert_html_to_pdf(render_template('export_results.html', Athletes=Athletes, filters=filters, stage='all', output=file_title[:-4], size=size), file_title)
            
            # export the general results for all categories
            size = len([athlete for athlete in Athletes])
            output.append('Resultado Geral.pdf')
            size = '58mm ' + str(size*6+30) + 'mm'

            file_title = output[len(output)-1]
            convert_html_to_pdf(render_template('export_results.html', Athletes=Athletes, filters='all', stage='all', output=file_title[:-4], size=size), file_title)
        
        else:
            output = []
            for filters in categories: # loop through all categories to export especific results for each category
                Athletes.sort(key=lambda x: x.time.compare())
                    
                # skip category if there are no athletes in it
                size = len([athlete for athlete in Athletes if athlete.category == filters])
                if size == 0:
                    continue
                    
                output.append('Resultado Categoria - ' + filters + '.pdf')
                size = '58mm ' + str(size*6+30) + 'mm'
                
                file_title = output[len(output)-1]
                convert_html_to_pdf(render_template('export_results.html', Athletes=Athletes, filters=filters, stage='all', output=file_title[:-4], size=size, flag=flag), file_title)
            
            # export the general results for all categories
            size = len([athlete for athlete in Athletes])
            output.append('Resultado Geral.pdf')
            size = '58mm ' + str(size*6+30) + 'mm'

            file_title = output[len(output)-1]
            convert_html_to_pdf(render_template('export_results.html', Athletes=Athletes, filters='all', stage='all', output=file_title[:-4], size=size, flag=flag), file_title)

        print(output)
        with zipfile.ZipFile('results.zip', 'w') as zipf:
            for file in output:
                zipf.write(file)
        return send_file('results.zip', as_attachment=True)
    
if __name__ == '__main__':
    app.run(debug=True) 
    