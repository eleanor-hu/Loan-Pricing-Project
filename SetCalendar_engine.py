import datetime
from datetime import date
from typing import List, Tuple
import Time
from Time import (
    day_count,
    add_months,
    sub_months,
    days_diff,
    business_day_adjust,
    first_imm_date
)

def set_df_calendar(vd: date, 
                    dates: list[date],
                    dcc: int) -> tuple[list[float], list[float]]:
    """
    Set up a schedule for discount factor calculations. Given a value date
    and a list of future cashflow dates, this function calculates the time 
    intervals between consecutive dates and the total elapsed time from 
    the value date to each date.

    Inputs:
        vd (date): value date
        dates (list[date]): a list of future cashflow dates
        dcc (int): day count convention code for use
    
    Returns (tuple[list[float], list[float]]):
        a list of year fraction between each consecutive pair of dates 
        and a list of cumulative time from vd to each date in the schedule.
    """
    n = len(dates)
    interv = [0.0] * n
    cumul = [0.0] * (n + 1)

     # first period (valuation date)
    interv[0] = day_count(vd, dates[0], dcc)
    cumul[0] = 0.0
    cumul[1] = interv[0]

    # remaining periods
    for i in range(1, n):
        interv[i] = day_count(dates[i-1], dates[i], dcc)
        cumul[i + 1] = cumul[i] + interv[i]
    
    return interv, cumul

def get_intensities_period(vd: date, freq: int, imm: int, spread: int) -> int:
    """
    Calculate the number of periods for default intensity

    Inputs:
        vd (date): value date
        freq (int): months in a period
        imm (int): 0 if vd is IMM and 1 if vd is non-IMM
        spread (int): number of spreads per year
    """
    periods = spread * (12 // freq)
    # IMM
    if imm == 0:
        # Check if vd is an IMM date (20th of Mar, Jun, Sep, Dec)
        if vd.day == 20 and vd.month in [3, 6, 9, 12]:
            return periods
        else:
            return periods + 1
    # non-Imm
    elif imm == 1:
        return periods
    else:
        return 0

def get_loan_period(sd: date, vd: date, ed: date, freq: int, gen: int) -> int:
    """
    Calculate the number of loan periods.

    Inputs:
        sd (date): start date of a loan
        vd (date): value date
        ed (date): maturity date
        freq (int): the frequency of payments in months
        gen (int): the method for generating the schedule, 0 for backward from 
                   the end date, and 1 for forward from the start date
    """
    # Backward from end date until pass vd
    if gen == 0:
        n = 1
        d = ed
        while days_diff(vd, sub_months(d, freq)) > 0:
            d = sub_months(d, freq)
            n += 1
        return n
    
    # Forward from start date until vd, then to ed
    elif gen == 1:
        n = 1
        d = sd
        while days_diff(d, vd) > 0:
            d = add_months(d, freq)
        while days_diff(d, ed) > 0:
            d = add_months(d, freq)
            n += 1
        return n
    
    else:
        return 0

def set_loan_calendar(sd: date, vd: date, ed: date, freq: int, dcc: int, 
    bda: int, gen: int, n: int) -> tuple[list[date], list[float], list[float]]:
    """
    Generate a full payment schedule for a loan.

    Inputs:
        sd (date): start date
        vd (date): value date 
        ed (date): maturity date
        freq (int): payment frequency in months
        ddc (int): day count convention code
        bda (int): business day adjustment convention
        gen (int): generation method
        n (int): number of periods, calculated from get_loan_period
    
    Returns (tuple[list, list, list]):
        dates (list[date]): the schedule of payment dates
        cumul (list[float]): the cumulative time from vd to each payment date
        interv (lift;float): the time intervals between each consecutive pair
                             of dates
    """  
    dates = [None] * (n + 1)
    interv = [0.0] * n
    cumul = [0.0] * (n + 1)
    cumul[0] = 0.0

    # Backward schedule
    if gen == 0:
        dates[0] = business_day_adjust(sub_months(ed, n * freq), bda)
        for i in range(1, n + 1):
            dates[i] = business_day_adjust(sub_months(ed, (n - i) * freq), bda)

    # Forward schedule
    elif gen == 1:
        dates[0] = vd
        # Find the first payment date after the value date
        d = sd
        while d <= vd:
            d = add_months(d, freq)
        for i in range(1, n):
            dates[i] = business_day_adjust(d, bda)
            d = add_months(d, freq)
        dates[n] = ed
    
    else:
        return ValueError(f"Unknown schedule generation code input: {gen}")

    for i in range(n):
        interv[i] = day_count(dates[i], dates[i + 1], dcc)
        cumul[i + 1] = cumul[i] + interv[i]
    
    return dates, interv, cumul

def set_default_intensities_calendar(vd: date, freq: int, dcc: int, bda: int, 
    imm: int, spread: int) -> tuple[list[date], list[float], list[float]]:
    """
    Generate a schedule of dates for default intensity

    Returns (tuple[list, list, list]):
        dates (list[date]) schedule of default intensity dates
        cumul (list[float]): cumulative time from vd to each curve date
        interv (list[float]): time interval between each consecutive pair of dates.
    """
    n = get_intensities_period(vd, freq, imm, spread)
    dates = [None] * (n + 1)
    interv = [0.0] * n
    cumul = [0.0] * (n + 1)
    dates[0] = vd

    # IMM
    if imm == 0:
        first = first_imm_date(vd, freq)
        dates[1] = business_day_adjust(first, bda)
        for i in range(2, n + 1):
            d = add_months(first, (i - 1) * freq)
            dates[i] = business_day_adjust(d, bda)
    
    # Non-Imm
    elif imm == 1:
        for i in range(1, n + 1):
            dates[i] = business_day_adjust(add_months(vd, i * freq), bda)

    else:
        raise ValueError(f"Unknown imm code: {imm}")

    cumul[0] = 0.0
    for i in range(n):
        interv[i] = day_count(dates[i], dates[i + 1], dcc)
        cumul[i + 1] = cumul[i] + interv[i]
    
    return dates, interv, cumul

def date_to_term(d1: date, d2: date, dcc: int) -> float:
    return day_count(d1, d2, dcc)

