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
            self.start = None
            self.finish = None
            self.time = None
    
    def to_dict(self):
        return {
            'category': self.category,
            'name': self.name,
            'number': self.number,
            'start': self.start,
            'finish': self.finish,
            'time': self.time
        }