import datetime
from datetime import date, timedelta
import calendar
import math
import Time
from Time import (
    excel_serial_to_date,
    date_to_excel_serial,
    day_count,
    add_months,
    sub_months,
    days_diff,
    business_day_adjust,
    first_imm_date
)
import SetCalendar_engine
from SetCalendar_engine import (
    set_df_calendar,
    get_intensities_period,
    get_loan_period,
    set_loan_calendar,
    set_default_intensities_calendar,
    date_to_term
)
from typing import List, Tuple
import xlwings as xw

def get_intensities_calendar_output(vd, freq, dcc, bda, spread, imm, choice):
    """
    High-level interface to generate different outputs for a
    default intensities calendar based on the 'choice' parameter

    Input:
        choice (int): determines the output format:
        - 0: return the list of dates
        - 1: return the list of time intervals
        - 2: return the list of cumulative time
    """
    dates, interv, cumul = set_default_intensities_calendar(
        vd, freq, dcc, bda, imm, spread
    )
    
    if choice == 0:
        return dates
    elif choice == 1:
        return interv
    elif choice == 2:
        return cumul
    else:
        return []

@xw.func
@xw.ret(expand='down')
def intensities_dates(vd_serial, freq, dcc, bda, imm, spread):
    """
    Return the schedule of dates as Excel serials

    Inputs:
        vd_serial (int): the Excel serial of value date
    """
    vd = excel_serial_to_date(int(vd_serial))
    dates = get_intensities_calendar_output(
        vd, int(freq), int(dcc), int(bda), int(spread), int(imm), choice=0
    )
    return [date_to_excel_serial(d) for d in dates]

@xw.func
@xw.ret(expand='down')
def intensities_interval(vd_serial, freq, dcc, bda, imm, spread):
    """
    Return the day count between each period
    """
    vd = excel_serial_to_date(int(vd_serial))
    interv = get_intensities_calendar_output(
        vd, int(freq), int(dcc), int(bda), int(spread), int(imm), choice=1
    )
    return interv

@xw.func
@xw.ret(expand='down')
def intensities_cumul(vd_serial, freq, dcc, bda, imm, spread):
    """
    Return the cumulative day count for each period
    """
    vd = excel_serial_to_date(int(vd_serial))
    cumul = get_intensities_calendar_output(
        vd, int(freq), int(dcc), int(bda), int(spread), int(imm), choice=2
    )
    return cumul

def get_cashflow_calendar_output(sd, vd, ed, freq, dcc, bda, gen, choice):
    """
    High-level interface to generate different outputs for a
    loan cash flow calendar based on the 'choice' parameter

    Inputs:
        choice (int): Determine the output format:
            - 0: Return the list of dates
            - 1: Return the list of time intervals
            - 2: Return the list of cumulative time
    """
    n = get_loan_period(sd, vd, ed, int(freq), int(gen))
    dates, interv, cumul = set_loan_calendar(
        sd, vd, ed, int(freq), int(dcc), int(bda), int(gen), n
    )
    if choice == 0:
        return dates
    elif choice == 1:
        return interv
    elif choice == 2:
        return cumul
    else:
        return []

@xw.func
@xw.ret(expand='down')
def cashflow_dates(sd_serial, vd_serial, ed_serial, freq, dcc, bda, gen):
    """
    return the loan payment dates as excel serial
    """
    sd = excel_serial_to_date(sd_serial)
    vd = excel_serial_to_date(vd_serial)
    ed = excel_serial_to_date(ed_serial)
    dates = get_cashflow_calendar_output(
        sd, vd, ed, int(freq), int(dcc), int(bda), int(gen), choice=0
    )
    return [date_to_excel_serial(d) for d in dates]

@xw.func
@xw.ret(expand='down')
def cashflow_interv(sd_serial, vd_serial, ed_serial, freq, dcc, bda, gen):
    """
    Return the period day count for the loan schedule
    """
    sd = excel_serial_to_date(sd_serial)
    vd = excel_serial_to_date(vd_serial)
    ed = excel_serial_to_date(ed_serial)
    interv = get_cashflow_calendar_output(
        sd, vd, ed, int(freq), int(dcc), int(bda), int(gen), choice=1
    )
    return interv

@xw.func
@xw.ret(expand='down')
def cashflow_cumul(sd_serial, vd_serial, ed_serial, freq, dcc, bda, gen):
    """
    Return the cumulative day count for the loan schedule
    """
    sd = excel_serial_to_date(sd_serial)
    vd = excel_serial_to_date(vd_serial)
    ed = excel_serial_to_date(ed_serial)
    cumul = get_cashflow_calendar_output(
        sd, vd, ed, int(freq), int(dcc), int(bda), int(gen), choice=2
    )
    return cumul

@xw.func
def xls_date_to_term(start_serial, end_serial, dcc):
    """
    Return the day count between two dates using DCC
    """
    d1 = excel_serial_to_date(start_serial)
    d2 = excel_serial_to_date(end_serial)
    if d2 < d1:
        d1, d2 = d2, d1
    return date_to_term(d1, d2, int(dcc))

@xw.func
def term_to_date(start_serial, target_term, dcc):
    """
    Calculates the end date given a start date and a term in years

    Inputs:
        start_serial (int): the Excel serial of start date
        target_term (float): the target term in years
    
    Returns (int): the calculated end date as Excel serial number
    """
    sd = excel_serial_to_date(start_serial)
    # initial guess by adding full years
    years = int(target_term)
    ed = sd.replace(year=sd.year + years)
    term = date_to_term(sd, ed, int(dcc))

    # refine by months then days
    while term < target_term:
        ed = add_months(ed, 1)
        term = date_to_term(sd, ed, int(dcc))
    
    while term > target_term:
        ed -= timedelta(days=1)
        term = date_to_term(sd, ed, int(dcc))
    
    while term < target_term:
        ed += timedelta(days=1)
        term = date_to_term(sd, ed, int(dcc))
    
    return date_to_excel_serial(ed)

@xw.func(expand='down')
def generate_tenor_cashflows(sd_serial, vd_serial, ed_serial, freq, dcc, 
    bda, gen, target_tenor, nominal):
    """
    Generate a custom amortization schedule (outstanding principal) to 
    match a target effective tenor.

    Returns (list[float]):
        the generated cash flow schedule (outstanding principal)
    """
    sd = excel_serial_to_date(sd_serial)
    vd = excel_serial_to_date(vd_serial)
    ed = excel_serial_to_date(ed_serial)

    # time to cashflow
    n = get_loan_period(sd, vd, ed, int(freq), int(gen))
    _, _, t_cf = set_loan_calendar(sd, vd, ed, int(freq), int(dcc), int(bda), int(gen), n)

    cash_flows = [0.0] * (n + 1)
    cash_flows[0] = nominal
    cash_flows[-1] = 0.0

    xl, xr = 0.0, float(n) * 2
    lam = 4.3671 * pow(n, -0.913)

    for _ in range(100):
        xm = (xl + xr) / 2
        for j in range(1, n):
            cash_flows[j] = cash_flows[0] / (1 + math.exp(lam * (j - xm)))
            
        #calculated effective tenor
        cumul = 0.0
        for i in range(n):
            repay = cash_flows[i] - cash_flows[i+1]
            cumul += repay * t_cf[i+1]
            
        if cash_flows[0] == 0:
            eff = 0.0
        else:
            eff = cumul / cash_flows[0]

        err = eff - target_tenor
        if abs(err) < 0.0001:
            break
        
        if err < 0:
            xl = xm
        else:
            xr = xm

    return cash_flows

