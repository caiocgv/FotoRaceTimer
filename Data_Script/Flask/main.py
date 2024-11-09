from flask import Flask, render_template, request, send_file
import zipfile
import yaml

from Athlete_Class import racer
from time_class import Time, is_time
from pdf_generator import convert_html_to_pdf

app = Flask(__name__)

@app.route('/', methods=['GET', 'POST'])
def main():
    global Athletes, categories, flag, calib_times

    if request.method == 'POST':
        Athletes.append(racer(request.form.getlist('info'))) if request.form.getlist('info')[1] != "" else None
        flag = 'false'
        save()
        return render_template('index.html', Athletes=Athletes, categories=categories, flag=flag, calib = calib_times[2])

    else:

        categories = []
        calib_times = []
        flag = 'false'
        Athletes = []

        try:
            absolute_path = 'athletes.yaml'
            with open(absolute_path, 'r') as file:
                existing_data = yaml.load(file, Loader=yaml.FullLoader)
        except:
            absolute_path = '/home/vianacc/athletes.yaml'
            with open(absolute_path, 'r') as file:
                existing_data = yaml.load(file, Loader=yaml.FullLoader)

        if len(existing_data) > 0:
            for data in existing_data:
                Athletes.append(racer(data))

            if isinstance(Athletes[0].stage, list):
                flag = 'true'


        try: # load categories and calibration times from relative path
            absolute_path = 'categories.yaml'
            with open(absolute_path, 'r') as file:
                existing_data = yaml.load(file, Loader=yaml.FullLoader)
        except: # load categories and calibration times from absolute path
            absolute_path = '/home/vianacc/categories.yaml'
            with open(absolute_path, 'r') as file:
                existing_data = yaml.load(file, Loader=yaml.FullLoader)

        try:
            for category in existing_data['categories']:
                categories.append(category)
        except:
            pass

        try:
            if existing_data['calib_times']:
                Time.calibrate(Time(existing_data['calib_times'][0]), Time(existing_data['calib_times'][1]))
                calib_times = [Time(existing_data['calib_times'][0]), Time(existing_data['calib_times'][1])]
                diff = calib_times[0].compare() - calib_times[1].compare()
                calib_times.append(diff/1000)
            return render_template('index.html', Athletes=Athletes, categories=categories, flag=flag, calib = calib_times[2])
        except:
            pass


        return render_template('index.html', Athletes=Athletes, categories=categories, flag=flag)

@app.route('/clear')
def clear():
    global Athletes, categories, calib_times, flag
    Athletes = []
    categories = []
    flag = 'false'
    save()
    save_categories()
    return render_template('index.html', Athletes=Athletes, categories=categories, flag=flag, calib = calib_times[2])

def save():
    global Athletes, categories, flag
    athlete_data = [athlete.to_dict() for athlete in Athletes]
    with open('athletes.yaml', 'w') as file:
        yaml.dump(athlete_data, file)

@app.route('/export')
def download():
    save()
    try:
        absolute_path = '/home/vianacc/athletes.yaml'
        return send_file(absolute_path , as_attachment=True)
    except:
        absolute_path = 'athletes.yaml'
        return send_file(absolute_path , as_attachment=True)

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


    global Athletes, flag, categories
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
            string = string.replace('\r\n', '\r').replace('\n', '\r')
            data = string.split('\r')

            for i in range(len(data)):
                if ":" in data[i] and is_time(data[i]):
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

    return render_template('results.html', Athletes=Athletes, categories=categories, filters=filters, stage=special, flag=flag)

@app.route('/export_pdf', methods=['GET', 'POST'])
def export_pdf():
    global Athletes, flag
    if request.method == 'POST':
        if flag == 'true':
            output = []
            output_html = ''
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
                    output_html += render_template('export_results.html', Athletes=Athletes, filters=filters, stage=special, output=file_title[:-4], size=size)

            for special in Athletes[0].stage: # loop through all stages to export the general results for each stage
                Athletes.sort(key=lambda x: x.time[x.stage.index(special)].compare())

                # skip category if there are no athletes in it
                size = len([athlete for athlete in Athletes])

                output.append('Resultado Geral' + ' - Especial ' + special + '.pdf') # define filename and title
                size = '58mm ' + str(size*6+30) + 'mm' # define the size of the pdf

                file_title = output[len(output)-1]
                convert_html_to_pdf(render_template('export_results.html', Athletes=Athletes, filters='all', stage=special, output=file_title[:-4], size=size), file_title)
                output_html += render_template('export_results.html', Athletes=Athletes, filters='all', stage=special, output=file_title[:-4], size=size)

            for filters in categories: # loop through all categories to export the general results for each category
                Athletes.sort(key=lambda x: x.totTime[-1].compare())

                # skip category if there are no athletes in it
                size = len([athlete for athlete in Athletes if athlete.category == filters])
                if size == 0:
                    continue

                output.append('Resultado Categoria - ' + filters + ' - Geral.pdf') # define filename and title
                size = '58mm ' + str(size*6+30) + 'mm'

                file_title = output[len(output)-1]

                convert_html_to_pdf(render_template('export_results.html', Athletes=Athletes, filters=filters, stage='all', output=file_title[:-4], size=size, flag=flag), file_title)
                output_html += render_template('export_results.html', Athletes=Athletes, filters=filters, stage='all', output=file_title[:-4], size=size, flag=flag)

            # export the general results for all categories
            size = len([athlete for athlete in Athletes])
            output.append('Resultado Geral.pdf')
            size = '58mm ' + str(size*6+30) + 'mm'

            file_title = output[len(output)-1]
            convert_html_to_pdf(render_template('export_results.html', Athletes=Athletes, filters='all', stage='all', output=file_title[:-4], size=size, flag=flag), file_title)
            output_html += render_template('export_results.html', Athletes=Athletes, filters='all', stage='all', output=file_title[:-4], size=size, flag=flag)
            # return output_html

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

        with zipfile.ZipFile('results.zip', 'w') as zipf:
            for file in output:
                zipf.write(file)

        try:
            absolute_path = 'results.zip'
            return send_file(absolute_path, as_attachment=True)
        except:
            absolute_path = '/home/vianacc/results.zip'
            return send_file(absolute_path, as_attachment=True)

