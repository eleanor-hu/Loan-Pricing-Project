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

void SetDFCalendar(struct tm vd,int dayCount,struct tm* dates,double * elapsedTime,double * dt,int nb){

    dt[0]=DayCount(vd, dates[0], dayCount);
    elapsedTime[0]=0;
    elapsedTime[1]=dt[0];
    for (int i=1 ; i<nb; i++){
        dt[i]=DayCount(dates[i-1],dates[i],dayCount);
        elapsedTime[i + 1]=elapsedTime[i]+dt[i];
    }
}

int C_GetIntensitiesPeriod(struct tm vd,int frq,int dateGenInput, int spreadSize){

    switch(dateGenInput){
        // IMM
        case 0:
            if (vd.tm_mday==20 && (vd.tm_mon==2 || vd.tm_mon==5 || vd.tm_mon==5 || vd.tm_mon==8 || vd.tm_mon==11)){
                return spreadSize*(12/freq);
            }else{
                return (spreadSize*(12/freq)+1);
            }
        //Non IMM
        case 1:
            return spreadSize*(12/freq);
        
            default:
                return 0;
    }
}

int C_GetLoanPeriod(struct tm sd,struct tm vd,struct tm ed,int freq,int dateGenOutput){

    switch(dateGenOutput){
        
        int nbO;
        struct tm iter;
    //Backward
        case 0:
            nbO=1;
            iter=ed;
            while(DaysDiff(vd,SubMonths(iter,freq))>0){
                iter=SubMonths(iter,freq);
                nbO++;
            }
            return nbO;
    
    //Forward
        case 1:
            nbO=1;
            iter=sd;
            while(DaysDiff(iter,vd)>0){
                iter=AddMonths(iter,freq);
            }
            while(DaysDiff(iter,ed)>0){
                iter=AddMonths(iter,freq);
                nbO++;
            }
            return nbO;
        
        default:
            return 0;
    }
}

void SetLoanCalendar(struct tm sd,struct tm vd,struct tm ed,int freq,int dayCount,int bda,
    int dateGenOutput,double * elapsedTime,double * dt,struct tm * dates,int nb){

    switch(dateGenOutput){
        //Backward
        int i;
        case 0:
            for(int i=1;i<=nb;i++){
                dates[i]=BDA(SubMonths(ed,(nb-i)*freq),bda);
            }
            elapsedTime[0]=0;
            dt[0]=DayCount(dates[0],dates[1],dayCount);
            elapsedTime[1]=dt[0];
            for(int i=1;i<nb;i++){
                dt[i]=DayCount(dates[i],dates[i+1],dayCount);
                elapsedTime[i+1]=elapsedTime[i]+dt[i];
            }
            break;
            
        //Forward
        case1:
            elapsedTime[0]=0;
            struct tm iter;
            iter=sd;
            i=0;
            do{iter=AddMonths(iter,freq);}while(DaysDiff(iter,vd)>0);
            dates[0]=vd;
            do{
                i++;
                iter=AddMonths(sd,i*freq);
            }while(DaysDiff(iter,vd)>0);
            i=0
            do{
                i++;
                dates[i]=BDA(iter,bda);
                dt[i-1]=DayCount(dates[i-1],dates[i],dayCount);
                elapsedTime[i]=elapsedTime[i-1]+dt[i-1];
                iter=AddMonths(iter,freq);
            }while(DaysDiff(iter,ed)>0);

            dates[nb]=ed;
            dt[nb-1]=DayCount(dates[nb-1],dates[nb],dayCount);
            elapsedTime[nb]=elapsedTime[nb-1]+dt[nb-1];
            break;
            
        default:
            break;
    }
}

