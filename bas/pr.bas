10 rem tabpr
20 let n=2
30 let nmax=8192*8192
40 if n>nmax then 90
50 goto 1000
60 print n
70 let n=n+1
80 goto 40
90 end
1000 let k=2
1010 if k>n/k then 60
1020 if n>n/k*k then 1040
1030 goto 70
1040 let k=k+1
1050 goto 1010
1060 end
2000 let h=#n
2010 if h=1 then 60
2020 goto 70
