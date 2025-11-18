10 rem Goldbach
20 let n=2*1024*1024
30 let nmax=4*1024*1024
40 if n>nmax then 80
50 goto 1000
60 let s=16*1024
61 if n>n/s*s then 65
62 print "ok ";n/1024;"k"
65 let n=n+2
70 goto 40
80 print "ok ";n-2
90 end
900 print "Counterexample: ";n
910 end
1000 let p=3
1010 if p>n-p then 900
1020 let r=p
1030 goto 2000
1035 if pr=0 then 1080
1040 let r=n-r
1050 if r=p then 60
1060 goto 1030
1070 goto 60
1080 let p=p+2
1090 goto 1010
2000 let k=3
2010 if k<=r/k then 2040
2020 let pr=1
2030 goto 1035
2040 if r=r/k*k then 2070
2050 let k=k+2
2060 goto 2010
2070 let pr=0
2080 goto 1035
