from time_class import Time

class racer:
    def __init__(self, data):
        if isinstance(data, list):
            self.category = data[0]
            self.name = data[1]
            self.number = data[2]
            self.start = None
            self.finish = None
            self.time = None
        elif isinstance(data, dict):
            self.category = data.get('category')
            self.name = data.get('name')
            self.number = data.get('number')
            self.start = Time(data.get('start'))
            self.finish = Time(data.get('finish'))
            self.time = Time(data.get('time'))
    
    def to_dict(self):
        return {
            'category': self.category,
            'name': self.name,
            'number': self.number,
            'start': str(self.start),
            'finish': str(self.finish),
            'time': str(self.time)
        }
    def calculate_time(self):
        if str(self.start).split(':')[0] != '00' and str(self.finish).split(':')[0] != '00':
            self.time = self.finish.diff(self.start)