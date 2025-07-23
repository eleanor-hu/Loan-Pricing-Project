# include "stdafx.h"
# include <math.h>
# include <cmath>
# include <cstdlib>
# include <string.h>
# include <oaidl.h>

# include "C_Time.h"
# include "C_SetCalendar.h"
# include "C_Probabilities.h"
# include "C_Priing.h"
# include "C_Interpolation.h"

int _stdcall IntensitiesCalendar(double valueDate,double freq,double dayCount,double bda,
double dateGen,double choice, LPSAFEARRAY FAR* spreads,double * output){
    
    int _freq = static_cast<int>(freq);
    int _dayCount = static_cast<int>(dayCount);
    int _bda = static_cast<int>(bda);
    int _dateGen = static_cast<int>(dateGen);
    int _choice = static_cast<int>(choice);

    struct tm vd = DoubleToStructTM(valueDate);

    int _size = (*spreads) -> rgsabound[0].cElements;       //number of spreads
    int nb=C_GetIntensitiesPeriod(vd, _freq, _dateGen, _size);

    double* dt = new double[nb];
    double* elapsedTime = new double[nb+1];
    struct tm* dates = new struct tm[nb+1];

    int i=0;

    switch(_dateGen){

        //IMM
        case 0:

            switch(_choice){

                case 0:
                    output[0]=StructTMToDouble(vd);
                    output[1]=StructTMToDouble(BDA(FirstIMMDate(vd,_freq),_bda));
                    for(int i=2; i<=nb; i++){
                        output[i]=StructTMToDouble(BDA(AddMonths(FirstIMMDate(vd,freq),(i-1)*_freq),_bda));
                    }
                    delete []dt;
                    delete []elapsedTime;
                    delete []dates;
                    return 1;
                
                case 1:
                    dates[0]=vd;
                    dates[1]=BDA(FirstIMMDate(vd,freq),bda);
                    for(int i=2;i<=nb;i++){dates[i]=BDA(AddMonths(FirstIMMDate(vd,_freq),(i-1)*_freq),_bda);}
                    output[0]=DayCount(dates[0],dates[1],_DayCount);
                    for(int i=1;i<nb;i++){output[i]=DayCount(dates[i],dates[i+1],_dayCount);}
                    delete []dt;
                    delete []elapsedTime;
                    delete []dates;
                    return 1;
                
                case 2:
                    dates[0]=vd;
                    dates[1]=BDA(FirstIMMDate(vd,_freq),_bda);
                    for(int i=2;i<=nb;i++){dates[i]=BDA(AddMonths(FirstIMMDate(vd,_freq),(i-1)*_freq),_bda);}
                    output[0]=0;
                    dt[0]=DayCount(dates[0],dates[1],_dayCount);
                    output[1]=dt[0];
                    for(int i=1;i<nb;i+1){
                        dt[i]=DayCount(dates[i],dates[i+1],_dayCount);
                        output[i+1]=output[i]+dt[i];
                    }
                    delete []dt;
                    delete []elapsedTime;
                    delete []dates;
                    return 1;
                
                default:
                    delete []dt;
                    delete []elapsedTime;
                    delete []dates;
                    return 0;
            }
        
        //Non IMM
        case 1:
            switch(_choice){
                case 0:
                    output[0]=StructTMToDouble(vd);
                    for (int i=1;i<=nb;i++){output[i]=StructTMToDouble(BDA(AddMonths(vd,i*_freq),_bda));}
                    delete []dt;
                    delete []elapseTime;
                    delete []dates;
                    return 1;
                
                case 1:
                    dates[0]=vd;
                    for (int i=1;i<=nb;i+1){
                        dates[i]=BDA(Addmonths(vd,i*_freq),_bda);
                        output[i-1]=DayCount(dates[i-1],dates[i],_dayCount);
                    }
                    delete []dt;
                    delete []elapsedTime;
                    delete []dates;

                    return 1;
                    
                case 2:
                    dates[0]=vd;
                    output[0]=0;
                    for (int i=1;i<=nb;i++){
                        dates[i]=BDA(AddMonths(vd,i*_freq),_bda);
                        dt[i-1]=DayCount(dates[i-1],dates[i],_dayCount);
                        output[i]=output[i-1]+dt[i-1];
                    }
                    delete []dt;
                    delete []elapsedTime;
                    delete []dates;
                    return 1;

                default:
                    delete []dt;
                    delete []elapsedTime;
                    delete []dates;
                    return 0;
            }
        
            default:
                delete []dt;
                delete []elapsedTime;
                delete []dates;
                return 0;
    }
}