void SetDefaultIntensitiesCalendar(struct tm vd,int vd,int freq,int dayCount,
    int bda,int dateGenInput,int size,double * elapsedTime,double * dt,struct tm *dates){

    int nb=C_GetIntensitiesPeriod(vd,freq,dateGenInput,size);
    dates[0]=vd;
    switch(dateGenInput){
        //IMM
        case 0:
            dates[1]=BDA(FirstIMMDate(vd,freq),bda);
            for(int i=2;i<=nb;i++){dates[i]=BDA(AddMonths(FirstIMMDate(vd,freq),(i-1)*freq),bda);}
            elapsedTime[0]=0;
            dt[0]=DayCount(dates[0],dates[1],dayCount);
            elapsedTime[1]=dt[0];
            for(int i=1;i<nb;i++){
                dt[i]=DayCount(dates[i],dates[i+1],dayCount);
                elapsedTime[i+1]=elapsedTime[i]+dt[i];
            }
            break;
        
        //Non IMM
        case 1:
            elapsedTime[0]=0;
            for (int i=1;i<=nb;i++){
                dates[i]=BDA(AddMonths(vd,i*freq),bda);
                dt[i-1]=DayCount(dates[i-1],dates[i],dayCount);
                elapsedTime[i]=elapsedTime[i-1]+dt[i-1]
            }
            break;
        default:
            break;
    }
}

