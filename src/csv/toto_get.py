#!/usr/local/bin/python3
#coding:utf-8

import sys,string,re,unicodedata,csv,urllib.request
from bs4 import BeautifulSoup,NavigableString,Declaration,Comment
from progressbar import ProgressBar

min_value=198
max_value=898

url_='http://www.toto-dream.com/dci/I/IPB/IPB01.do?op=lnkHoldCntLotResultLsttoto&holdCntId=0'

def OutputToFile(rows,url_num):
    csvFile=open(str(url_num)+".csv","wt",newline="",encoding="utf-8")
    writer=csv.writer(csvFile)

    try:
        for tab in rows:
            csvRow=[]
            for cell in tab.findAll(["td","td"]):
                csvRow.append(str(cell.get_text()).strip())
            writer.writerow(csvRow)
    finally:
        csvFile.close()

def main_loop(url_num):
    url=url_+str(url_num)
    soup=BeautifulSoup(urllib.request.urlopen(url),"lxml")

    check=soup.findAll("table",{"class","adjustment"})
    checker=True
    if check is not None and len(check) > 0:
        for div in check:
            if div.text.find("totoGOAL") == -1:
                checker=False
    
    if not(checker) :
        x=soup.findAll("table",{"class","kobetsu-format2"})
        if x is not None and len(x) > 0:
            table=x[0]
            rows=table.findAll("tr")
            OutputToFile(rows,url_num)

def main():
    counting=max_value-min_value
    p=ProgressBar(0,counting)
    counter=0

    for var in range(min_value,max_value):
        main_loop(var)
        counter+=1
        p.update(counter)

main()