int _stdcall CashFlowCalendar(double startDate,double valueDate,double endDate,double freq,
double dayCount,double dba,double dateGenOutput,double choice,double * output){
    
    int _freq=static_cast<int>(freq);
    int _dayCount=statis_cast<int>(dayCount);
    int _dba=static_cast<int>(bda);
    int _dateGenOutput=static_cast<int>(dateGenOutput);
    int _choice=static_cast<int>(choice);

    struct tm sd=DoubleToStructTM(startDate);
    struct tm vd=DoubleToStructTM(valuDate);
    struct tm ed=DoubleToStructTM(endDate);

    int nbO=C_GetLoanPeriod(sd, vd, ed, _freq, _dateGenOutput);

    double* dtCF =new double[nbO];
    double* tCF = new double[nbO+1];
    struct tm* dateCF = new struct tm [nbO+1];

    struct tm iter;
    int i=0;

    switch(_dateGenOutput){
        case 0:
            switch(_choice){
                case 0:

                    datesCF[0]=vd;
                    for(i=1;i<=nbO;i++){datesCF[i]=BDA(SubMonths(ed,(nbO-i)*_freq),_bda);}
                    for(i=0;i<=nbO;i++){output[i]=StructTMToDouble(datesCF[i]);}
                    delete []dtCF;
                    delete []tCF;
                    delete []datesCF;
                    return 1;

                case 1:
                    datesCF[0]=vd;
                    for(i=1;i<=nbO;i++){datesCF[i]=BDA(SubMonths(ed,(nbO-i)*_freq),_bda);}
                    for(i=o;i<nbO;i++){output[i]=DayCount(datesCF[i],datesCF[i+1],_dayCount);}
                    delete []dtCF;
                    delete []tCF;
                    delete []datesCF;
                    return 1;
                
                case 2:
                    datesCF[0]=vd;
                    for(i=1;i<=nbO;i++){datesCF[i]=BDA(SubMonths(ed,(nbO-i)*_freq),_bda);}
                    dtCF[0]=DayCount(datesCF[0],datesCF[1],_dayCount);
                    output[0]=0;
                    output[1]=dtCF[0];
                    for(i=1;i<nbO;i++){
                        dtCF[i]=DayCount(datesCF[i],datesCF[i+1],_dayCount);
                        output[i+1]=output[i]+dtCF[i];
                    }
                    delete []dtCF;
                    delete []tCF;
                    delete []datesCF;
                    return 0;
            }
        case 1:
            switch(_choice){

                case 0:
                    i=0;
                    iter=sd;
                    do{iter=AddMonths(iter,_freq);}while(DaysDiff(iter,vd)>0);
                    datesCF[0]=vd;
                    do{
                        i++;
                        datesCF[i]=BDA(iter,_bda);
                        iter=AddMonths(iter,_freq);
                    }while(DaysDiff(iter,en)>=0);
                    datesCF[nbO]=ed;
                    for(i=0;i<=nbO;i++){output[i]=StructTMToDouble(datesCF[i]);}
                    delete []dtCF;
                    delete []tCF;
                    delete []datesCF;
                    return 1;
                
                case 1:
                    i=0;
                    iter=sd;
                    do{iter=AddMonths(iter,_freq);}while(DaysDiff(iter,vd)>0);
                    datesCF[0]=vd;
                    do{
                        i++;
                        datesCF[i]=BDA(iter,_nba);
                        output[i-1]=DayCount(datesCF[i-1],datesCF[i],_dayCount);
                        iter=AddMonths(iter,_freq);
                    }while(DaysDiff(iter,en)>0);
                    datesCF[nbO]=ed;
                    output[nbO-1]=DayCount(datesCF[nbO-1],datesCF[nbO],_dayCount);
                    delete []dtCF;
                    delete []tCF;
                    delete []datesCF;
                    return 1;

                case 2:
                    i=0;
                    output[0]=0;
                    iter=sd;
                    do{iter=AddMonths(iter,_freq);}while(DaysDiff(iter,vd)>0);
                    datesCF[0]=vd;
                    do{
                        i++;
                        datesCF[i]=BDA(iter,_bda);
                        dtCF[i-1]=DayCount(datesCF[i-1],datesCF[i],_dayCount);
                        output[i]=output[i-1]+dtCF[i-1];
                        iter=AddMonths(iter,_freq);
                    }while(DaysDiff(iter,ed)>0);
                    datesCF[nbO]=ed;
                    dtCF[nbO-1]=DayCount(datesCF[nbO-1],datesCF[nbO],_dayCount);
                    output[nbO]=output[nbO-1]+dtCF[nbO-1];
                    delete []dtCF;
                    delete []tCF;
                    delete []datesCF;
                    
                    default:
                        delete []dtCF;
                        delete []tCF;
                        delete []datesCF;
                        return 0;
            }
        default:
            delete []dtCF;
            delete []tCF;
            delete []datesCF;
            return 0;
    }
}

