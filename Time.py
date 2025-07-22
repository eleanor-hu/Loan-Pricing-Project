import datetime
from datetime import date
import calendar
from typing import Tuple

def is_leap_year(year: int) -> bool:
    """True if year is a leap year and False otherwise."""
    if year == 1900:
        return False
    if year % 400 == 0:
        return True
    if (year % 4) == 0:
        if year % 100 == 0:
            return False
        return True
    return False

def is_imm_date(d:datetime.date) -> bool:
    """True if 20th day of Mar/Jun/Sep/Dec"""
    return d.day==20 and d.month in (3, 6, 9, 12)

def days_diff(d1: datetime.date, d2: datetime.date) -> int:
    """
    Calculate the difference in days between two datetime.date objects.
    """
    # Ensure d2 >= d1
    if d2 < d1:
        d1, d2 = d2, d1

    return (d2 - d1).days

def dmy_to_excel_serial(dd: int, m: int, y: int) -> int:
    """
    Convert a date (day, month, year) to an Excel serial number.
    Compensate for the 1900 leap year bug present in Excel.
    """
    # Excel bug for 29-Feb-1900. 1900 is not a leap year, but 
    # Excel/Lotus 123 think it is
    if dd == 29 and m == 2 and y == 1900:
        return 60
    
    d = datetime.date(y, m, dd)
    excel_base = datetime.date(1899, 12, 31)
    serial = (d - excel_base).days
    
    # Excel inserts a phantom leap-day on Feb 29, 1900,
    # so bump every date from Mar 1, 1900 onward by +1
    if d >= datetime.date(1900, 3, 1):
        serial += 1
    
    return serial

def date_to_excel_serial(d: datetime.date) -> int:
    """
    Convert a Python date into Excel serial date number as a float.
    Excel stores dates as the number of days since 1899-12-31,
    and (mistakenly) treats 1900 as a leap year by inserting
    a phantom 29-Feb-1900.  This function:
        1. Compute the true day count from 1899-12-31 → d.
        2. Add 1 extra day for all dates on or after 1900-03-01
    to replicate Excel leap year behavior.
    """
    return dmy_to_excel_serial(d.day, d.month, d.year)

def excel_serial_to_dmy(serial: int) -> Tuple[int, int, int]:
    """
    Convert an Excel serial date back into a date (day, month, year)
    This is the reverse of dmy_to_excel_serial.
    """
    # Excel bug for 29-Feb-1900. 1900 is not a leap year, but 
    # Excel/Lotus 123 think it is
    base = datetime.date(1899, 12, 31)
    if serial == 60:
        return (29, 2, 1900)
    if serial > 60:
        serial -= 1
    real_date = base + datetime.timedelta(days=serial)
    return (real_date.day, real_date.month, real_date.year)

def excel_serial_to_date(serial: int) -> datetime.date:
    """
    Convert an Excel serial date back into a datetime.date object
    """
    dd, m, y = excel_serial_to_dmy(serial)
    return datetime.date(y, m, dd)

def days_in_year(y:int) -> float
    """
    365 or 366 depending on leap year
    """
    if is_leap_year(y):
        return 366.0
    else: 
        return 365.0

def add_months(d: datetime.date, months: int) -> datetime.date:
    year_inc, new_mon = divmod(d.month - 1 + months, 12)
    y = d.year + year_inc
    m = new_mon + 1
    day = min(d.day, calendar.monthrange(y, m)[1])
    return datetime.date(y, m, day)

def sub_months(d: datetime.date, months: int) -> datetime.date:
    return add_months(d, - months)

def add_quarters(d: datetime.date, quar: int) -> datetime.date:
    return add_months(d, 3 * quar)

def add_one_day(d: datetime.date) -> datetime.date:
    return d + datetime.timedelta(days=1)

def add_years(d: datetime.date, years: int) -> datetime.date:
    """
    Need to handle leap year Feb-29 and non-leap year Feb-28
    """
    y = d.year + years
    if d.month == 2 and d.day == 29:
        if not is_leap_year(y):
            dd = 28
        else:
            dd = 29
    return datetime.date(y, d.month, dd)

