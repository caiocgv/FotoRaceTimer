from time_class import Time
"""
Represents a racer in a race.
Attributes:
    category (str): The category of the racer.
    name (str): The name of the racer.
    number (int): The number of the racer.
    start (Time): The start time of the racer.
    finish (Time): The finish time of the racer.
    time (Time): The total time taken by the racer.
Methods:
    to_dict(): Converts the racer object to a dictionary.
    calculate_time(): Calculates the total time taken by the racer.
"""

class racer:
    def __init__(self, data):
        if isinstance(data, list):
            self.category = data[0]
            self.name = data[1]
            self.number = data[2]
            self.stage = []
            self.start = Time(None)
            self.finish = Time(None)
            self.time = Time(None)
            self.totTime = Time(None)
        elif isinstance(data, dict):
            if isinstance(data.get('start'), list) and isinstance(data.get('finish'), list):
                self.category = data.get('category')
                self.name = data.get('name')
                self.number = data.get('number')
                self.stage = data.get('stage')
                self.start = [Time(start) for start in data.get('start')]
                self.finish = [Time(finish) for finish in data.get('finish')]
                self.time = [Time(time) for time in data.get('time')]
                self.totTime = [Time(totTime) for totTime in data.get('totTime')]
            else:
                self.category = data.get('category')
                self.name = data.get('name')
                self.number = data.get('number')
                self.stage = data.get('sNumber')
                self.start = Time(data.get('start'))
                self.finish = Time(data.get('finish'))
                self.time = Time(data.get('time'))
                self.totTime = Time(data.get('totTime'))
        
    def to_dict(self):
        if isinstance(self.start, list) and isinstance(self.finish, list):
            dictionary = {
                'category': self.category,
                'name': self.name,
                'number': self.number,
                'stage': self.stage,
                'start': [str(start) for start in self.start],
                'finish': [str(finish) for finish in self.finish],
                'time': [str(time) for time in self.time],
                'totTime': [str(totTime) for totTime in self.totTime]  if self.totTime != None else None
            }
        else:
            dictionary = {
                'category': self.category,
                'name': self.name,
                'number': self.number,
                'stage': self.stage,
                'start': str(self.start),
                'finish': str(self.finish),
                'time': str(self.time),
                'totTime': str(self.totTime)
            }
        return dictionary
    def calculate_time(self):
        if isinstance(self.start, list) and isinstance(self.finish, list):
            self.time = []
            try:
                for i in range(len(self.start)):
                    if str(self.start[i]).split(':')[0] != '00' and str(self.finish[i]).split(':')[0] != '00':
                        self.time.append(self.finish[i].diff(self.start[i]))
                    else:
                        self.time.append(Time('00:10:00:000'))
            except:
                pass
        else:
            if str(self.start).split(':')[0] != '00' and str(self.finish).split(':')[0] != '00':
                self.time = self.finish.diff(self.start)

    def update_info(self, data):
        """
        Updates the information of the athlete based on the provided data.
        Parameters:
        - data (list): A list containing the updated information of the athlete. The list should have the following structure:
            - data[0] (str): The category of the athlete.
            - data[1] (str): The name of the athlete.
            - data[2] (int): The number of the athlete.
            - data[3:] (varies): Additional data for each stage of the athlete. The number of elements in this part should be a multiple of 3, where each group of 3 elements represents the stage, start time, and finish time of a particular stage.
        Returns:
        - None
        Note:
        - If the athlete has stages already defined, the existing stages will be updated with the provided data.
        - If the athlete does not have any stages defined, the start and finish times will be updated directly.
        Example usage:
        athlete = Athlete()
        athlete.update_info(['Category A', 'John Doe', 1, 'Stage 1', '10:00:00', '11:00:00', 'Stage 2', '12:00:00', '13:00:00'])
        """

        if self.stage != []:

            self.category = data[0]
            self.name = data[1]
            self.number = data[2]
            print(data)
            j=3
            for i in range(len(self.stage)):
                self.stage[i] = data[j]
                print(data[j])
                j+=1
                self.start[i] = Time(data[j])
                print(data[j])
                j+=1
                self.finish[i] = Time(data[j])
                print(data[j])
                j+=1
        else:
            self.category = data[0]
            self.name = data[1]
            self.number = data[2]
            self.start = Time(data[3])
            self.finish = Time(data[4])
        
        self.calculate_time()