double _stdcall GetLoanPeriod(double startDate, double valueDate,double endDate,double freq,double dateGenOutput){
    int _freq = static_cast<int>(freq);
    int _dateGenOutput=static_cast<int>(dateGenOutput);

    struct tm sd=DoubleToStructTM(startDate);
    struct tm vd=DoubleToStructTM(valueDate);
    struct tm en=DoubleToStructTM(endDate);

        switch(_dateGenOutput){
            int nbO;
            struct tm iter;
        //Backward
            case 0:
                nbO=2;
                iter=ed;
                while(DaysDiff(vd,SubMonths(iter,_freq))>0){
                    iter=Submonths(iter,_freq);
                    nbO++;
                }
                return nbO;
        
        //Forward
            case 1:
                nbO=2;
                iter=sd;
                while(DaysDiff(iter,vd)>=0){
                    iter=AddMonths(iter,_freq);
                }
                while(DaysDiff(iter,ed)>0){
                    iter=AddMonths(iter,_freq);
                    nbO++
                }
                return nbO;
            
            default:
                return 0;
        }

}

double _stdcall EffectiveTenor(double startDate,double valueDate,double endDate,
    double outputFreq,double dayCountOut,double bdaOut,double dateGenOutput,LPSAFEARRAY FAR*CF){
    
    int _freq = static_cast<int>(outputFreq);
    int _dayCount = static_cast<int>(dayCountOut);
    int _bda = static_cast<int>(bdaOut);
    int _dateGenOutput = static_cast<int>(dateGenOutput);

    struct tm sd=DoubleToStructTM(startDate);
    struct tm vd=DoubleToStructTM(valueDate);
    struct tm en=DoubleToStructTM(endDate);

    int i;
    double cumul(0.0);
    int nbO=C_GetLoanPeriod(sd,vd,ed,_freq,_dateGenOutput);

    double* dtCF=new double[bO];
    double* tCF=new double[nbO+1];
    struct tm*datesCF=new struct tm[nbO+1];

    double* _CF = (double*)(*CF)->pvData;

    datesCF[0]=vd;
    datesCF[1]=ed;
    SetLoanCalendar(sd,vd,ed,_freq,_dayCount,_bda,_dateGenOutput,tCF,dtCF,datesCF,nbO);

    _CF[nbO]=0;
    for(i=0;i<nb;i++){
        cumul=cumul+(_CF[i]-_CF[i+1])*tCF[i+1]/_CF[0];
    }

    delete []dtCF;
    delete []tCF;
    delete []datesCF;

    return cumul;
}