def day_count(d1: datetime.date, d2: datetime.date, dcc: int) -> float:
    """
    Calculate the day count between two dates based on the 
    specified Day Count Convention (DCC).

    Day Count Conventions (DCC):
        0: Act/360
        1: Act/365
        2: Act/365 with adjustment for leap year
        3: 30/360 USA(NASD Bond-Basis)
    """
    # Ensure d2 >= d1
    if d2 < d1:
        d1, d2 = d2, d1

    # Case 0: ACT/360
    if dcc == 0:
        return days_diff(d1, d2) / 360.0
    
    # Case 1: ACT/365
    elif dcc == 1:
        return days_diff(d1, d2) / 365.0
    
    # Case 2: Act/365 with adjustment for leap year
    elif dcc == 2:
        y1, y2 = d1.year, d2.year
        # same calendar year
        if y2 - y1 == 0:
            return days_diff(d1, d2) / days_in_year(y1)
        
        # spanning different years
        elif y2 - y1 >= 1:
            end = datetime.date(y1 + 1, 1, 1)
            beg = datetime.date(y2, 1, 1)
            return (
                (y2 - y1 - 1) + 
                days_diff(d1, end) / days_in_year(y1) + 
                days_diff(beg, d2) / days_in_year(y2)
            )

    # Case 3: 30/360
    elif dcc == 3:
        y1, m1, dd1 = d1.year, d1.month, d1.day
        y2, m2, dd2 = d2.year, d2.month, d2.day

        # same calendar year
        if y2 - y1 == 0:
            #same month
            if m1 == m2:
                return (dd2 - dd1) / 360.0
            # different month, day1 <= day2
            elif dd1 <= dd2:
                return (30 * (m2 - m1) + (dd2 - dd1)) / 360.0
            # different month, day2 > day1
            elif dd2 > dd1:
                pivot = datetime.date(y2, m2, dd1)
                pivot = add_months(pivot, -1)
                return (30 * (m2 - m1) + days_diff(pivot, d2)) / 360.0
            
        # different year but time between d1 and d2 inferior at 1 year
        if y2 - y1 == 1 and m2 <= m1:
            if dd1 <= dd2:
                return (30 * (12 + m2 - m1) + (dd2 - dd1)) / 360.0
            elif dd1 > dd2:
                pivot = datetime.date(y2, m2, dd1)
                pivot = add_months(pivot, -1)
                return (30 * (12 + m2 - m1 - 1) + days_diff(pivot, d2)) / 360.0
        
        # different year but time between d1 and d2 superior at 1 year
        if m2 >= m1:
            pivot = datetime.date(y2, m1, dd1)
            return (y2 - y1) + day_count(pivot, d2, dcc)
        elif m2 < m1:
            pivot = datetime.date(y2 - 1, m1, dd1)
            return (d2.tm_year) + day_count(pivot, d2, dcc)

    # Case 4: 30/365
    elif dcc == 4:
        y1, m1, dd1 = d1.year, d1.month, d1.day
        y2, m2, dd2 = d2.year, d2.month, d2.day

        # same calendar year
        if y2 - y1 == 0:
            # same month
            if m1 == m2:
                return (dd2 - dd1) / 365.0
            # different month, day1 <= day2
            elif dd1 <= dd2:
                return (30 * (m2 - m1) + (dd2 - dd1)) / 365.0
            # different month, day2 > day1
            elif dd2 > dd1:
                pivot = datetime.date(y2, m2, dd1)
                pivot = add_months(pivot, -1)
                return (30 * (m2 - m1 - 1) + days_diff(pivot, d2)) / 365.0

        # different year but time between d1 and d2 inferior at 1 year
        if y2 - y1 == 1 and m2 <= m1:
            if dd1 <= dd2:
                return (30 * (12 + m2 - m1) + (dd2 - dd1)) / 365.0
            elif dd2 > dd2:
                pivot = datetime.date(y2, m2, dd1)
                pivot = add_months(pivot, -1)
                return (30 * (12 + m2 - m1 - 1)+ days_diff(pivot, d2)) / 365.0

        # different year but time between d1 and d2 superior at 1 year
        if m2 >= m1:
            pivot = datetime.date(y2, m1, dd1)
            return (y2 - y1) + day_count(pivot, d2, dcc)
        elif m2 < m1:
            pivot = datetime.date(y2 - 1, m1, dd1)
            return (y2 - y1 - 1) + day_count(pivot, d2, ddc)
    
    # Case 5: 30/360 with annualized adjustment for leap year
    elif dcc == 5:
        y1, m1, dd1 = d1.year, d1.month, d1.day
        y2, m2, dd2 = d2.year, d2.month, d2.day

        if y2 - y1 == 0:
            return day_count(d1, d2, 3) * 360 / days_in_year(y1)

        elif y2 - y1 >= 1:
            end = datetime.date(y1 + 1, 1, 1)
            beg = datetime.date(y2, 1, 1)
            return (
                (y2 - y1 - 1) + 
                day_count(d1, end,3) * 360.0 / days_in_year(y1) + 
                day_count(beg, d2, 3) * 360.0 / days_in_year(y2)
            )

def next_business_day(d: datetime.date) -> datetime.date:
    """
    Move forward past weekends
    """
    while d.weekday() >= 5:   # 5 for Sat, 6 for Sun
        d += datetime.timedelta(days=1)
    return d

def prev_business_day(d: datetime.date) -> datetime.date:
    """
    Move backward past weekends
    """
    while d.weekday() >= 5:
        d -= datetime.timedelta(days=1)
    return d

def business_day_adjust(d: datetime.date, bda = 0: int) -> datetime.date:
    """
    business day adjustment:
        0: no change (default case)
        1: following business day
        2: previous business day
        3: modified following
        4: modified previous
    """
    # no change (default case)
    if bda == 0:
        return d
    
    # following business day
    if bda == 1:
        return next_business_day(d)
    
    # previous business day
    if bda == 2:
        return prev_business_day(d)
    
    # modified following
    if bda == 3:
        nd = next_business_day(d)
        if nd.month != d.month:
            return prev_business_day(d)
        else:
            return nd
    
    # modified previous
    if bda == 4:
        pd = previous_business_day(d)
        if pd.month != d.month:
            return next_business_day(d)
        else:
            return pd

def first_imm_date(d: datetime.date, freq = 1: int) -> datetime.date:
    """
    Find the first IMM date AFTER the valuation date.
    IMM dates are always the 20th of Mar/Jun/Sep/Dec.

    Inputs:
        d (datetime.date): the valuation date
        freq (int): the number of quarters to step forward, default to 1
    """
    y = d.year
    imm = datetime.date(y, 3, 20)
    while imm <= d:
        imm = add_months(imm, 3 * freq)
    return imm

