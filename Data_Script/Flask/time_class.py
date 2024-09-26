class Time:
    def __init__(self, time_str):
        if time_str == None:
            self.hour = 0
            self.minute = 0
            self.second = 0
            self.millis = 0
        elif ':' in time_str:
            time_parts = time_str.split(':')
            self.hour = int(time_parts[0])
            self.minute = int(time_parts[1])
            self.second = int(time_parts[2])
            self.millis = int(time_parts[3])
        
    def __str__(self):
        if self.hour < 10:
            hour = f'0{self.hour}'
        else:
            hour = self.hour
        if self.minute < 10:
            minute = f'0{self.minute}'
        else:
            minute = self.minute
        if self.second < 10:
            second = f'0{self.second}'
        else:
            second = self.second
        if self.millis < 10:
            millis = f'00{self.millis}'
        elif self.millis < 100:
            millis = f'0{self.millis}'
        else:
            millis = self.millis
        return f'{hour}:{minute}:{second}:{millis}'
    
    def diff(self, other):
        self_millis = self.hour*3600000 + self.minute*60000 + self.second*1000 + self.millis
        other_millis = other.hour*3600000 + other.minute*60000 + other.second*1000 + other.millis
        diff = self_millis - other_millis
        hour = diff // 3600000
        diff = diff % 3600000
        minute = diff // 60000
        diff = diff % 60000
        second = diff // 1000
        millis = diff % 1000
        return Time(f'{hour}:{minute}:{second}:{millis}')
    
    def sum_up(self, other):
        self_millis = self.hour*3600000 + self.minute*60000 + self.second*1000 + self.millis
        other_millis = other.hour*3600000 + other.minute*60000 + other.second*1000 + other.millis
        sum_up = self_millis + other_millis
        hour = sum_up // 3600000
        sum_up = sum_up % 3600000
        minute = sum_up // 60000
        sum_up = sum_up % 60000
        second = sum_up // 1000
        millis = sum_up % 1000
        return Time(f'{hour}:{minute}:{second}:{millis}')