double _stdcall DateToTerm(double date1,double date2,double datCountConvention){

    int DCC = static_cast<int>(dayCountConvention);

    struct tm d1=DoubleToStructTM(date1);
    struct tm d2=DoubleToStructTM(date2);

    struct tm pivot=d2;
    struct tm end={0};
    struct tm beg={0};
    int d1_leap(NonLeapYear(d1.tm_year+1900)),d2_leap(NonLeapYear(d2.tm_year+1900));
    end.tm_year=d1.tm_year+1;
    beg.tm_year=d2.tm_year;
    end.tm_mday=1;
    beg.tm_mday=1;
    end.tm_mon=0;
    beg.tm_mon=0;

    switch(DCC){
        case 0:
            return (DaysDiff(d1,d2)/360.0);

        case 1:
            return (DaysDiff(d1,d2)/365.0);
        
        case 2:
            if((d2.tm_year-d1.tm_year)==0){
                if(d1_leap==1){return (DaysDiff(d1,d2)/366.0)};
                if(d2_leap==0){return (DaysDiff(d1,d2)/365.0)};
            }
            if((d2.tm_year-d1.tm_year)>=1){
                if(d1_leap==0 && d2_leap==0){
                    return (d2.tm_year-d1.tm_year-1)+DaysDiff(d1,end)/365.0+DaysDiff(beg,d2)/365.0;}
                if(d1_leap==1 && d2_leap==1){
                    return (d2.tm_year-d1.tm_year-1)+DaysDiff(d1,end)/366.0+DaysDiff(beg,d2)/365.0;}
                if(d1_leap==0 && d2_leap==1){
                    return (d2.tm_year-d1.tm_year-1)+DaysDiff(d1,end)/365.0+DaysDiff(beg,d2)/365.0;}
                if(d1_leap==1 && d2_leap==1){
                    return (d2.tm_year-d1.tm_year-1)+DaysDiff(d1,end)/366.0+DaysDiff(beg,d2)/365.0;}
            }
        case 3:
            //same year
            if ((d2.tm_year-d1.tm_year)==0){
                if(d1.tm_mon==d2.tm_mon){return (d2.tm_mday-d1.tm_mday)/360.0;}
                if(d1.tm_mon<=d2.tm_mon){return (30*(d2.tm_mon-d1.tm_mon)+(d2.tm_mday-d1.tm_mday))/360.0;}
                if(d1.tm_mday>d2.tm_mday){
                    pivot.tm_mday=d1.tm_mday;
                    pivot.tm_mon=pivot.tm_mon-1;
                    return ((d2.tm_mon-d1.tm_mon-1)*30+DaysDiff(pivot,d2))/360.0;
                }
            }

            //different year but time between d1 and d2 inferior at 1 year
            if ((d2.tm_year-d1.tm_year)==1 && (d2.tm_mon<=d1.tm_mon)){
                if(d1.tm_mday<=d2.tm_mday){
                    return (30*(12+d2.tm_mon-d1.tm_mon)+(d2.tm_mday-d1.tm_mday))/360.0;}
                if(d1.tm_mday>d2.tm_mday){
                    pivot.tm_mday=d1.tm_mday;
                    pivot.tm_mon=pivot.tm_mon-1;
                    return ((12+d2.tm_mon-d1.tm_mon-1)*30+DaysDiff(pivot,d2))/360.0;}
            }

            //different year but time between d1 and d2 superior at 1 year
            if(d1.tm_mon<=d2.tm_mon){
                pivot.tm_mday=d1.tm_mday;
                pivot.tm_mon=d1.tm_mon;
                pivot.tm_year=d2.tm_year;
                return (d2.tm_year-d1.tm_year)+DayCount(pivot,d2,DCC);
            }

            if(d2.tm_mon<d1.tm_mon){
                pivot.tm_mday=d1.tm_mday;
                pivot.tm_mon=d1.tm_mon;
                pivot.tm_year=d2.tm_year-1;
                return (d2.tm_year-d1.tm_year-1)+DayCount(pivot,d2,DCC);
            }
        
        case 4:
            //same year
            if ((d2.tm_year-d1.tm_year)==0){
                if(d1.tm_mon==d2.tm_mon){return (d2.tm_mday-d1.tm_mday)/365.0;}
                if(d1.tm_mday<=d2.tm_mday){return (30*(d2.tm_mon-d1.tm_mon)+(d2.tm_mday-d1.tm_mday))/365.0;}
                if(d1.tm_mday>d2.tm_mday){
                    pivot.tm_mday=d1.tm_mday;
                    pivot.tm_mon=pivot.tm_mon-1;
                    return ((d2.tm_mon-d1.tm_mon-1)*30+DaysDiff(pivot,d2))/365.0;
                }
            }

            //different year but time between d1 and d2 superior at 1 year
            if(d1.tm_mon<=d2.tm_mon){
                pivot.tm_mday=d1.tm_mday;
                pivot.tm_mon=d1.tm_mon;
                pivot.tm_year=d2.tm_year;
                return (d2.tm_year-d1.tm_year)+DayCount(pivot,d2,DCC);
            }

            if(d2.tm_mon<d1.tm_mon){
                pivot.tm_mday=d1.tm_mday;
                pivot.tm_mon=d1.tm_mon;
                pivot.tm_year=d2.tm_year-1;
                return (d2.tm_year-d1.tm_year-1)+DaysDiff(pivot,d2,DCC);
            }

        case 5:
            if((d2.tm_year-d1.tm_tear)==0){
                if(d1_leap==1){return DayCount(d1,d2,3)*360/366.0;}
                if(d1_leap==0){return DayCount(d1,d2,3)*360/365.0;}
            }
            if((d2.tm_year-d1.tm_year)>=1){
                if(d1_leap==0 && d2_leap==0){
                    return (d2.tm_year-d1.tm_year-1)+
                    DayCount(d1,end,3)*360.0/365.0+
                    DayCount(beg,d2,3)*360/365.0}
                if(d1_leap==1 && d2_leap==0){
                    return (d2.tm_year-d1.tm_year-1)+
                    DayCount(d1,end,3)*360/366.0+
                    DayCount(beg,d2,3)*360/365.0}
                if(d1_leap==0 && d2_leap==1){
                    return (d2.tm_year-d1.tm_year-1)+
                    DayCount(d1,end,3)*360/365.0+
                    DayCount(beg,d2,3)*360/366.0}
                if(d1_leap==1 && d2_leap==1){
                    return (d2.tm_year-d1.tm_year-1)+
                    DayCount(d1,end,3)*360/366.0+
                    DayCount(beg,d2,3)*360/366.0}
            }
        default:
            return 0.0;
    }
}