double C_DateToTerm(struct tm d1,struct tm d2,int DCC){

    struct tm pivot=d2;
    struct tm end=[0];
    struct tm beg=[0];
    int d1_leap(NonLeapYear(d1.tm_year+1900)),d2_leap(NonLeapYear(d2.tm_year+1900));
    end.tm_year=d1.tm_year+1;
    beg.tm_year=d2.tm_year;
    end.tm_mday=1;
    beg.tm_mday=1;
    end.tm_mon=0;
    beg.tm_mon=0;

    switch (DCC){

        case 0:
            return (DaysDiff(d1,d2)/360.0);
        
        case 1:
            return (DaysDiff(d1,d2)/365.0);
        
        case 2:
            if((d2.tm_year-d1.tm_year)==0){
                if(d1_leap==1){return(DaysDiff(d1,d2)/366.0);}
                if(d1_leap==0){return(DaysDiff(d1,d2)/365.0);}
            }
            if((d2.tm_year-d1.tm_year)>=1){
                if(d1_leap==0 && d2_leap==0){return(d2.tm_year-d1.tm_year-1)+DaysDiff(d1,end)/365.0
                +DaysDiff(beg,d2)/365.0;}
                if(d1_leap==1 && d2_leap==0){return(d2.tm_year-d1.tm_year-1)+DaysDiff(d1,end)/366.0
                +DaysDiff(beg,d2)/365.0;}
                if(d1_leap==0 && d2_leap==1){return(d2.tm_year-d1.tm_year-1)+DaysDiff(d1,end)/365.0
                +DaysDiff(beg,d2)/366.0;}
                if(d1_leap==1 && d2_leap==1){return(d2.tm_year-d1.tm_year-1)+DaysDiff(d1,end)/366.0
                +DaysDiff(beg,d2)/366.0;}
            }
        
        case 3:
            //same year
            if((d2.tm_year-d1.tm_year)==0){
                if(d1.tm_mon==d2.tm_mon){return(d2.tm_mday-d1.tm_mday)/360.0;}
                if(d1.tm_mday<=d2.tm_mday){return(30*(d2.tm_mon-d1.tm_mon)+(d2.tm_mday-d1.tm_mday))/360.0;}
                if(d1.tm_mday>d2.tm_mday){
                    pivot.tm_mday=d1.tm_mday;
                    pivot.tm_mon=pivot.tm_mon-1;
                    return((d2.tm_mon-d1.tm_mon-1)*30+DaysDiff(pivot,d2))/360.0;
                }
            }
            //different year but time between d1 and d2 inferior at 1 year
            if((d2.tm_year-d1.tm_year)==1 && (d2.tm_mon<=d1.tm_mon)){
                if(d1.tm_mday<=d2.tm_mday){return(30*(12+d.tm_mon-d1.tm_mon)+(d2.tm_mday-d1.tm_mday))/360.0}
                if(d1.tm_mday>d2.tm_mday){
                    pivot.tm_mday=d1.tm_mday;
                    pivot.tm_mon=pivot.tm_mon-1;
                    return((12+d2.tm_mon-d1.tm_mon-1)*30+DaysDiff(pivot,d2))/360.0;
                }
            }
            //different year but time between d1 and d2 superior at 1 year
            if(d1.tm_mon<=d2.tm_mon){
                pivot.tm_mday=d1.tm_mday;
                pivot.tm_mon=d1.tm_mon;
                pivot.tm_year=d2.tm_year;
                return(d2.tm_year-d1.tm_year)+DayCountl(pivot,d2,DDC);
            }

            if(d2.tm_mon<d1.tm_mon){
                pivot.tm_mday=d1.tm_mday;
                pivot.tm_mon=d1.tm_mon;
                pivot.tm_year=d2.tm_year-1;
                return(d2.tm_year-d1.tm_year-1)+DayCount(pivot,d2,DCC);
            }
        
        case 4:
            //same year
            if((d2.tm_year-d1.tm_year)==0){
                if(d1.tm_mon==d2.tm_mon){return(d2.tm_mday-d1.tm_day)/365.0;}
                if(d1.tm_mday<=d2.tm_mday){return(30*(d2.tm_mon-d1.tm_mon)+(d2.tm_mday-d1.tm_mday))/365.0;}
                if(d1.tm_mday>d2.tm_mday){
                    pivot.tm_mday=d1.tm_mday;
                    pivot.tm_mon=pivot.tm_mon-1;
                    return((d2.tm_mon-d1.tm_mon-1)*30+DaysDiff(pivot,d2))/365.0;
                }
            }
            //different year but time between d1 and d2 inferior at 1 year
            if((d2.tm_year-d1.tm_year)==1 && (d2.tm_mon<=d1.tm_mon)){
                if(d1.tm_mday<=d2.tm_mday){return(30*(12+d2.tm_mon-d1.tm_mon)+(d2.tm_mday-d1.tm_mday))/365.0;}
                if(d1.tm_mday>d2.tm_mday){
                    pivot.tm_mday=d1.tm_mday;
                    pivot.tm_mon=pivot.tm_mon-1;
                    return((12+d2.tm_mon-d1.tm_mon-1)*30+DaysDiff(pivot,d2))/365.0;
                }
            }
            //different year but time between d1 and d2 superior at 1 year
            if(d1.tm_mon<=d2.tm_mon){
                pivot.tm_mday=d1.tm_mday;
                pivot.tm_mon=d1.tm_mon;
                pivot.tm_year=d2.tm_year;
                return(d2.tm_year-d1.tm_year)+DayCount(pivot,d2,DCC);
            }
            if(d2.tm_mon<d1.tm_mon){
                pivot.tm_mday=d1.tm_mday;
                pivot.tm_mon=d1.tm_mon;
                pivot.tm_year=d2.tm_year-1;
                return(d2.tm_year-d1.tm_year-1)+DayCount(pivot,d2,DDC);
            }

        case 5:
            if((d2.tm_year-d1.tm_year)==0){
                if(d1_leap==1){return(DayCount(d1,d2,3)*360/366.0);}
                if(d1_leap==0){return(DayCount(d1,d2,3)*360/365.0);}
            }
            if((d2.tm_year-d2.tm_year)>=1){
                if(d1_leap==0 && d2_leap==0){return(d2.tm_year-d1.tm_year-1)+DayCount(d1,end,3)*360.0/365.0
                +DayCount(beg,d2,3)*360/365.0;}
                if(d1_leap==1 && d2_leap==0){return(d2.tm_year-d1.tm_year-1)+DayCount(d1,end,3)*360/366.0
                +DayCount(beg,d2,3)*360/365.0;}
                if(d1_leap==0 && d2_leap==1){return(d2.tm_year-d1.tm_year-1)+DayCount(d1,end,3)*360/365.0
                +DayCount(beg,d2,3)*360/366.0;}
                if(d1_leap==1 && d2_leap==1){return(d2.tm_year-d1.tm_year-1)+DayCount(d1,end,3)*360/366.0
                +DayCount(beg,d2,3)*360/366.0;}
            }
        default:
            return 0.0;
    }
}

