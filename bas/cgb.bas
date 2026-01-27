10 rem Goldbach
15 let mi=1024*1024
20 let n=1536*mi
30 let nmax=1792*mi
40 if n>nmax then 80
50 goto 1000
60 print n;"=";p;"+";n-p
65 let n=n+2
70 goto 40
80 end
900 print "Counterexample: ";n
910 end
1000 let p=3
1010 if p>n-p then 900
1020 let r=p
1030 let pr=#r
1035 if pr=0 then 1080
1040 let r=n-r
1050 if r=p then 60
1060 goto 1030
1080 let p=p+2
1090 goto 1010