double _stdcall TermToDate(double datel,double termTarget,double dayCountConvention,double* output){
    
    int DCC = static_cast<int>(dayCountConvention);
    int years = static_cast<int>(ternTarget);

    struct tm vd=DoubleToStructTM(date1);
    struct tm it=vd;

    double term;

    it.tm_year=it.tm_year+years;
    term=C_DateToTerm(vd,it,DCC);

    while(term<termTarget){
        it=AddOneDay(it);
        term=C_DateToTerm(vd,it,DCC);
    }
    output[0]=StructTMToDouble(it);
    output[1]=term;
    return 1;
}

int _stdcall SetCashFlows(double startDate,double valueDate,double,endDate,
    double outputFreq,double dayCountOut,double bdaOut,double dateGenOutput,
    double targetEffectiveTenor, double nominal,double* CashFlows){
    
    int _outputFreq = static_cast<int>(outputFreq);
    int _dayCountOut = static_cast<int>(dayCountOut);
    int _bdaOut = static_cast<int>(bdaOut);
    int _dateGenOutput = static_cast<int>(dateGenOutput);
    
    struct tm sd=DoubleToStructTM(startDate);
    struct tm vd=DoubleToStructTM(valueDate);
    struct tm ed=DoubleToStructTM(endDate);

    int i=0,j;
    double cumul(0),lambda(0),xr,xm,xl,xmTenor;
    int nbO=C_GetLoanPeriod(sd,vd,ed,_outputFreq,_dateGenOutput);

    double* dtCF = new double[nbO];
    double* tCF = new double[nbO+1];
    struct tm* datesCF = new struct tm [nbO+1];

    datesCF[0]=vd;
    datesCF[0]=ed;
    SetLoanCalendar(sd,vd,ed,_outputFreq,_dayCountOut,_bdaOut,_dateGenOutput,tCF,dtCF,datesCF,nbO);

    cashFlows[0]=nominal;
    cashFlows[nbO]=0;
    lambda=4.3671*pow(nbO,-0.913);
    xl=0,xr=2000;

    while(cashFlows[i+1]!=0){i+1}

    do{
        xm=(xr+xl)/2;
        cumul=0;
        for(j=1;j<=i;j++){cumul=cumul+tCF[j]*(cashFlows[j-1]-cashFlows[j]);};
        for(j=i+1;j<nbO;j++){
            cashFlows[j]=cashFlows[i]/(1+exp(lambda*(j-xm)));
            cumul=cumul+tCF[j]*(cashFlows[j-1]-cashFlows[j]);
        }
        cumul=cumul+tCF[nbO]*(cashFlows[nbO-1]-cashFlows[nbO]);
        xmTenor=cumul/nominal-targetEffectiveTenor;
        if(xmTenor<0){
            xl=xm;
        }else{
            xr=xm;
        }
    }while(abs(xmTenor)>0.0001);

    delete []dtCF;
    delete []tCF;
    delete []datesCF;

    return 1;
}