@app.route('/category', methods=['POST'])
def update_category():
    global categories, calib_times

    new_category = request.form.get('categoria')
    calib = request.files.getlist('calib')

    if new_category:
        if new_category not in categories: # check if the category is already in the list
            categories.append(new_category)
            save_categories()
            if calib_times:
                return render_template('index.html', Athletes=Athletes, categories=categories, flag=flag, calib = calib_times[2])
            else:
                return render_template('index.html', Athletes=Athletes, categories=categories, flag=flag)
        else:
            return render_template('error.html', error='Categoria já cadastrada')
    else:
        if calib and calib[0].filename.rsplit('.', 1)[1].lower() == 'txt':
            for file in calib:
                if file.filename.rsplit('.', 1)[0].lower() in ['largada', 'chegada']:
                    string = file.read().decode('utf-8')
                    data = string.split('\r')

                    for i in range(len(data)):
                        if ":" in data[i]:
                            if file.filename.rsplit('.', 1)[0].lower() == 'largada':
                                calib_start = Time(data[i])
                                break
                            elif file.filename.rsplit('.', 1)[0].lower() == 'chegada':
                                calib_finish = Time(data[i])
                                break
            if calib_start and calib_finish:
                Time.calibrate(calib_finish, calib_start)
                calib_times = [str(calib_finish), str(calib_start)]
                save_categories()
                diff = calib_finish.compare() - calib_start.compare() # get time difference in millis, avoid compensation of 'diff' method
                calib_times.append(diff/1000)
                return render_template('index.html', Athletes=Athletes, categories=categories, flag=flag, calib = diff/1000)
            else:
                return render_template('error.html', error='Arquivos de calibração inválido')
        else:
            return render_template('index.html', Athletes=Athletes, categories=categories, flag=flag, calib = diff/1000)



def save_categories():
    global categories, calib_times

    category_data = {'categories': categories,
                     'calib_times': [str(calib) for calib in calib_times]}

    with open('categories.yaml', 'w') as file:
        yaml.dump(category_data, file)


@app.route('/special_results', methods=['POST'])
def special_results():
    """
    Route handler for special_results endpoint.
    This function handles the POST request to the special_results endpoint. It processes the specialResults data received from the request form 
    and updates the stage and time attributes of the Athletes.
    Returns:
        A rendered template 'results.html' with the updated Athletes data, categories, filters, stage, and flag.
    """


    global Athletes, flag, categories

    if request.method == 'POST':
        specialResults = request.form.get('specialResults')
        for separator in [';',',','\t','\r','\n','\r\n','|','/',':',' ','-']:
            if separator in specialResults:
                Results = specialResults.split(separator)
                separator = True
                break
            if separator == '-':
                Results = specialResults
        
        for athlete in Athletes:
            sIndex = []

            for special in Results:
                if special in athlete.stage:
                    sIndex.append(athlete.stage.index(special))
                else:
                    return render_template('error.html', error='Estágio não existente.')
                
            for i in range(len(sIndex)):
                if i == 0:
                    newTottime = athlete.time[sIndex[i]]
                else:
                    newTottime = newTottime.add(athlete.time[sIndex[i]])

            athlete.stage.append(specialResults)    # add the new stage sommatory to the list
            athlete.time.append(newTottime)         # add the new stage sommatory to the list        

        return render_template('results.html', Athletes=Athletes, categories=categories, filters='all', stage=specialResults, flag=flag)

@app.route('/edit_info/<id>', methods=['GET', 'POST'])
def edit_info(id):
        global Athletes, categories, flag, calib_times

        if request.method == 'POST':
            athlete = next((athlete for athlete in Athletes if athlete.number == id), None)
            if athlete:
                athlete.update_info(request.form.getlist('info'))
                save()
                return render_template('index.html', Athletes=Athletes, categories=categories, flag=flag, calib = calib_times[2])
            
        else:
                athlete = next((athlete for athlete in Athletes if athlete.number == id), None)
                if athlete:
                    return render_template('edit_info.html', athlete=athlete, categories=categories, flag=flag, title='Editar Atleta')
                else:
                    return render_template('error.html', error='Athlete not found')
        




if __name__ == '__main__':
    app.run(debug=True)
