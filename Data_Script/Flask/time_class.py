class Time:
    """
    Represents a time in hours, minutes, seconds, and milliseconds.
    Args:
        time_str (str): A string representing the time in the format "hour:minute:second:millis".
    Attributes:
        hour (int): The hour component of the time.
        minute (int): The minute component of the time.
        second (int): The second component of the time.
        millis (int): The millisecond component of the time.
    Methods:
        __str__(): Returns a string representation of the time in the format "hour:minute:second:millis".
        diff(other): Calculates the difference between two Time objects and returns a new Time object representing the difference.
        sum_up(other): Calculates the sum of two Time objects and returns a new Time object representing the sum.
        compare(): Returns the time in milliseconds.
        calibrate(time1, time2): Calibrates the time difference between two Time objects.
    """

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
        global calibration_offset
        self_millis = self.hour*3600000 + self.minute*60000 + self.second*1000 + self.millis
        other_millis = other.hour*3600000 + other.minute*60000 + other.second*1000 + other.millis
        diff = self_millis - other_millis + calibration_offset # correction factor for RTC desync
        hour = diff // 3600000
        diff = diff % 3600000
        minute = diff // 60000
        diff = diff % 60000
        second = diff // 1000
        millis = diff % 1000
        return Time(f'{hour}:{minute}:{second}:{millis}')
    
    def add(self, other):
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
    
    def compare(self):
        self_millis = self.hour*3600000 + self.minute*60000 + self.second*1000 + self.millis
        return self_millis
    
    def calibrate(time1, time2):
        global calibration_offset
        diff = time1.compare() - time2.compare()
        calibration_offset = diff